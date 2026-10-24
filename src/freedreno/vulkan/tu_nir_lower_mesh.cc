/* SPDX-License-Identifier: MIT */
#include "nir/nir_builder.h"
#include "tu_nir_mesh.h"

/* Recovered A8 AQE task ABI. These slots belong to the task stage, not the
 * compute user-constant layout. Mesh dispatch/state support must be present
 * before exposing this stage to applications.
 */
static nir_def *
aqe_const(nir_builder *b, unsigned slot, unsigned components = 1)
{
   assert(slot + components <= TU_AQE_CONST_PREFIX_VEC4 * 4);
   return nir_load_const_ir3(b, components, 32, nir_imm_int(b, 0), .base = (int) slot);
}

static bool
lower_mesh_fragment_view_index(nir_builder *b, nir_intrinsic_instr *intr, void *)
{
   if (intr->intrinsic != nir_intrinsic_load_view_index)
      return false;
   b->cursor = nir_before_instr(&intr->instr);
   nir_def_replace(&intr->def, aqe_const(b, 33));
   return true;
}

bool
tu_nir_lower_mesh_fragment_view_index(nir_shader *nir)
{
   assert(nir->info.stage == MESA_SHADER_FRAGMENT);
   return nir_shader_intrinsics_pass(nir, lower_mesh_fragment_view_index,
                                     nir_metadata_control_flow, NULL);
}

static nir_def *
aqe_pointer(nir_builder *b, unsigned slot)
{
   return nir_pack_64_2x32(b, aqe_const(b, slot, 2));
}

static nir_def *
task_num_workgroups(nir_builder *b)
{
   return nir_vec3(b, aqe_const(b, 6), aqe_const(b, 7), aqe_const(b, 8));
}

static nir_def *
aqe_global_index(nir_builder *b, nir_def *raw_index)
{
   return nir_iadd(b, nir_imul(b, aqe_const(b, 0), aqe_const(b, 1)), raw_index);
}

static nir_def *
aqe_udiv(nir_builder *b, nir_def *numerator, nir_def *denominator)
{
   /* GameSir ELF0x9b0fb4: AQE group coordinates use a reciprocal estimate
    * with one downward and one upward correction. This is the recovered
    * workgroup-index path, not a general replacement for nir_udiv: the
    * task/mesh total-workgroup limit is 2^22 and dispatched dimensions are
    * nonzero. Validate the native reciprocal and index bounds with AQE when
    * enabling the mesh compiler path.
    */
   nir_def *estimate = nir_f2u32(b, nir_fmul(b, nir_u2f32(b, numerator),
                                           nir_frcp(b, nir_u2f32(b, denominator))));
   nir_def *product = nir_imul(b, estimate, denominator);
   nir_def *quotient = nir_bcsel(b, nir_ult(b, numerator, product),
                                nir_iadd_imm(b, estimate, -1), estimate);
   nir_def *next = nir_iadd_imm(b, quotient, 1);
   return nir_bcsel(b, nir_uge(b, numerator, nir_imul(b, next, denominator)),
                    next, quotient);
}

static nir_def *
task_workgroup_id(nir_builder *b, nir_def *raw_index)
{
   nir_def *index = aqe_global_index(b, raw_index);
   nir_def *x_count = aqe_const(b, 6), *y_count = aqe_const(b, 7);
   nir_def *row = aqe_udiv(b, index, x_count);
   nir_def *z = aqe_udiv(b, row, y_count);
   return nir_vec3(b, nir_isub(b, index, nir_imul(b, row, x_count)),
                  nir_isub(b, row, nir_imul(b, z, y_count)), z);
}

static bool
prepare_task_intrinsic(nir_builder *b, nir_intrinsic_instr *intr, void *)
{
   if (intr->intrinsic != nir_intrinsic_load_num_workgroups)
      return false;
   b->cursor = nir_before_instr(&intr->instr);
   nir_def_replace(&intr->def, task_num_workgroups(b));
   return true;
}

bool
tu_nir_prepare_mesh(nir_shader *nir)
{
   assert(nir->info.stage == MESA_SHADER_MESH);
   const nir_lower_compute_system_values_options options = {
      .lower_workgroup_id_to_index = true,
   };
   return nir_lower_compute_system_values(nir, &options);
}

bool
tu_nir_prepare_task(nir_shader *nir)
{
   assert(nir->info.stage == MESA_SHADER_TASK);
   const nir_lower_compute_system_values_options options = {
      .lower_workgroup_id_to_index = true,
   };
   bool progress = nir_lower_compute_system_values(nir, &options);
   progress |= nir_shader_intrinsics_pass(nir, prepare_task_intrinsic,
                                         nir_metadata_control_flow, NULL);
   return progress;
}

static bool
reserve_primitive_header(nir_builder *, nir_intrinsic_instr *intr, void *)
{
   if (intr->intrinsic != nir_intrinsic_load_per_primitive_input)
      return false;

   nir_intrinsic_set_base(intr, nir_intrinsic_base(intr) + 1);
   return true;
}

