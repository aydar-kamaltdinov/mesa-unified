/* SPDX-License-Identifier: MIT */
#include "tu_mesh_program.h"
#include "tu_mesh_vpc.h"

#include "ir3/ir3_shader.h"
#include <string.h>

/* GameSir ELF0xc1154,128 bytes copied by the mesh branch of
 * tu_emit_program_state (ELF0x9e9070). AQE supplies the output registers;
 * this passthrough program terminates immediately, with zero padding.
 */
static const uint32_t a8xx_mesh_passthrough_vs[32] = {
   0, 0x03000000,
};

static unsigned
last_bit(uint32_t value)
{
   return value ? 32 - __builtin_clz(value) : 0;
}

static uint32_t
local_size_word(const ir3_shader_variant *v)
{
   const uint32_t x = v->local_size[0];
   const uint32_t y = v->local_size[1];
   const uint32_t z = v->local_size[2];
   const uint32_t dimensions = z > 1 ? 3 : (y > 1 ? 2 : 1);
   return ((y - 1) << 12) | ((z - 1) << 22) |
          ((dimensions | (x << 2)) - 4);
}

static bool
valid_local_size(const ir3_shader_variant *v)
{
   return v && v->local_size[0] && v->local_size[1] && v->local_size[2] &&
          !v->local_size_variable;
}

