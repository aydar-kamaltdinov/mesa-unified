/* SPDX-License-Identifier: MIT */
#include "tu_mesh_vpc.h"
#include <string.h>

bool
tu_a8_build_mesh_fs_inputs(const tu_mesh_shader_layout *m,
                           ir3_mesh_fs_inputs *out)
{
   if (!m || !out)
      return false;
   ir3_mesh_fs_inputs result = {};
   result.enabled = true;
   if (m->uses_primitive_id) {
      result.primitive_base =
         (unsigned(m->primitive_attributes) + m->vertex_attributes_before_specials) * 4;
   } else if (m->primitive_attributes) {
      unsigned base = unsigned(m->vertex_attributes) * 4 + 4;
      if (base + (m->primitive_stride >> 2) > 128)
         base -= 4;
      result.primitive_base = base;
   }
   if (m->has_layer || m->has_viewport)
      result.layer_viewport_base = unsigned(m->vertex_attributes) * 4;

   for (unsigned slot = 0; slot < ARRAY_SIZE(result.location_plus_one); slot++) {
      bool mapped = (slot >= VARYING_SLOT_VAR0 && slot < VARYING_SLOT_VAR0 + 32) ||
         (slot >= VARYING_SLOT_CLIP_DIST0 && slot <= VARYING_SLOT_CULL_DIST1) ||
         slot == VARYING_SLOT_PRIMITIVE_ID;
      if (!mapped)
         continue;
      unsigned location;
      /* Native tu_compile_shaders ELF0xa0c324..0xa0c378 selects the
       * vertex mapping whenever it exists. Clip/cull maps use only the
       * vertex interface; PrimitiveID has its own guarded primitive entry.
       */
      if (slot == VARYING_SLOT_PRIMITIVE_ID) {
         if (!m->uses_primitive_id)
            continue;
         if (m->primitive_offset[slot] == UINT16_MAX)
            return false;
         location = result.primitive_base + (m->primitive_offset[slot] >> 2);
      } else if (m->vertex_offset[slot] != UINT16_MAX) {
         location = unsigned(m->vertex_register[slot]) * 4 +
                    m->vertex_component[slot];
      } else if (slot >= VARYING_SLOT_VAR0 &&
                 slot < VARYING_SLOT_VAR0 + 32 &&
                 m->primitive_offset[slot] != UINT16_MAX) {
         location = result.primitive_base + (m->primitive_offset[slot] >> 2);
      } else {
         continue;
      }
      if (location >= 128)
         return false;
      result.location_plus_one[slot] = location + 1;
   }
   if (result.primitive_base > 128 || result.layer_viewport_base > 128)
      return false;
   *out = result;
   return true;
}

bool
tu_a8_mesh_fs_inputs_match(const tu_mesh_shader_layout *m,
                            const ir3_mesh_fs_inputs *compiled)
{
   ir3_mesh_fs_inputs expected;
   return compiled && compiled->enabled &&
      tu_a8_build_mesh_fs_inputs(m, &expected) &&
      compiled->primitive_base == expected.primitive_base &&
      compiled->layer_viewport_base == expected.layer_viewport_base &&
      !memcmp(compiled->location_plus_one, expected.location_plus_one,
              sizeof(expected.location_plus_one));
}

static unsigned
component_extent(unsigned mask)
{
   return mask ? 32 - __builtin_clz(mask) : 0;
}

