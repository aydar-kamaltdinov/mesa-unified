/* SPDX-License-Identifier: MIT */
#include "tu_cs.h"
#include "tu_cmd_buffer.h"
#include "tu_device.h"
#include "tu_mesh_dispatch.h"

bool
tu_a8_prepare_mesh_dispatch_state(const tu_cmd_buffer *cmd, const tu_bo *aqe_bo,
                                   uint64_t gmem_base,
                                   tu_a8_mesh_dispatch_state *out)
{
   if (!out)
      return false;
   *out = {};
   if (!cmd || fd_dev_gen(&cmd->device->physical_device->dev_id) != 8 ||
       !aqe_bo || aqe_bo->size < TU_A8_MESH_AQE_SIZE || !aqe_bo->iova)
      return false;
   const tu_shader *mesh = cmd->state.shaders[MESA_SHADER_MESH];
   const tu_draw_state metadata = cmd->state.program.mesh_metadata;
   if (!mesh || !mesh->variant || mesh->variant->type != MESA_SHADER_MESH ||
       !metadata.iova || metadata.size != 20 || (metadata.iova & 3))
      return false;
   out->resource_iova = aqe_bo->iova;
   out->gmem_base = gmem_base;
   out->metadata_iova = metadata.iova;
   out->statistics_enabled = cmd->state.statistics_query_may_be_running;
   return true;
}

static void
emit_mesh_packet(struct tu_cs *cs, const uint32_t *packet, unsigned count)
{
   assert(!cs->pkt);
   /* In particular, CP_COND_EXEC + NOP + view + dispatch must occupy the
    * same IB: reserving once for the complete packet body is required.
    */
   tu_cs_reserve(cs, count);
   tu_cs_emit_array(cs, packet, count);
}

void
tu_a8_emit_mesh_direct_dispatches(struct tu_cs *cs,
                                   const tu_a8_mesh_dispatch_state *state,
                                   const tu_mesh_shader_layout *layout,
                                   uint32_t view_mask,
                                   uint32_t x, uint32_t y, uint32_t z)
{
   /* Original CmdDrawMeshTasksEXT ELF0x94a0a0, dispatch loop only. */
   unsigned ordinal = 0;
   uint32_t views = view_mask ? view_mask : 1;
   do {
      uint32_t packet[40];
      unsigned count = tu_a8_build_mesh_view_state(
         packet, view_mask != 0, __builtin_ctz(views), 0);
      count += tu_a8_build_mesh_dispatch_packet(
         packet + count, state, layout, x, y, z, 0, ordinal & 1);
      emit_mesh_packet(cs, packet, count);
      ordinal++;
      views &= views - 1;
   } while (views);
}

void
tu_a8_emit_mesh_indirect_dispatches(struct tu_cs *cs,
                                     const tu_a8_mesh_dispatch_state *state,
                                     const tu_mesh_shader_layout *layout,
                                     uint32_t view_mask,
                                     uint64_t indirect_iova,
                                     uint32_t draw_count, uint32_t stride,
                                     uint64_t count_iova,
                                     uint64_t always_true_iova)
{
   /* Original Indirect/IndirectCount loops ELF0x94a180/0x94a2ac.
    * A nonzero always_true_iova enables the count predicate and must refer
    * to a command-buffer-owned, initialized UINT32_MAX dword. count_iova is
    * the actual GPU draw-count source, never dereferenced by the host.
    */
   unsigned ordinal = 0;
   for (uint32_t draw = 0; draw < draw_count; draw++) {
      uint32_t views = view_mask ? view_mask : 1;
      do {
         uint32_t packet[47];
         const uint64_t address = indirect_iova + uint64_t(draw) * stride;
         unsigned count;
         if (always_true_iova) {
            count = tu_a8_build_mesh_indirect_count_packet(
               packet, state, layout, view_mask, __builtin_ctz(views),
               draw, address, count_iova, always_true_iova, ordinal & 1);
         } else {
            count = tu_a8_build_mesh_view_state(
               packet, view_mask != 0, __builtin_ctz(views), draw);
            count += tu_a8_build_mesh_dispatch_packet(
               packet + count, state, layout, 0, 0, 0, address, ordinal & 1);
         }
         emit_mesh_packet(cs, packet, count);
         ordinal++;
         views &= views - 1;
      } while (views);
   }
}