void
tu_nir_link_mesh_fragment(nir_shader *mesh, nir_shader *fragment)
{
   assert(mesh && mesh->info.stage == MESA_SHADER_MESH);
   assert(!fragment || fragment->info.stage == MESA_SHADER_FRAGMENT);

   nir_recompute_io_bases(mesh, nir_var_shader_in | nir_var_shader_out);
   if (fragment) {
      nir_recompute_io_bases(fragment, nir_var_shader_in | nir_var_shader_out);
      /* Original mesh linkage increments load_per_primitive_input.base
       * (original intrinsic499), leaving other input classes unchanged.
       */
      nir_shader_intrinsics_pass(fragment, reserve_primitive_header,
                                  nir_metadata_control_flow, NULL);
      nir_shader_gather_info(fragment, nir_shader_get_entrypoint(fragment));
   }
   nir_shader_gather_info(mesh, nir_shader_get_entrypoint(mesh));

   if (fragment) {
      /* The mesh layout groups attributes by the consumer's interpolation
       * class. Gathering the producer alone cannot recover those qualifiers.
       */
      mesh->info.known_interpolation_qualifiers =
         fragment->info.known_interpolation_qualifiers;
      mesh->info.perspective_varyings = fragment->info.perspective_varyings;
      mesh->info.linear_varyings = fragment->info.linear_varyings;
   }
}

static bool
lower_task_intrinsic(nir_builder *b, nir_intrinsic_instr *intr, void *data)
{
   const unsigned stride = *(unsigned *) data;
   b->cursor = nir_before_instr(&intr->instr);
   switch (intr->intrinsic) {
   case nir_intrinsic_load_draw_id:
      nir_def_replace(&intr->def, aqe_const(b, 32));
      return true;
   case nir_intrinsic_load_num_workgroups:
      nir_def_replace(&intr->def, task_num_workgroups(b));
      return true;
   case nir_intrinsic_load_workgroup_index: {
      nir_def *raw = nir_channel(b, nir_load_workgroup_id(b), 0);
      nir_def_replace(&intr->def, aqe_global_index(b, raw));
      return true;
   }
   case nir_intrinsic_load_workgroup_id: {
      /* Keep the hardware load as the source of the logical ID. Rewriting
       * only later uses prevents a cycle through the newly inserted math.
       */
      b->cursor = nir_after_instr(&intr->instr);
      nir_def *id = task_workgroup_id(b, nir_channel(b, &intr->def, 0));
      nir_def_rewrite_uses_after(&intr->def, id);
      return true;
   }
   case nir_intrinsic_load_task_payload:
   case nir_intrinsic_store_task_payload: {
      bool store = intr->intrinsic == nir_intrinsic_store_task_payload;
      nir_def *raw = nir_channel(b, nir_load_workgroup_id(b), 0);
      nir_def *offset = nir_iadd_imm(b, nir_iadd(b, nir_imul_imm(b, raw, stride),
                           intr->src[store ? 1 : 0].ssa), nir_intrinsic_base(intr));
      nir_def *pointer = aqe_pointer(b, 4);
      enum gl_access_qualifier access = (enum gl_access_qualifier)
         (nir_intrinsic_access(intr) | ACCESS_VOLATILE);
      unsigned bits = store ? intr->src[0].ssa->bit_size : intr->def.bit_size;
      unsigned alignment = nir_intrinsic_align_mul(intr);
      if (!alignment)
         alignment = bits / 8;
      /* AQE payload records guarantee only four-byte alignment. The next
       * group's record may break a stronger alignment of the original I/O.
       */
      alignment = MIN2(alignment, 4);
      unsigned alignment_offset = nir_intrinsic_align_offset(intr) % alignment;
      if (store) {
         unsigned mask = nir_intrinsic_write_mask(intr);
         nir_store_global_offset(b, intr->src[0].ssa, pointer, offset,
            .access = access,
            .write_mask = mask ? mask : BITFIELD_MASK(intr->num_components),
            .align_mul = alignment, .align_offset = alignment_offset);
         nir_instr_remove(&intr->instr);
      } else {
         nir_def *value = nir_load_global_offset(b, intr->def.num_components,
            bits, pointer, offset, .access = access, .align_mul = alignment,
            .align_offset = alignment_offset);
         nir_def_replace(&intr->def, value);
      }
      return true;
   }
   case nir_intrinsic_launch_mesh_workgroups: {
      nir_def *raw = nir_channel(b, nir_load_workgroup_id(b), 0);
      nir_push_if(b, nir_ieq_imm(b, nir_load_local_invocation_index(b), 0));
      nir_def *count = intr->src[0].ssa;
      nir_def *x = nir_channel(b, count, 0), *y = nir_channel(b, count, 1);
      nir_def *z = nir_channel(b, count, 2);
      nir_def *total = nir_imul(b, nir_imul(b, x, y), z);
      nir_def *xy = nir_ior(b, nir_iand_imm(b, nir_iadd_imm(b, x, -1), 0xffff),
         nir_ishl_imm(b, nir_iand_imm(b, nir_iadd_imm(b, y, -1), 0xffff), 16));
      nir_def *record = nir_bcsel(b, nir_ieq_imm(b, total, 0), nir_imm_ivec3(b, 0, 0, 0),
         nir_vec3(b, total, xy, nir_iadd_imm(b, z, -1)));
      nir_def *pointer = aqe_pointer(b, 2);
      nir_def *offset = nir_imul_imm(b, raw, 24);
      nir_store_global_offset(b, record, pointer, offset,
                             .write_mask = 7, .access = ACCESS_VOLATILE,
                             .align_mul = 4);
      nir_store_global_offset(b, task_workgroup_id(b, raw), pointer,
                             nir_iadd_imm(b, offset, 12),
                             .write_mask = 7, .access = ACCESS_VOLATILE,
                             .align_mul = 4);
      nir_pop_if(b, NULL);
      nir_instr_remove(&intr->instr);
      return true;
   }
   case nir_intrinsic_barrier: {
      nir_variable_mode modes = nir_intrinsic_memory_modes(intr);
      const unsigned payload_modes = nir_var_shader_out | nir_var_mem_task_payload;
      if (!(modes & payload_modes))
         return false;
      nir_intrinsic_set_memory_modes(intr, (nir_variable_mode)
         ((modes & ~payload_modes) | nir_var_mem_global));
      return true;
   }
   default:
      return false;
   }
}