bool
tu_a8_prepare_mesh_vpc(const tu_mesh_shader_layout *m,
                       const ir3_shader_variant *fs, tu_a8_mesh_vpc_plan *out)
{
   if (!m || !out || (fs && (fs->type != MESA_SHADER_FRAGMENT ||
                            fs->inputs_count > ARRAY_SIZE(fs->inputs))))
      return false;

   memset(out, 0, sizeof(*out));
   out->has_fs = fs != nullptr;
   out->clip_mask = m->clip_mask;
   out->cull_mask = m->cull_mask;

   auto add_output = [&](unsigned slot, unsigned regid) {
      if (regid >= 128 ||
          out->producer_output_count == ARRAY_SIZE(out->producer_outputs))
         return false;
      auto &output = out->producer_outputs[out->producer_output_count++];
      output.slot = slot;
      output.regid = regid;
      return true;
   };

   /* Special records precede the ordinary layout table, as in the original.
    * In particular viewport occupies component1, not component0.
    */
   if (m->has_layer) {
      if (!add_output(VARYING_SLOT_LAYER, unsigned(m->layer_register) << 2))
         return false;
      out->skip_output_slots |= UINT64_C(1) << VARYING_SLOT_LAYER;
   }
   if (m->has_viewport) {
      if (!add_output(VARYING_SLOT_VIEWPORT,
                      (unsigned(m->viewport_register) << 2) | 1))
         return false;
      out->skip_output_slots |= UINT64_C(1) << VARYING_SLOT_VIEWPORT;
   }
   for (unsigned slot = 0; slot < 64; slot++) {
      if (m->vertex_offset[slot] == UINT16_MAX)
         continue;
      if (!add_output(slot, (unsigned(m->vertex_register[slot]) << 2) |
                            (m->vertex_component[slot] & 3)))
         return false;
   }

   out->primitive_base = m->primitive_attributes ? 4 : 0;
   if (!fs)
      return true;

   const auto &compiled_inputs = fs->shader_options.mesh_fs_inputs;
   /* The immutable map identifies the producer used by the compiler.
    * Native VPC still remaps a separate borrowed copy for every mesh FS,
    * including variants without the compiler's per-vertex compaction.
    */
   if (compiled_inputs.enabled && !tu_a8_mesh_fs_inputs_match(m, &compiled_inputs))
      return false;

   if (m->uses_primitive_id) {
      /* The original checks the first named input before bary/sysval filtering. */
      for (unsigned i = 0; i < fs->inputs_count; i++) {
         if (fs->inputs[i].slot == VARYING_SLOT_PRIMITIVE_ID) {
            out->primitive_id_per_vertex = !fs->inputs[i].per_primitive;
            break;
         }
      }
   }
   if (out->primitive_id_per_vertex &&
       m->vertex_register[VARYING_SLOT_POS] != UINT8_MAX) {
      out->initial_location = unsigned(m->vertex_register[VARYING_SLOT_POS]) << 2;
      if (out->initial_location >= 128)
         return false;
   }

   if (m->primitive_attributes) {
      unsigned first = UINT32_MAX;
      for (unsigned i = 0; i < fs->inputs_count; i++) {
         const auto &input = fs->inputs[i];
         if (input.per_primitive ||
             (m->uses_primitive_id && input.slot == VARYING_SLOT_PRIMITIVE_ID))
            first = MIN2(first, unsigned(input.inloc));
      }
      out->primitive_base = first != UINT32_MAX ? first :
         (m->uses_primitive_id ?
            (m->vertex_attributes_before_specials + m->primitive_attributes) * 4 :
            m->vertex_attributes * 4 + 4);
   } else {
      out->primitive_base = 0;
   }

   out->full_fs = *fs;
   out->full_fs.total_in = 0;
   out->full_fs.varying_in = 0;
   for (unsigned i = 0; i < fs->inputs_count; i++) {
      auto &input = out->full_fs.inputs[i];
      if (input.sysval)
         continue;
      if (input.slot >= 64 || input.compmask > 0xf)
         return false;

      unsigned location = input.inloc;
      if (!input.per_primitive && input.slot != VARYING_SLOT_PRIMITIVE_ID) {
         if (m->vertex_offset[input.slot] != UINT16_MAX)
            location = unsigned(m->vertex_register[input.slot]) * 4 +
                       m->vertex_component[input.slot];
      } else if (m->primitive_offset[input.slot] != UINT16_MAX) {
         location = out->primitive_base + (m->primitive_offset[input.slot] >> 2);
      }
      /* Absent outputs retain their original FS location. The original
       * linkage skips entries without a producer register; this remap does
       * not promise defined values for unwritten varyings.
       */
      unsigned end = location + component_extent(input.compmask);
      if (location >= 128 || end > 128 ||
          (input.bary && out->full_fs.has_per_vertex_input && location + 4 > 128))
         return false;
      input.inloc = location;
      out->full_fs.total_in = MAX2(out->full_fs.total_in, end);
      if (input.bary)
         out->full_fs.varying_in = MAX2(out->full_fs.varying_in, end);
   }

   out->vertex_fs = out->full_fs;
   out->vertex_fs.inputs_count = 0;
   out->vertex_fs.total_in = 0;
   out->vertex_fs.varying_in = 0;
   /* The original filtered copy clears reads_primid (native offset0x560),
    * not has_per_vertex_input (0x572). Retain the compiler's latter flag:
    * it still selects VPC alignment/padding and the A8 register0x925a write
    * when all per-vertex records were removed by the primitive filter.
    */
   out->vertex_fs.reads_primid = false;
   for (unsigned i = 0; i < out->full_fs.inputs_count; i++) {
      const auto &input = out->full_fs.inputs[i];
      if (input.per_primitive ||
          (out->primitive_id_per_vertex && input.slot == VARYING_SLOT_PRIMITIVE_ID) ||
          (m->has_layer && input.slot == VARYING_SLOT_LAYER) ||
          (m->has_viewport && input.slot == VARYING_SLOT_VIEWPORT))
         continue;
      out->vertex_fs.inputs[out->vertex_fs.inputs_count++] = input;
      /* Original filtered copy computes both ranges from fetched varyings. */
      if (input.bary && !input.sysval) {
         unsigned end = input.inloc + component_extent(input.compmask);
         out->vertex_fs.total_in = MAX2(out->vertex_fs.total_in, end);
         out->vertex_fs.varying_in = MAX2(out->vertex_fs.varying_in, end);
      }
   }
   return true;
}