bool
tu_a8_build_mesh_program_plan(const tu_a8_mesh_program_inputs *in,
                               tu_a8_mesh_program_plan *out)
{
   memset(out, 0, sizeof(*out));
   const tu_mesh_shader_layout *m = in->layout;
   if (!m || !valid_local_size(in->mesh) ||
       in->mesh->type != MESA_SHADER_MESH ||
       (in->task && in->task->type != MESA_SHADER_TASK) ||
       (in->task && !valid_local_size(in->task)) ||
       (in->fs_input_count && !in->fs_inputs) ||
       m->vertex_attributes > 32 || m->vertices_per_primitive < 1 ||
       m->vertices_per_primitive > 3)
      return false;

   const bool mapped_fs = in->compiled_fs_inputs &&
                          in->compiled_fs_inputs->enabled;
   if (mapped_fs && (!in->has_fragment_shader ||
                     !tu_a8_mesh_fs_inputs_match(m, in->compiled_fs_inputs)))
      return false;

   /* Preserve the original per-register component extent: several packed
    * slots can contribute to one output. Offsets stay in the full layout.
    */
   uint8_t extent[32] = {};
   uint16_t shared_offset[32];
   bool used[32] = {};
   memset(shared_offset, 0xff, sizeof(shared_offset));
   for (unsigned slot = 0; slot < 64; slot++) {
      unsigned reg = m->vertex_register[slot];
      if (reg == 0xff)
         continue;
      if (reg >= m->vertex_attributes)
         return false;
      unsigned width = m->vertex_components[slot] + m->vertex_component[slot];
      if (width > 4)
         return false; /* The restored producer packs at most four components. */
      used[reg] = true;
      if (width > extent[reg])
         extent[reg] = width;
      if (m->vertex_offset[slot] < shared_offset[reg])
         shared_offset[reg] = m->vertex_offset[slot];
   }

   uint16_t outputs[32] = {};
   uint8_t destinations[32] = {};
   unsigned output_count = 0;
   for (unsigned reg = 0; reg < m->vertex_attributes; reg++) {
      if (!extent[reg])
         continue;
      destinations[output_count] = reg << 2;
      outputs[output_count++] = (reg << 2) | (((1u << extent[reg]) - 1) << 8);
   }

   auto add = [&](uint32_t reg, uint32_t value) {
      assert(out->reg_count < ARRAY_SIZE(out->regs));
      out->regs[out->reg_count++] = { .reg = reg, .value = value };
   };
   const bool psize = m->vertex_offset[VARYING_SLOT_PSIZ] != UINT16_MAX;
   if (psize && !m->vertex_attributes)
      return false;
   const uint32_t config = 0x100 |
      uint32_t(in->mesh->bindless_tex) |
      (uint32_t(in->mesh->bindless_samp) << 1) |
      (uint32_t(in->mesh->bindless_ibo) << 2) |
      (uint32_t(in->mesh->bindless_ubo) << 3) |
      ((in->mesh->num_samp & 0xff) << 9) |
      ((in->mesh->num_samp & 0x1f) << 17) |
      ((ir3_shader_num_uavs(in->mesh) & 0x7f) << 22);
   add(0xa827, 0x100);
   add(0xa823, config); /* SP_VS_CONFIG inherits mesh resource usage. */
   add(0xa81c, 0); /* Patched to the uploaded passthrough IOVA below. */
   add(0xa81d, 0);
   add(0xa800, (psize ? 0x08002000 : 0x2000) |
               (uint32_t(m->vertex_attributes) - uint32_t(psize)) * 0x80);
   add(0xa802, output_count);
   for (unsigned i = 0; i < (output_count + 1) / 2; i++)
      add(0xa803 + i, outputs[2 * i] |
          ((2 * i + 1 < output_count ? uint32_t(outputs[2 * i + 1]) : 0) << 16));
   for (unsigned i = 0; i < (output_count + 3) / 4; i++) {
      uint32_t value = 0;
      for (unsigned j = 0; j < 4 && i * 4 + j < output_count; j++)
         value |= uint32_t(destinations[i * 4 + j]) << (j * 8);
      add(0xa813 + i, value);
   }
   add(0xa81b, 0);
   add(0xa824, 1);
   add(0x9b0c, 0);
   unsigned views = last_bit(in->view_mask ? in->view_mask | 1 : 0);
   const uint32_t stereo = views > 1 ? 3 | (views << 2) : 0;
   add(0x9b09, stereo);
   add(0xa008, stereo);
   add(0x931a, stereo);
   if (in->view_mask) {
      add(0x9b0d, in->view_mask);
      add(0x90c1, in->view_mask);
   }

   auto special_reg = [&](unsigned slot, unsigned shift, uint32_t absent) {
      return m->vertex_offset[slot] == UINT16_MAX ? absent :
         (uint32_t(m->vertex_register[slot]) << (shift + 2)) |
         ((uint32_t(m->vertex_component[slot]) & 3) << shift);
   };
   const uint32_t clip_cull = m->clip_mask | m->cull_mask;
   add(0x9b02, (m->vertex_stride >> 2) | (uint32_t(m->has_shading_rate) << 24) |
               (uint32_t(m->has_viewport) << 10) | (uint32_t(psize) << 8) |
               (clip_cull << 16) | ((m->has_layer || m->synthesize_layer) ? 0x200 : 0));
   add(0x9300, special_reg(VARYING_SLOT_POS, 8, 0xff00) |
               special_reg(VARYING_SLOT_PSIZ, 16, 0xff0000) |
               (m->vertex_stride >> 2));
   add(0x9307, special_reg(VARYING_SLOT_CLIP_DIST0, 8, 0xff00) |
               special_reg(VARYING_SLOT_CLIP_DIST1, 16, 0xff0000) | clip_cull);
   add(0x930a, (m->has_viewport ? (uint32_t(m->viewport_register) << 10) | 0x100 : 0xff00) |
               ((m->has_layer || m->synthesize_layer) ?
                  (uint32_t(m->layer_register) << 2) | 0xff0000 : 0xff00ff));
   static const uint32_t fixed[][2] = {
      {0x8809, 0x400}, {0x9240, 0x550000},
      {0x9241, 0}, {0x9242, 0}, {0x9243, 0}, {0x9244, 0},
      {0x9245, 0}, {0x9246, 0}, {0x9247, 0},
   };
   for (const auto &r : fixed)
      add(r[0], r[1]);

   /* Per-primitive FS input span, including the primitive-ID reservation. */
   uint32_t first = UINT32_MAX, end = 0;
   for (unsigned i = 0; i < in->fs_input_count; i++) {
      const auto &input = in->fs_inputs[i];
      if (input.compmask > 0xf)
         return false;
      if (input.per_primitive || (m->uses_primitive_id && input.slot == VARYING_SLOT_PRIMITIVE_ID)) {
         if (input.inloc < first)
            first = input.inloc;
         unsigned last = input.inloc + last_bit(input.compmask);
         if (last > end)
            end = last;
      }
   }
   uint32_t span = first == UINT32_MAX ? 0 : end - first;
   out->primitive_start = m->primitive_attributes ? 4 : 0;
   if (in->has_fragment_shader && m->primitive_attributes) {
      out->primitive_start = first != UINT32_MAX ? first :
         (m->uses_primitive_id ?
            (m->vertex_attributes_before_specials + m->primitive_attributes) * 4 :
            m->vertex_attributes * 4 + 4);
   }
   if (!in->has_fragment_shader) {
      span = 0;
      for (unsigned slot = 0; slot < 64; slot++) {
         if (m->primitive_offset[slot] != UINT16_MAX)
            span += m->primitive_components[slot] ? m->primitive_components[slot] : 4;
      }
   } else if (span && m->uses_primitive_id) {
      for (unsigned i = 0; i < in->fs_input_count; i++) {
         const auto &input = in->fs_inputs[i];
         if (input.slot == VARYING_SLOT_PRIMITIVE_ID) {
            if (!input.per_primitive && span < 5)
               span = 4;
            break;
         }
      }
   }
   out->primitive_override = span != 0;
   out->primitive_span = span;
   if (out->primitive_start + span > 128)
      return false;
   out->vertex_stride = m->vertex_stride;
   out->primitive_stride = m->primitive_stride;

   /* Original four-entry floating-point vertex-fetch format table ELF0xae650. */
   static const uint32_t formats[4] = {0x04b00000, 0x06800000, 0x07200000, 0x08300000};
   for (unsigned reg = 0; reg < m->vertex_attributes; reg++) {
      if (!used[reg])
         continue;
      unsigned width = reg == m->vertex_register[VARYING_SLOT_PSIZ] ? 1 : extent[reg];
      if (!width)
         width = 4;
      unsigned i = out->fetch_count++;
      out->fetch[i] = formats[width - 1] | (uint32_t(shared_offset[reg]) << 5) | 0x40000000;
      out->destination[i] = ((1u << width) - 1) | (reg << 6);
   }

   uint32_t *data = out->metadata;
   const bool has_task = in->task != NULL;
   data[0] = uint32_t(has_task) | (uint32_t(!in->has_fragment_shader) << 1) |
      (m->uses_cull_bitmap ? 4 : 0) | (m->primitive_attributes ? 8 : 0) |
      (m->vertex_offset[VARYING_SLOT_POS] == UINT16_MAX ? 16 : 0) |
      (m->vertices_per_primitive == 2 ? 32 : 0);
   if (has_task) {
      data[1] = uint32_t(in->task_upload.iova);
      data[2] = uint32_t(in->task_upload.iova >> 32);
      data[3] = in->task_upload.size;
   }
   data[4] = uint32_t(in->mesh_upload.iova);
   data[5] = uint32_t(in->mesh_upload.iova >> 32);
   data[6] = in->mesh_upload.size;
   data[7] = (m->index_stride * m->max_primitives) >> 1;
   data[9] = m->vertex_stride * m->max_vertices;
   data[11] = m->primitive_stride * m->max_primitives;
   data[13] = m->uses_cull_bitmap ? 0x20 : 0;
   data[15] = m->max_vertices;
   data[16] = m->max_primitives;
   data[17] = (m->has_viewport ? 3 : 1) |
      ((m->has_layer || m->synthesize_layer) ? 4 : 0) | (m->has_shading_rate ? 8 : 0);
   data[18] = has_task ? local_size_word(in->task) : 0;
   data[19] = local_size_word(in->mesh);
   return true;
}