bool
tu_nir_lower_task(nir_shader *nir)
{
   assert(nir->info.stage == MESA_SHADER_TASK);
   unsigned stride = ALIGN_POT(MAX2(nir->info.task_payload_size, 1), 4);
   return nir_shader_intrinsics_pass(nir, lower_task_intrinsic,
                                    nir_metadata_none, &stride);
}


static nir_def *
mesh_num_workgroups(nir_builder *b)
{
   return nir_vec3(b, aqe_const(b, 15), aqe_const(b, 16), aqe_const(b, 17));
}

static nir_def *
mesh_workgroup_id(nir_builder *b, nir_def *raw_index)
{
   nir_def *index = aqe_global_index(b, raw_index);
   nir_def *x_count = aqe_const(b, 15), *y_count = aqe_const(b, 16);
   nir_def *row = aqe_udiv(b, index, x_count);
   nir_def *z = aqe_udiv(b, row, y_count);
   return nir_vec3(b, nir_isub(b, index, nir_imul(b, row, x_count)),
                  nir_isub(b, row, nir_imul(b, z, y_count)), z);
}

static bool
lower_mesh_input(nir_builder *b, nir_intrinsic_instr *intr, void *)
{
   b->cursor = nir_before_instr(&intr->instr);
   switch (intr->intrinsic) {
   case nir_intrinsic_load_view_index:
      if (!b->shader->info.view_mask)
         return false;
      nir_def_replace(&intr->def, aqe_const(b, 33));
      return true;
   case nir_intrinsic_load_draw_id:
      nir_def_replace(&intr->def, aqe_const(b, 32));
      return true;
   case nir_intrinsic_load_num_workgroups:
      nir_def_replace(&intr->def, mesh_num_workgroups(b));
      return true;
   case nir_intrinsic_load_workgroup_index: {
      nir_def *raw = nir_channel(b, nir_load_workgroup_id(b), 0);
      nir_def_replace(&intr->def, aqe_global_index(b, raw));
      return true;
   }
   case nir_intrinsic_load_workgroup_id: {
      b->cursor = nir_after_instr(&intr->instr);
      nir_def *id = mesh_workgroup_id(b, nir_channel(b, &intr->def, 0));
      nir_def_rewrite_uses_after(&intr->def, id);
      return true;
   }
   case nir_intrinsic_load_task_payload: {
      unsigned stride = ALIGN_POT(MAX2(b->shader->info.task_payload_size, 1), 4);
      /* A mesh group reads its producing task group's payload, independently
       * of the raw mesh group ID and the global mesh group index.
       */
      nir_def *offset = nir_iadd_imm(b, nir_iadd(b,
         nir_imul_imm(b, aqe_const(b, 14), stride), intr->src[0].ssa),
         nir_intrinsic_base(intr));
      unsigned alignment = nir_intrinsic_align_mul(intr);
      if (!alignment)
         alignment = intr->def.bit_size / 8;
      alignment = MIN2(alignment, 4);
      unsigned alignment_offset = nir_intrinsic_align_offset(intr) % alignment;
      nir_def *value = nir_load_global_offset(b, intr->def.num_components,
         intr->def.bit_size, aqe_pointer(b, 2), offset,
         .access = (enum gl_access_qualifier)
            (nir_intrinsic_access(intr) | ACCESS_VOLATILE),
         .align_mul = alignment, .align_offset = alignment_offset);
      nir_def_replace(&intr->def, value);
      return true;
   }
   default:
      return false;
   }
}

bool
tu_nir_lower_mesh_inputs(nir_shader *nir)
{
   assert(nir->info.stage == MESA_SHADER_MESH);
   return nir_shader_intrinsics_pass(nir, lower_mesh_input,
                                    nir_metadata_control_flow, NULL);
}

static nir_def *
mesh_primitive_offset(nir_builder *b, const tu_mesh_output_layout *layout,
                      nir_def *primitive)
{
   nir_def *raw = nir_channel(b, nir_load_workgroup_id(b), 0);
   return nir_iadd(b, nir_imul_imm(b, raw,
                                 layout->index_stride * layout->primitive.max_count),
                  nir_imul_imm(b, primitive, layout->index_stride));
}

static unsigned
mesh_flags_offset(const tu_mesh_output_layout *layout)
{
   assert(layout->vertices_per_primitive >= 1 &&
          layout->vertices_per_primitive <= 3);
   return layout->vertices_per_primitive == 1 ? 6 :
          2 * layout->vertices_per_primitive;
}