bool
tu_a8_build_mesh_vpc_maps(const tu_a8_mesh_vpc_link *links, unsigned count,
                          uint64_t skip_output_slots, tu_a8_mesh_vpc_maps *out)
{
   if (!out || (count && !links))
      return false;
   memset(out, 0, sizeof(*out));

   /* Original tu6_emit_vpc A8 ELF0x9c6d74 merges every semantic output sharing
    * a physical vec4. In particular two packed scalar varyings must not become
    * two separate SP output records. Ascending physical-register order matters.
    * Current staging retains the32-record/128-scalar checked envelope.
    * Record capacity alone does not prove a native register-ID limit.
    */
   uint8_t mask[64] = {};
   for (unsigned i = 0; i < count; i++) {
      const auto &link = links[i];
      if (link.slot < 64 && (skip_output_slots & (UINT64_C(1) << link.slot)))
         continue;
      unsigned shifted = unsigned(link.compmask) << (link.regid & 3);
      /* Original ELF0x9c8438..9c845c tests the low byte of the merged
       * mask before keeping its low nibble in the output record. Keep that
       * order: a nonzero high nibble still produces a zero-compmask record.
       */
      mask[link.regid >> 2] |= shifted;
   }
   for (unsigned reg = 0; reg < ARRAY_SIZE(mask); reg++) {
      if (!mask[reg])
         continue;
      if (reg >= 32 || out->count == 32)
         return false;
      unsigned i = out->count++;
      out->output[i / 2] |= ((reg << 2) | ((unsigned(mask[reg]) & 0xfu) << 8)) <<
                            ((i & 1) * 16);
      out->destination[i / 4] |= (reg << 2) << ((i & 3) * 8);
   }
   return true;
}


VkResult
tu_a8_upload_mesh_vpc(tu_cs *sub_cs, const tu_mesh_shader_layout *layout,
                       const ir3_shader_variant *fs, tu_a8_mesh_vpc_upload *out)
{
   if (!sub_cs || !out)
      return VK_ERROR_FEATURE_NOT_PRESENT;
   *out = {};
   tu_cs cs;
   /* Conservative reserve for current32-map/128-component staging limits.
    * Reconcile the bound when extending those limits during final validation.
    */
   VkResult result = tu_cs_begin_sub_stream(sub_cs, 1024, &cs);
   if (result != VK_SUCCESS)
      return result;
   const uint32_t *map = cs.cur;
   bool supported = tu_a8_emit_mesh_vpc(&cs, layout, fs);
   tu_draw_state state = tu_cs_end_draw_state(sub_cs, &cs);
   result = tu_cs_get_status(sub_cs);
   if (result != VK_SUCCESS)
      return result;
   if (!supported)
      return VK_ERROR_FEATURE_NOT_PRESENT;
   /* The entire failed stream stays unpublished. All allocations continue to
    * belong to sub_cs; never expose a partially emitted register sequence.
    */
   *out = {map, state.size, state};
   return VK_SUCCESS;
}