static void
emit_reg(tu_cs *cs, uint16_t reg, uint32_t value)
{
   tu_cs_emit_pkt4(cs, reg, 1);
   tu_cs_emit(cs, value);
}

VkResult
tu_a8_upload_mesh_program(tu_cs *sub_cs, const tu_a8_mesh_program_inputs *in,
                           tu_a8_mesh_program_upload *out)
{
   *out = {};
   tu_a8_mesh_program_plan plan;
   if (!tu_a8_build_mesh_program_plan(in, &plan))
      return VK_ERROR_FEATURE_NOT_PRESENT;

   tu_a8_mesh_program_upload upload = {};
   VkResult result = tu_cs_alloc(sub_cs, 1, 32, &upload.passthrough);
   if (result != VK_SUCCESS)
      return result;
   memcpy(upload.passthrough.map, a8xx_mesh_passthrough_vs, sizeof(a8xx_mesh_passthrough_vs));
   plan.regs[2].value = uint32_t(upload.passthrough.iova);
   plan.regs[3].value = uint32_t(upload.passthrough.iova >> 32);

   tu_cs cs;
   result = tu_cs_begin_sub_stream(sub_cs, 512, &cs);
   if (result != VK_SUCCESS)
      return result;
   upload.setup_map = cs.cur;
   {
      tu_crb crb = cs.crb(plan.reg_count);
      for (unsigned i = 0; i < plan.reg_count; i++)
         crb.add(plan.regs[i]);
   }
   if (plan.primitive_override) {
      emit_reg(&cs, 0x9b07, 1);
      emit_reg(&cs, 0x927b, plan.primitive_stride);
      emit_reg(&cs, 0x9311, plan.primitive_span | (plan.primitive_start << 8) | 0x10000);
   }
   emit_reg(&cs, 0xa006, 0xfc00);
   emit_reg(&cs, 0x8240, 0);
   emit_reg(&cs, 0x9306, 3);
   emit_reg(&cs, 0xa000, plan.fetch_count << 8);
   emit_reg(&cs, 0xa001, 0xfcfcfcfc);
   upload.vpc_split_dwords = cs.cur - upload.setup_map;
   emit_reg(&cs, 0xa009, 3);
   emit_reg(&cs, 0xa00a, 0xfcfcfcfc);
   emit_reg(&cs, 0xa013, plan.vertex_stride);
   for (unsigned i = 0; i < plan.fetch_count; i++)
      emit_reg(&cs, 0xa090 + 2 * i, plan.fetch[i]);
   if (plan.fetch_count) {
      tu_cs_emit_pkt4(&cs, 0xa0d0, plan.fetch_count);
      tu_cs_emit_array(&cs, plan.destination, plan.fetch_count);
   }
   upload.setup_dwords = cs.cur - upload.setup_map;
   upload.draw = tu_cs_end_draw_state(sub_cs, &cs);
   result = tu_cs_get_status(sub_cs);
   if (result != VK_SUCCESS)
      return result;

   upload.binning = upload.draw;
   if (in->layout->primitive_attributes) {
      result = tu_cs_begin_sub_stream(sub_cs, upload.setup_dwords + 6, &cs);
      if (result != VK_SUCCESS)
         return result;
      tu_cs_emit_array(&cs, upload.setup_map, upload.setup_dwords);
      emit_reg(&cs, 0x9b07, 0);
      emit_reg(&cs, 0x927b, 0);
      emit_reg(&cs, 0x9311, 0);
      upload.binning = tu_cs_end_draw_state(sub_cs, &cs);
   }

   tu_cs_memory metadata;
   result = tu_cs_alloc(sub_cs, ARRAY_SIZE(plan.metadata), 1, &metadata);
   if (result != VK_SUCCESS)
      return result;
   memcpy(metadata.map, plan.metadata, sizeof(plan.metadata));
   upload.metadata = { .iova = metadata.iova, .size = ARRAY_SIZE(plan.metadata),
                        .writeable = metadata.writeable };
   if ((result = tu_cs_get_status(sub_cs)) != VK_SUCCESS)
      return result;
   *out = upload;
   return VK_SUCCESS;
}