static bool
lower_mesh_special_output(nir_builder *b, nir_intrinsic_instr *intr,
                          const tu_mesh_output_layout *layout,
                          bool store, unsigned location)
{
   b->cursor = nir_before_instr(&intr->instr);
   nir_def *primitive = intr->src[store ? 1 : 0].ssa;
   unsigned base;
   switch (location) {
   case VARYING_SLOT_LAYER: base = layout->layer_shared_base; break;
   case VARYING_SLOT_VIEWPORT: base = layout->viewport_shared_base; break;
   case VARYING_SLOT_PRIMITIVE_SHADING_RATE:
      base = layout->shading_rate_shared_base;
      break;
   case VARYING_SLOT_CULL_PRIMITIVE: base = layout->cull_shared_base; break;
   default: return false;
   }
   nir_def *shared_offset = nir_iadd_imm(b, nir_imul_imm(b, primitive, 4), base);
   if (!store) {
      nir_def_replace(&intr->def, nir_load_shared(b, intr->def.num_components,
         intr->def.bit_size, shared_offset, .align_mul = 4));
      return true;
   }
   nir_def *value = intr->src[0].ssa;
   /* GameSir lower_mesh_intrinsic ELF0x9ae8b0/0x9aec6c keeps the
    * cull/shading-rate shared mirrors volatile. Layer/viewport mirrors
    * use ordinary stores; their packed global flags are handled below.
    */
   enum gl_access_qualifier shared_access =
      location == VARYING_SLOT_CULL_PRIMITIVE ||
      location == VARYING_SLOT_PRIMITIVE_SHADING_RATE ? ACCESS_VOLATILE :
                                                     (gl_access_qualifier) 0;
   nir_store_shared(b, value, shared_offset,
                    .access = shared_access,
                    .write_mask = nir_intrinsic_write_mask(intr),
                    .align_mul = 4);
   nir_def *scalar = nir_channel(b, value, 0);
   if (location == VARYING_SLOT_CULL_PRIMITIVE) {
      nir_push_if(b, nir_ine_imm(b, scalar, 0));
      nir_def *raw = nir_channel(b, nir_load_workgroup_id(b), 0);
      nir_def *offset = nir_iadd(b, nir_imul_imm(b, raw, 32),
                                nir_imul_imm(b, nir_ushr_imm(b, primitive, 5), 4));
      nir_def *address = nir_iadd(b, aqe_pointer(b, 12), nir_u2u64(b, offset));
      nir_def *mask = nir_inot(b, nir_ishl(b, nir_imm_int(b, 1),
                                        nir_iand_imm(b, primitive, 31)));
      nir_global_atomic(b, 32, address, mask, .atomic_op = nir_atomic_op_iand);
      nir_pop_if(b, NULL);
   } else {
      nir_def *offset = nir_iadd_imm(b, mesh_primitive_offset(b, layout, primitive),
                                   mesh_flags_offset(layout));
      nir_def *pointer = aqe_pointer(b, 8);
      nir_def *flags = nir_load_global_offset(b, 1, 16, pointer, offset,
                                             .access = ACCESS_VOLATILE,
                                             .align_mul = 2);
      if (location == VARYING_SLOT_PRIMITIVE_SHADING_RATE) {
         scalar = nir_ishl_imm(b, nir_iand_imm(b, scalar, 15), 4);
         flags = nir_iand_imm(b, flags, 0xff0f);
      } else if (location == VARYING_SLOT_LAYER) {
         scalar = nir_ishl_imm(b, scalar, 8);
      }
      flags = nir_ior(b, flags, nir_u2u16(b, scalar));
      nir_store_global_offset(b, flags, pointer, offset, .write_mask = 1,
                              .access = ACCESS_VOLATILE, .align_mul = 2);
   }
   nir_instr_remove(&intr->instr);
   return true;
}

static bool
lower_mesh_primitive_indices(nir_builder *b, nir_intrinsic_instr *intr,
                             const tu_mesh_output_layout *layout)
{
   b->cursor = nir_before_instr(&intr->instr);
   nir_def *primitive = intr->src[1].ssa;
   nir_def *value = intr->src[0].ssa;
   nir_def *offset = mesh_primitive_offset(b, layout, primitive);
   nir_def *pointer = aqe_pointer(b, 8);
   nir_def *flags = nir_load_global_offset(b, 1, 16, pointer,
      nir_iadd_imm(b, offset, mesh_flags_offset(layout)),
      .access = ACCESS_VOLATILE, .align_mul = 2);
   if (b->shader->info.view_mask)
      flags = nir_ior(b, flags, nir_u2u16(b, nir_ishl_imm(b, aqe_const(b, 33), 8)));
   nir_def *x = nir_u2u16(b, nir_channel(b, value, 0));
   nir_def *packed;
   switch (layout->vertices_per_primitive) {
   case 1:
      packed = nir_vec4(b, x, x, x, flags);
      break;
   case 2:
      packed = nir_vec3(b, x, nir_u2u16(b, nir_channel(b, value, 1)), flags);
      break;
   case 3:
      packed = nir_vec4(b, x, nir_u2u16(b, nir_channel(b, value, 1)),
                        nir_u2u16(b, nir_channel(b, value, 2)), flags);
      break;
   default:
      UNREACHABLE("invalid mesh primitive size");
   }
   nir_store_global_offset(b, packed, pointer, offset,
      .write_mask = BITFIELD_MASK(packed->num_components),
      .access = layout->index_store_volatile ? ACCESS_VOLATILE : (gl_access_qualifier) 0,
      .align_mul = 2);
   nir_def *shared_offset = nir_iadd_imm(b,
      nir_imul_imm(b, primitive, layout->index_stride), layout->index_shared_base);
   nir_store_shared(b, packed, shared_offset,
                    .write_mask = BITFIELD_MASK(packed->num_components), .align_mul = 2);
   nir_instr_remove(&intr->instr);
   return true;
}

