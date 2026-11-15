/* SPDX-License-Identifier: MIT */
#ifndef TU_FRAGMENT_PRIMITIVE_H
#define TU_FRAGMENT_PRIMITIVE_H

#include "tu_shader.h"
#include "compiler/shader_info.h"
#include "compiler/nir/nir.h"
#include "vk_graphics_state.h"

/* Zero leaves the primitive unspecified for a later link. The original
 * builder reads the IA/provoking values without checking these dynamic bits;
 * retaining an unspecified dynamic key here is a local adaptation, not a
 * recovered draw-time variant selector. Keep its final dynamic-state behavior
 * separate from the source reconstruction of the static primitive classes.
 */
static inline unsigned
tu_fragment_primitive_from_topology(VkPrimitiveTopology topology)
{
   switch (topology) {
   case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
      return IR3_FS_POINTS;
   case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
   case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
   case VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY:
   case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY:
      return IR3_FS_LINES;
   case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
   case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
   case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
   case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY:
   case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY:
      return IR3_FS_TRIANGLES;
   default:
      return IR3_FS_UNSPECIFIED;
   }
}

static inline void
tu_fragment_key_from_graphics_state(struct tu_shader_key *key,
                                    const struct vk_graphics_pipeline_state *state)
{
   key->fs_primitive = IR3_FS_UNSPECIFIED;
   key->fs_provoking_vtx_last = false;
   if (state->ia &&
       !BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_IA_PRIMITIVE_TOPOLOGY))
      key->fs_primitive = tu_fragment_primitive_from_topology(
         (VkPrimitiveTopology)state->ia->primitive_topology);
   if (state->rs &&
       !BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_RS_PROVOKING_VERTEX))
      key->fs_provoking_vtx_last =
         state->rs->provoking_vertex == VK_PROVOKING_VERTEX_MODE_LAST_VERTEX_EXT;
}

/* AQE output topology is independent of the input-assembly topology. The
 * cached mesh layout stores its arity; NIR supplies the corresponding enum.
 */
static inline unsigned
tu_fragment_primitive_from_mesh_vertices(unsigned vertices)
{
   switch (vertices) {
   case 1: return IR3_FS_POINTS;
   case 2: return IR3_FS_LINES;
   case 3: return IR3_FS_TRIANGLES;
   default: return IR3_FS_UNSPECIFIED;
   }
}

static inline unsigned
tu_fragment_primitive_from_mesh(enum mesa_prim primitive)
{
   switch (primitive) {
   case MESA_PRIM_POINTS: return IR3_FS_POINTS;
   case MESA_PRIM_LINES: return IR3_FS_LINES;
   case MESA_PRIM_TRIANGLES: return IR3_FS_TRIANGLES;
   default: return IR3_FS_UNSPECIFIED;
   }
}

static inline unsigned
tu_fragment_primitive_from_producer(unsigned input_primitive,
                                    const struct shader_info *gs,
                                    const struct shader_info *tes)
{
   if (gs) {
      switch (gs->gs.output_primitive) {
      case MESA_PRIM_POINTS:
         return IR3_FS_POINTS;
      case MESA_PRIM_LINE_STRIP:
         return IR3_FS_LINES;
      case MESA_PRIM_TRIANGLE_STRIP:
         return IR3_FS_TRIANGLES;
      default:
         return IR3_FS_UNSPECIFIED;
      }
   }
   if (tes) {
      if (tes->tess.point_mode)
         return IR3_FS_POINTS;
      switch (tes->tess._primitive_mode) {
      case TESS_PRIMITIVE_ISOLINES:
         return IR3_FS_LINES;
      case TESS_PRIMITIVE_TRIANGLES:
      case TESS_PRIMITIVE_QUADS:
         return IR3_FS_TRIANGLES;
      default:
         return IR3_FS_UNSPECIFIED;
      }
   }
   return input_primitive;
}

/* Retain the pre-IR3 NIR only for fragment shaders whose code depends on the
 * primitive delivered at link time. In particular, point/line coordinate
 * variants need this even though they do not allocate a triangle VPC basis.
 */
static inline bool
tu_fragment_shader_uses_primitive_key(const nir_shader *nir)
{
   if (nir->info.stage != MESA_SHADER_FRAGMENT)
      return false;
   if (BITSET_TEST(nir->info.system_values_read, SYSTEM_VALUE_BARYCENTRIC_PERSP_COORD) ||
       BITSET_TEST(nir->info.system_values_read, SYSTEM_VALUE_BARYCENTRIC_LINEAR_COORD))
      return true;
   nir_foreach_function_impl(impl, nir) {
      nir_foreach_block(block, impl) {
         nir_foreach_instr(instr, block) {
            if (instr->type != nir_instr_type_intrinsic)
               continue;
            nir_intrinsic_instr *intr = nir_instr_as_intrinsic(instr);
            switch (intr->intrinsic) {
            case nir_intrinsic_load_deref: {
               nir_variable *var = nir_intrinsic_get_var(intr, 0);
               if (var && var->data.mode == nir_var_shader_in &&
                   (var->data.per_vertex ||
                    var->data.interpolation == INTERP_MODE_EXPLICIT))
                  return true;
               break;
            }
            case nir_intrinsic_load_input_vertex:
            case nir_intrinsic_load_per_vertex_input:
            case nir_intrinsic_load_barycentric_coord_pixel:
            case nir_intrinsic_load_barycentric_coord_centroid:
            case nir_intrinsic_load_barycentric_coord_sample:
            case nir_intrinsic_load_barycentric_coord_at_offset:
            case nir_intrinsic_load_barycentric_coord_at_sample:
            case nir_intrinsic_load_barycentric_coord_ir3:
               return true;
            default:
               break;
            }
         }
      }
   }
   return false;
}

static inline bool
tu_shader_uses_primitive_key(const struct tu_shader *shader)
{
   return shader && shader->variant &&
          shader->variant->type == MESA_SHADER_FRAGMENT &&
          shader->fs.uses_primitive_key;
}

struct tu_fragment_primitive_cntl {
   uint32_t pc;
   uint32_t vpc;
};

static inline bool
tu_fragment_uses_vertex_ordering(bool a8xx,
                                const struct ir3_shader_variant *fs)
{
   return a8xx && fs &&
          (fs->has_per_vertex_input || fs->has_barycentric_coord);
}

static inline struct tu_fragment_primitive_cntl
tu_fragment_primitive_cntl(uint32_t base, bool last, bool vertex_ordering)
{
   struct tu_fragment_primitive_cntl result = { base, base };
   if (vertex_ordering) {
      /* a6xx_pc_cntl: PROVOKING_VTX_LAST=bit1, D3D_VERTEX_ORDERING=bit2.
       * VPC uses first-vertex ordering; the shader reads the last vertex
       * explicitly with LDLV when required.
       */
      uint32_t other = base & ~(BIT(1) | BIT(2));
      result.pc = other | (last ? BIT(1) : BIT(2));
      result.vpc = other | BIT(2);
   }
   return result;
}

#endif