bool
tu_a8_collect_mesh_fs_inputs(const ir3_shader_variant *fs,
                              tu_a8_mesh_fs_input *out,
                              unsigned capacity, unsigned *count)
{
   *count = 0;
   if (!fs)
      return true;
   if (fs->type != MESA_SHADER_FRAGMENT)
      return false;
   for (unsigned i = 0; i < fs->inputs_count; i++) {
      if (!fs->inputs[i].bary)
         continue;
      if (*count == capacity)
         return false;
      out[(*count)++] = {
         .slot = fs->inputs[i].slot,
         .compmask = fs->inputs[i].compmask,
         .inloc = fs->inputs[i].inloc,
         .per_primitive = fs->inputs[i].per_primitive,
      };
   }
   return true;
}

VkResult
tu_a8_splice_mesh_program_state(tu_cs *sub_cs,
                                 const tu_a8_mesh_program_upload *setup,
                                 const uint32_t *fs, unsigned fs_dwords,
                                 const uint32_t *vpc, unsigned vpc_dwords,
                                 tu_draw_state *out)
{
   *out = {};
   if (!setup->setup_map || setup->vpc_split_dwords > setup->setup_dwords ||
       (fs_dwords && !fs) || (vpc_dwords && !vpc))
      return VK_ERROR_FEATURE_NOT_PRESENT;
   uint64_t count = uint64_t(setup->setup_dwords) + fs_dwords + vpc_dwords;
   if (count > UINT16_MAX)
      return VK_ERROR_OUT_OF_DEVICE_MEMORY;

   /* Original tu_emit_program_state tail: FS state is inserted between the
    * initial AQE/VPC setup and the generated mesh VPC state. VFD fetch state
    * then follows, so subsequent regular FS/VPC/VS groups must not overwrite
    * it. The caller publishes the combined state and clears the regular
    * groups; this helper only assembles their shared sub-CS allocation.
    */
   tu_cs cs;
   VkResult result = tu_cs_begin_sub_stream(sub_cs, count, &cs);
   if (result != VK_SUCCESS)
      return result;
   tu_cs_emit_array(&cs, setup->setup_map, setup->vpc_split_dwords);
   if (fs_dwords)
      tu_cs_emit_array(&cs, fs, fs_dwords);
   if (vpc_dwords)
      tu_cs_emit_array(&cs, vpc, vpc_dwords);
   tu_cs_emit_array(&cs, setup->setup_map + setup->vpc_split_dwords,
                    setup->setup_dwords - setup->vpc_split_dwords);
   tu_draw_state state = tu_cs_end_draw_state(sub_cs, &cs);
   result = tu_cs_get_status(sub_cs);
   if (result == VK_SUCCESS)
      *out = state;
   return result;
}