static bool
lower_mesh_output(nir_builder *b, nir_intrinsic_instr *intr, void *data)
{
   const auto *layout = (const tu_mesh_output_layout *) data;
   bool store, primitive;
   switch (intr->intrinsic) {
   case nir_intrinsic_store_per_vertex_output:
      store = true;
      primitive = false;
      break;
   case nir_intrinsic_store_per_primitive_output:
      store = true;
      primitive = true;
      break;
   case nir_intrinsic_load_per_vertex_output:
      store = false;
      primitive = false;
      break;
   case nir_intrinsic_load_per_primitive_output:
      store = false;
      primitive = true;
      break;
   case nir_intrinsic_barrier: {
      nir_variable_mode modes = nir_intrinsic_memory_modes(intr);
      if (!(modes & nir_var_shader_out))
         return false;
      nir_intrinsic_set_memory_modes(intr, (nir_variable_mode)
         ((modes & ~nir_var_shader_out) | nir_var_mem_shared));
      return true;
   }
   default:
      return false;
   }

   unsigned location = nir_intrinsic_io_semantics(intr).location;
   if (location == VARYING_SLOT_PRIMITIVE_INDICES) {
      /* EXT primitive-index arrays are write-only (see vtn_variables.c).
       * Reject invalid reads in the recovered entry before any mutation.
       */
      if (!store)
         return false;
      return lower_mesh_primitive_indices(b, intr, layout);
   }
   if (primitive && (location == VARYING_SLOT_CULL_PRIMITIVE ||
                     location == VARYING_SLOT_PRIMITIVE_SHADING_RATE ||
                     location == VARYING_SLOT_LAYER ||
                     location == VARYING_SLOT_VIEWPORT))
      return lower_mesh_special_output(b, intr, layout, store, location);

   const auto *buffer = primitive ? &layout->primitive : &layout->vertex;
   assert(location < VARYING_SLOT_MAX);
   assert(buffer->slot_offset[location] != UINT16_MAX);
   b->cursor = nir_before_instr(&intr->instr);
   nir_def *element = intr->src[store ? 1 : 0].ssa;
   nir_def *slot = intr->src[store ? 2 : 1].ssa;
   unsigned constant_offset = buffer->slot_offset[location] +
                              nir_intrinsic_component(intr) * 4;
   nir_def *offset = nir_iadd_imm(b, nir_iadd(b,
      nir_imul_imm(b, element, buffer->stride), nir_imul_imm(b, slot, 16)),
      constant_offset);
   nir_def *shared_offset = nir_iadd_imm(b, offset, buffer->shared_base);
   if (store) {
      nir_def *value = intr->src[0].ssa;
      unsigned mask = nir_intrinsic_write_mask(intr);
      nir_store_shared(b, value, shared_offset, .write_mask = mask, .align_mul = 4);
      nir_def *raw = nir_channel(b, nir_load_workgroup_id(b), 0);
      nir_def *global_offset = nir_iadd(b, offset, nir_imul_imm(b, raw,
         buffer->stride * buffer->max_count));
      nir_store_global_offset(b, value, aqe_pointer(b, primitive ? 6 : 4),
                              global_offset, .write_mask = mask, .align_mul = 4);
      nir_instr_remove(&intr->instr);
   } else {
      nir_def *value = nir_load_shared(b, intr->def.num_components,
                                      intr->def.bit_size, shared_offset,
                                      .align_mul = 4);
      nir_def_replace(&intr->def, value);
   }
   return true;
}

bool
tu_nir_lower_mesh_outputs(nir_shader *nir, const tu_mesh_output_layout *layout)
{
   assert(nir->info.stage == MESA_SHADER_MESH);
   return nir_shader_intrinsics_pass(nir, lower_mesh_output,
                                    nir_metadata_none, (void *) layout);
}


static void
mesh_initialize_primitive_flags(nir_builder *b,
                                const tu_mesh_output_layout *layout,
                                nir_def *primitive_count)
{
   nir_variable *counter = nir_local_variable_create(b->impl, glsl_uint_type(),
                                                    "aqe_primitive");
   nir_store_var(b, counter, nir_imm_int(b, 0), 1);
   nir_push_loop(b);
   nir_def *i = nir_load_var(b, counter);
   nir_push_if(b, nir_uge(b, i, primitive_count));
   nir_jump(b, nir_jump_break);
   nir_pop_if(b, NULL);
   nir_def *offset = nir_iadd_imm(b, mesh_primitive_offset(b, layout, i),
                                mesh_flags_offset(layout));
   nir_store_global_offset(b, nir_imm_intN_t(b, 0, 16), aqe_pointer(b, 8), offset,
                           .write_mask = 1, .access = ACCESS_VOLATILE,
                           .align_mul = 2);
   nir_store_var(b, counter, nir_iadd_imm(b, i, 1), 1);
   nir_pop_loop(b, NULL);
}

static void
mesh_publish_counts(nir_builder *b, const tu_mesh_output_layout *layout,
                     nir_def *vertices, nir_def *primitives)
{
   unsigned workgroup_size = b->shader->info.workgroup_size[0] *
                             b->shader->info.workgroup_size[1] *
                             b->shader->info.workgroup_size[2];
   bool multiple_invocations = workgroup_size != 1;
   if (multiple_invocations)
      nir_push_if(b, nir_ieq_imm(b, nir_load_local_invocation_index(b), 0));

   mesh_initialize_primitive_flags(b, layout, primitives);
   nir_def *raw = nir_channel(b, nir_load_workgroup_id(b), 0);
   if (layout->uses_cull_bitmap) {
      nir_def *offset = nir_imul_imm(b, raw, 32);
      nir_def *pointer = aqe_pointer(b, 12);
      nir_def *ones = nir_imm_ivec4(b, -1, -1, -1, -1);
      nir_store_global_offset(b, ones, pointer, offset, .write_mask = 15,
                              .access = ACCESS_VOLATILE, .align_mul = 4);
      nir_store_global_offset(b, ones, pointer, nir_iadd_imm(b, offset, 16),
                              .write_mask = 15, .access = ACCESS_VOLATILE,
                              .align_mul = 4);
   }
   nir_def *pointer = aqe_pointer(b, 10);
   nir_def *offset = nir_imul_imm(b, raw, 4);
   nir_store_global_offset(b, primitives, pointer, offset,
                           .write_mask = 1, .align_mul = 4);
   nir_store_global_offset(b, vertices, pointer,
      nir_iadd(b, offset, nir_imul_imm(b, aqe_const(b, 1), 4)),
      .write_mask = 1, .align_mul = 4);
   if (multiple_invocations) {
      nir_pop_if(b, NULL);
      /* Counts are workgroup-uniform control flow, but that is not an
       * execution barrier. Other invocations must not update packed flags
       * or clear cull bits before invocation0 completes initialization.
       * Keep this conservative synchronization until full AQE integration
       * can establish an equivalent hardware ordering guarantee.
       */
      nir_barrier(b, .execution_scope = SCOPE_WORKGROUP,
                     .memory_scope = SCOPE_WORKGROUP,
                     .memory_semantics = NIR_MEMORY_ACQ_REL,
                     .memory_modes = nir_var_mem_global);
   }
}

static bool
lower_mesh_count(nir_builder *b, nir_intrinsic_instr *intr, void *data)
{
   if (intr->intrinsic != nir_intrinsic_set_vertex_and_primitive_count)
      return false;
   const auto *layout = (const tu_mesh_output_layout *) data;
   b->cursor = nir_before_instr(&intr->instr);
   mesh_publish_counts(b, layout, intr->src[0].ssa, intr->src[1].ssa);
   /* The third source is a GS decomposed-primitive count. The SPIR-V mesh
    * frontend supplies undef there; mesh counts use only sources0 and1.
    */
   nir_instr_remove(&intr->instr);
   return true;
}

bool
tu_nir_lower_mesh_counts(nir_shader *nir, const tu_mesh_output_layout *layout)
{
   assert(nir->info.stage == MESA_SHADER_MESH);
   bool progress = nir_shader_intrinsics_pass(nir, lower_mesh_count,
                                             nir_metadata_none, (void *) layout);
   if (progress)
      nir_lower_vars_to_ssa(nir);
   return progress;
}

static bool
remove_mesh_count(nir_builder *, nir_intrinsic_instr *intr, void *)
{
   if (intr->intrinsic != nir_intrinsic_set_vertex_and_primitive_count)
      return false;
   nir_instr_remove(&intr->instr);
   return true;
}

/* Called for one constant entry-block count site, or the original empty
 * output case with no count site (both counts are zero). Create fresh
 * constants so hoisted code never references later SSA.
 */
static void
lower_static_mesh_counts(nir_shader *nir, const tu_mesh_output_layout *layout,
                          unsigned vertices, unsigned primitives)
{
   nir_builder b = nir_builder_at(nir_before_impl(nir_shader_get_entrypoint(nir)));
   mesh_publish_counts(&b, layout, nir_imm_int(&b, vertices),
                        nir_imm_int(&b, primitives));
   nir_progress(true, b.impl, nir_metadata_none);
   nir_shader_intrinsics_pass(nir, remove_mesh_count, nir_metadata_control_flow, NULL);
   nir_lower_vars_to_ssa(nir);
}