VkResult
tu_a8_upload_linked_mesh_program(tu_cs *sub_cs,
                                  const tu_a8_mesh_program_inputs *in,
                                  const ir3_shader_variant *fs,
                                  const uint32_t *fs_state, unsigned fs_dwords,
                                  tu_a8_linked_mesh_program *out)
{
   if (!out)
      return VK_ERROR_FEATURE_NOT_PRESENT;
   *out = {};
   if (!sub_cs || !in || (fs_dwords && !fs_state) ||
       (!fs && fs_dwords) ||
       (fs && (fs->type != MESA_SHADER_FRAGMENT ||
               fs->inputs_count > ARRAY_SIZE(fs->inputs))))
      return VK_ERROR_FEATURE_NOT_PRESENT;

   /* Derive both setup and final VPC ranges from the same real FS. This
    * wrapper must not accept an unrelated caller-prepared primitive list.
    * Unlike the earlier live-varying collector, retain non-bary user inputs
    * as well: the original range scans inspect all primitive input records.
    */
   tu_a8_mesh_fs_input inputs[ARRAY_SIZE(((ir3_shader_variant *)nullptr)->inputs)];
   unsigned count = 0;
   if (fs) {
      for (unsigned i = 0; i < fs->inputs_count; i++) {
         const auto &input = fs->inputs[i];
         inputs[count++] = {input.slot, input.compmask, input.inloc, input.per_primitive};
      }
   }
   tu_a8_mesh_program_inputs selected = *in;
   selected.fs_inputs = inputs;
   selected.fs_input_count = count;
   selected.has_fragment_shader = fs != nullptr;
   selected.compiled_fs_inputs = fs ? &fs->shader_options.mesh_fs_inputs : nullptr;

   tu_a8_linked_mesh_program linked = {};
   VkResult result = tu_a8_upload_mesh_program(sub_cs, &selected, &linked.setup);
   if (result != VK_SUCCESS)
      return result;
   tu_a8_mesh_vpc_upload vpc;
   result = tu_a8_upload_mesh_vpc(sub_cs, selected.layout, fs, &vpc);
   if (result != VK_SUCCESS)
      return result;
   result = tu_a8_splice_mesh_program_state(sub_cs, &linked.setup,
                                           fs_state, fs_dwords,
                                           vpc.map, vpc.dwords, &linked.combined);
   if (result == VK_SUCCESS)
      *out = linked;
   return result;
}