bool
tu_nir_collect_mesh_layout_inputs(const nir_shader *mesh,
                                  const nir_shader *fragment,
                                  tu_mesh_layout_inputs *inputs)
{
   if (mesh->info.stage != MESA_SHADER_MESH ||
       (fragment && fragment->info.stage != MESA_SHADER_FRAGMENT))
      return false;

   tu_mesh_layout_inputs in = {};
   switch (mesh->info.mesh.primitive_type) {
   case MESA_PRIM_POINTS: in.vertices_per_primitive = 1; break;
   case MESA_PRIM_LINES: in.vertices_per_primitive = 2; break;
   case MESA_PRIM_TRIANGLES: in.vertices_per_primitive = 3; break;
   default: return false;
   }
   in.shared_size = mesh->info.shared_size;
   in.max_vertices = mesh->info.mesh.max_vertices_out;
   in.max_primitives = mesh->info.mesh.max_primitives_out;
   in.clip_distance_count = mesh->info.clip_distance_array_size;
   in.cull_distance_count = mesh->info.cull_distance_array_size;
   if (in.clip_distance_count > 8 || in.cull_distance_count > 8)
      return false;
   in.view_mask = mesh->info.view_mask != 0;
   const uint64_t primitive_outputs = mesh->info.per_primitive_outputs;
   in.layer = primitive_outputs & VARYING_BIT_LAYER;
   in.viewport = primitive_outputs & VARYING_BIT_VIEWPORT;
   in.cull_primitive = primitive_outputs & VARYING_BIT_CULL_PRIMITIVE;
   in.primitive_shading_rate = primitive_outputs & VARYING_BIT_PRIMITIVE_SHADING_RATE;
   in.no_fragment_shader = !fragment;
   in.uses_draw_id = BITSET_TEST(mesh->info.system_values_read, SYSTEM_VALUE_DRAW_ID);
   in.uses_num_workgroups = BITSET_TEST(mesh->info.system_values_read, SYSTEM_VALUE_NUM_WORKGROUPS);

   const shader_info *linked_info = fragment ? &fragment->info : &mesh->info;
   const uint64_t interpolated = linked_info->perspective_varyings | linked_info->linear_varyings;
   if (fragment) {
      nir_foreach_function_impl(impl, fragment) {
         nir_foreach_block(block, impl) {
            nir_foreach_instr(instr, block) {
               if (instr->type != nir_instr_type_intrinsic) continue;
               const nir_intrinsic_instr *intr = nir_instr_as_intrinsic(instr);
               /* The original propagates FS Layer/Viewport demand into the
                * mesh per-primitive interface before building its layout.
                * Derive it locally instead of mutating borrowed consumer NIR.
                */
               if (nir_intrinsic_has_io_semantics(intr)) {
                  unsigned slot = nir_intrinsic_io_semantics(intr).location;
                  in.layer |= slot == VARYING_SLOT_LAYER;
                  in.viewport |= slot == VARYING_SLOT_VIEWPORT;
               }
               if (intr->intrinsic != nir_intrinsic_load_per_vertex_input &&
                   intr->intrinsic != nir_intrinsic_load_input_vertex)
                  continue;
               unsigned location = nir_intrinsic_io_semantics(intr).location;
               if (location >= VARYING_SLOT_VAR0 && location < VARYING_SLOT_VAR0 + 32)
                  in.unpacked_variable_mask |= 1u << (location - VARYING_SLOT_VAR0);
            }
         }
      }
   }

   nir_foreach_function_impl(impl, mesh) {
      nir_foreach_block(block, impl) {
         nir_foreach_instr(instr, block) {
            if (instr->type != nir_instr_type_intrinsic) continue;
            const nir_intrinsic_instr *intr = nir_instr_as_intrinsic(instr);
            bool primitive, store;
            switch (intr->intrinsic) {
            case nir_intrinsic_load_per_vertex_output: primitive = false; store = false; break;
            case nir_intrinsic_store_per_vertex_output: primitive = false; store = true; break;
            case nir_intrinsic_load_per_primitive_output: primitive = true; store = false; break;
            case nir_intrinsic_store_per_primitive_output: primitive = true; store = true; break;
            default: continue;
            }
            nir_io_semantics sem = nir_intrinsic_io_semantics(intr);
            unsigned location = sem.location;
            if (location == VARYING_SLOT_PRIMITIVE_INDICES) continue;
            if (primitive) {
               if (location == VARYING_SLOT_LAYER) { in.layer = true; continue; }
               if (location == VARYING_SLOT_VIEWPORT) { in.viewport = true; continue; }
               if (location == VARYING_SLOT_CULL_PRIMITIVE) { in.cull_primitive = true; continue; }
               if (location == VARYING_SLOT_PRIMITIVE_ID) in.primitive_id = true;
            }
            unsigned slots = sem.num_slots;
            if (!primitive && location >= VARYING_SLOT_CLIP_DIST0 && location <= VARYING_SLOT_CULL_DIST1)
               slots = DIV_ROUND_UP(slots, 4);
            if (location >= 64 || slots > 64 - location) return false;
            bool *used = primitive ? in.primitive_used : in.vertex_used;
            uint8_t *components = primitive ? in.primitive_components : in.vertex_components;
            for (unsigned i = 0; i < slots; i++) used[location + i] = true;
            if (!store) continue;
            unsigned end_component = util_last_bit(nir_intrinsic_write_mask(intr) <<
                                                   nir_intrinsic_component(intr));
            if (end_component > 4) return false;
            components[location] = MAX2(components[location], end_component);
            for (unsigned i = 1; i < slots; i++) components[location + i] = 4;
            if (!primitive) {
               for (unsigned i = 0; i < slots; i++) {
                  unsigned slot = location + i;
                  in.qualifier_key[slot] = (uint8_t)nir_intrinsic_src_type(intr) |
                                           (((interpolated >> slot) & 1) << 8);
               }
            }
         }
      }
   }
   *inputs = in;
   return true;
}

bool
tu_nir_build_mesh_layout(nir_shader *mesh, const nir_shader *fragment,
                         bool index_store_volatile, tu_mesh_shader_layout *layout,
                         tu_mesh_output_layout *outputs)
{
   tu_mesh_layout_inputs inputs;
   if (!tu_nir_collect_mesh_layout_inputs(mesh, fragment, &inputs)) return false;
   tu_mesh_shader_layout result;
   tu_mesh_build_layout(&inputs, &result);
   tu_mesh_output_layout io = {};
   io.vertex.shared_base = result.vertex_shared_base;
   io.vertex.stride = result.vertex_stride;
   io.vertex.max_count = result.max_vertices;
   io.primitive.shared_base = result.primitive_shared_base;
   io.primitive.stride = result.primitive_stride;
   io.primitive.max_count = result.max_primitives;
   static_assert(VARYING_SLOT_MAX == 64);
   memcpy(io.vertex.slot_offset, result.vertex_offset, sizeof(result.vertex_offset));
   memcpy(io.primitive.slot_offset, result.primitive_offset, sizeof(result.primitive_offset));
   io.index_shared_base = result.index_shared_base;
   io.index_stride = result.index_stride;
   io.layer_shared_base = result.layer_shared_base;
   io.viewport_shared_base = result.viewport_shared_base;
   io.cull_shared_base = result.cull_shared_base;
   io.shading_rate_shared_base = result.shading_rate_shared_base;
   io.vertices_per_primitive = result.vertices_per_primitive;
   io.uses_cull_bitmap = result.uses_cull_bitmap;
   /* The publication wrapper must supply its independently selected policy. */
   io.index_store_volatile = index_store_volatile;
   mesh->info.shared_size = MAX2(mesh->info.shared_size, result.shared_size);
   *layout = result;
   *outputs = io;
   return true;
}


bool
tu_nir_lower_task_aqe(nir_shader *task)
{
   if (task->info.stage != MESA_SHADER_TASK)
      return false;
   /* tu_lower_nir must already have made task payload/shared I/O explicit. */
   const nir_lower_task_shader_options options = {
      .payload_to_shared_for_atomics = true,
      .payload_to_shared_for_small_types = true,
   };
   tu_nir_prepare_task(task);
   nir_lower_task_shader(task, options);
   tu_nir_lower_task(task);
   /* Generic task lowering inserts new compute system values for copies. */
   nir_lower_compute_system_values(task, NULL);
   const nir_lower_idiv_options idiv_options = { .allow_fp16 = true };
   nir_opt_idiv_const(task, 8);
   nir_lower_idiv(task, &idiv_options);
   return true;
}

static bool
lower_mesh_aqe(nir_shader *mesh, const nir_shader *fragment,
                bool index_store_volatile, bool use_recovered_policy,
                tu_mesh_shader_layout *layout)
{
   if (mesh->info.stage != MESA_SHADER_MESH || mesh->info.mesh.nv)
      return false;
   nir_function_impl *entry = nir_shader_get_entrypoint(mesh);
   if (!entry)
      return false;

   /* Inspect the complete input before any mutation. EXT primitive-index
    * arrays are write-only; their read/write NV form is not advertised by
    * the original driver and is not covered by these EXT passes. A fallback in
    * the original wrapper requires dynamic_counts && !counts_initialized;
    * its only dynamic-count writer also sets counts_initialized, so that
    * shared-count branch has no producer in this EXT wrapper/callback chain.
    * Do not guess at an NV ABI from that code.
    * In particular, do not let an unlowered read reach IR3 by returning success.
    */
   unsigned count_sites = 0;
   bool static_entry_counts = false;
   uint32_t vertices = 0, primitives = 0;
   nir_foreach_function_impl(impl, mesh) {
      if (impl != entry)
         return false;
      nir_foreach_block(block, impl) {
         nir_foreach_instr(instr, block) {
            if (instr->type != nir_instr_type_intrinsic)
               continue;
            const nir_intrinsic_instr *intr = nir_instr_as_intrinsic(instr);
            switch (intr->intrinsic) {
            case nir_intrinsic_set_vertex_and_primitive_count:
               count_sites++;
               static_entry_counts = block == nir_start_block(entry) &&
                  nir_src_is_const(intr->src[0]) && nir_src_is_const(intr->src[1]);
               if (static_entry_counts) {
                  vertices = nir_src_as_uint(intr->src[0]);
                  primitives = nir_src_as_uint(intr->src[1]);
               }
               break;
            case nir_intrinsic_load_per_vertex_output:
            case nir_intrinsic_load_per_primitive_output:
               if (nir_intrinsic_io_semantics(intr).location == VARYING_SLOT_PRIMITIVE_INDICES)
                  return false;
               break;
            case nir_intrinsic_load_output:
            case nir_intrinsic_store_output:
            case nir_intrinsic_load_per_view_output:
            case nir_intrinsic_store_per_view_output:
               return false;
            case nir_intrinsic_copy_deref:
               if (nir_deref_mode_is(nir_src_as_deref(intr->src[1]), nir_var_shader_out))
                  return false;
               FALLTHROUGH;
            case nir_intrinsic_load_deref:
            case nir_intrinsic_store_deref:
               if (nir_deref_mode_is(nir_src_as_deref(intr->src[0]), nir_var_shader_out))
                  return false;
               break;
            default:
               break;
            }
         }
      }
   }
   tu_mesh_shader_layout result;
   tu_mesh_output_layout outputs;
   if (!tu_nir_build_mesh_layout(mesh, fragment, index_store_volatile, &result, &outputs))
      return false;
   if (use_recovered_policy)
      outputs.index_store_volatile = tu_mesh_indices_require_volatile(&result);

   /* The original zeroes the layout and reaches its static publication
    * wrapper with zero/zero when there is no SetMeshOutputs site. Missing
    * counts do not select the unreachable shared-count fallback.
    */
   static_entry_counts = count_sites == 0 ||
                         (static_entry_counts && count_sites == 1);
   result.dynamic_counts = !static_entry_counts;
   if (static_entry_counts) {
      result.static_vertices = vertices;
      result.static_primitives = primitives;
   }

   tu_nir_prepare_mesh(mesh);
   /* Ordering is essential: count/output lowering inserts raw hardware group
    * loads, which must not be run through logical-group input lowering again.
    */
   tu_nir_lower_mesh_inputs(mesh);
   if (static_entry_counts && (use_recovered_policy || count_sites == 0))
      lower_static_mesh_counts(mesh, &outputs, vertices, primitives);
   else
      tu_nir_lower_mesh_counts(mesh, &outputs);
   /* Local marker: publication code exists, whether at entry or count site. */
   result.counts_initialized = true;
   tu_nir_lower_mesh_outputs(mesh, &outputs);
   /* Count initialization adds local-invocation indices after preparation.
    * Match the original post-mesh compute-system-value lowering here. */
   nir_lower_compute_system_values(mesh, NULL);
   const nir_lower_idiv_options idiv_options = { .allow_fp16 = true };
   nir_opt_idiv_const(mesh, 8);
   nir_lower_idiv(mesh, &idiv_options);
   *layout = result;
   return true;
}

bool
tu_nir_lower_mesh_aqe(nir_shader *mesh, const nir_shader *fragment,
                      bool index_store_volatile, tu_mesh_shader_layout *layout)
{
   return lower_mesh_aqe(mesh, fragment, index_store_volatile, false, layout);
}

bool
tu_nir_lower_mesh_aqe_recovered(nir_shader *mesh, const nir_shader *fragment,
                                tu_mesh_shader_layout *layout)
{
   return lower_mesh_aqe(mesh, fragment, false, true, layout);
}
