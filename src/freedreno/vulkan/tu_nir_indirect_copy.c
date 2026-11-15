/* SPDX-License-Identifier: MIT */

#include "tu_nir_indirect_copy.h"
#include "tu_indirect_copy_format.h"
#include "compiler/nir/nir_format_convert.h"
#include "compiler/nir/nir_builder.h"
#include "vulkan/vulkan_core.h"

static nir_def *
load_const(nir_builder *b, unsigned base, unsigned components)
{
   return nir_load_const_ir3(b, components, 32, nir_imm_int(b, 0), .base = base);
}

static nir_def *
load_command_u32(nir_builder *b, nir_def *command, unsigned offset)
{
   return nir_load_global(b, 1, 32, nir_iadd_imm(b, command, offset),
                          .access = ACCESS_CAN_REORDER, .align_mul = 4);
}

static nir_def *
div_round_up(nir_builder *b, nir_def *value, nir_def *block)
{
   return nir_udiv(b, nir_iadd(b, value, nir_iadd_imm(b, block, -1)), block);
}

nir_shader *
tu_nir_copy_memory_to_image_indirect_vs(const nir_shader_compiler_options *options)
{
   nir_builder build = nir_builder_init_simple_shader(MESA_SHADER_VERTEX, options,
                                                       "indirect memory-to-image vs");
   nir_builder *b = &build;
   nir_variable *position = nir_create_variable_with_location(
      b->shader, nir_var_shader_out, VARYING_SLOT_POS, glsl_vec4_type());
   nir_variable *coords = nir_create_variable_with_location(
      b->shader, nir_var_shader_out, VARYING_SLOT_VAR0, glsl_vec2_type());

   /* c0.xy is the address of the GPU-written indirect record; c1.xy
    * contains the destination format's block width and height.
    */
   nir_def *command = nir_pack_64_2x32(b, load_const(b, 0, 2));
   nir_def *block = load_const(b, 4, 2);
   nir_def *bw = nir_channel(b, block, 0);
   nir_def *bh = nir_channel(b, block, 1);
   nir_def *x = load_command_u32(
      b, command, offsetof(VkCopyMemoryToImageIndirectCommandKHR, imageOffset.x));
   nir_def *y = load_command_u32(
      b, command, offsetof(VkCopyMemoryToImageIndirectCommandKHR, imageOffset.y));
   nir_def *width = load_command_u32(
      b, command, offsetof(VkCopyMemoryToImageIndirectCommandKHR, imageExtent.width));
   nir_def *height = load_command_u32(
      b, command, offsetof(VkCopyMemoryToImageIndirectCommandKHR, imageExtent.height));

   x = nir_udiv(b, x, bw);
   y = nir_udiv(b, y, bh);
   width = div_round_up(b, width, bw);
   height = div_round_up(b, height, bh);
   nir_def *last = nir_ine_imm(b, nir_load_vertex_id(b), 0);
   nir_def *px = nir_bcsel(b, last, nir_iadd(b, x, width), x);
   nir_def *py = nir_bcsel(b, last, nir_iadd(b, y, height), y);
   nir_store_var(b, position,
                 nir_vec4(b, nir_u2f32(b, px), nir_u2f32(b, py),
                           nir_imm_float(b, 0), nir_imm_float(b, 1)), 0xf);
   nir_store_var(b, coords,
                 nir_vec2(b, nir_u2f32(b, nir_bcsel(b, last, width, nir_imm_int(b, 0))),
                           nir_u2f32(b, nir_bcsel(b, last, height, nir_imm_int(b, 0)))), 0x3);
   return b->shader;
}

static nir_def *
load_pixel_word(nir_builder *b, nir_def *address, unsigned bits, unsigned offset)
{
   return nir_u2u32(b, nir_load_global(b, 1, bits, nir_iadd_imm(b, address, offset),
                                      .access = ACCESS_CAN_REORDER,
                                      .align_mul = bits / 8));
}

static nir_def *
load_generic_channel(nir_builder *b, nir_def *address, nir_def *channel, bool integer)
{
   nir_def *shift = nir_iand_imm(b, channel, 0xff);
   nir_def *width = nir_iand_imm(b, nir_ushr_imm(b, channel, 8), 0xff);
   nir_def *conversion = nir_iand_imm(b, nir_ushr_imm(b, channel, 16), 0xff);
   nir_def *byte_offset = nir_ushr_imm(b, shift, 3);
   nir_def *bit_offset = nir_iand_imm(b, shift, 7);
   nir_def *bytes = nir_ushr_imm(b, nir_iadd_imm(b, nir_iadd(b, bit_offset, width), 7), 3);
   nir_variable *packed = nir_local_variable_create(b->impl, glsl_uint64_t_type(), "packed channel");
   nir_store_var(b, packed, nir_imm_int64(b, 0), 1);
   /* A 32-bit channel beginning at bit 7 spans at most five bytes.  Read
    * only the bytes belonging to the channel, including at the buffer end.
    */
   for (unsigned i = 0; i < 5; i++) {
      nir_push_if(b, nir_ult(b, nir_imm_int(b, i), bytes));
      nir_def *ptr = nir_iadd(b, address, nir_u2u64(b, nir_iadd_imm(b, byte_offset, i)));
      nir_def *byte = nir_u2u64(b, load_pixel_word(b, ptr, 8, 0));
      nir_store_var(b, packed,
                    nir_ior(b, nir_load_var(b, packed), nir_ishl_imm(b, byte, i * 8)), 1);
      nir_pop_if(b, NULL);
   }
   nir_def *raw = nir_u2u32(b, nir_ushr(b, nir_load_var(b, packed), bit_offset));
   width = nir_umax_imm(b, width, 1);
   nir_def *width32 = nir_ieq_imm(b, width, 32);
   nir_def *safe_shift = nir_bcsel(b, width32, nir_imm_int(b, 31), width);
   nir_def *mask = nir_bcsel(b, width32, nir_imm_int(b, ~0u),
                             nir_iadd_imm(b, nir_ishl(b, nir_imm_int(b, 1), safe_shift), -1));
   raw = nir_iand(b, raw, mask);
   nir_def *sign_shift = nir_isub(b, nir_imm_int(b, 32), width);
   nir_def *signed_value = nir_ishr(b, nir_ishl(b, raw, sign_shift), sign_shift);
   nir_def *unorm = nir_fdiv(b, nir_u2f32(b, raw), nir_u2f32(b, mask));
   nir_def *signed_max = nir_iadd_imm(
      b, nir_ishl(b, nir_imm_int(b, 1), nir_iadd_imm(b, width, -1)), -1);
   nir_def *snorm = nir_fmax_imm(
      b, nir_fdiv(b, nir_i2f32(b, signed_value), nir_i2f32(b, signed_max)), -1.0);
   nir_def *half = nir_channel(b, nir_unpack_half_2x16(b, raw), 0);
   nir_def *value = integer ? raw : unorm;
   value = nir_bcsel(b, nir_ieq_imm(b, conversion, 0),
                     integer ? nir_imm_int(b, 0) : nir_imm_float(b, 0), value);
   value = nir_bcsel(b, nir_ieq_imm(b, conversion, 1),
                     integer ? nir_imm_int(b, 1) : nir_imm_float(b, 1), value);
   value = nir_bcsel(b, nir_ieq_imm(b, conversion, 3), snorm, value);
   value = nir_bcsel(b, nir_ieq_imm(b, conversion, 4), raw, value);
   value = nir_bcsel(b, nir_ieq_imm(b, conversion, 5), signed_value, value);
   value = nir_bcsel(b, nir_ieq_imm(b, conversion, 6), half, value);
   return nir_bcsel(b, nir_ieq_imm(b, conversion, 7), raw, value);
}

nir_shader *
tu_nir_copy_memory_to_image_indirect_fs(bool integer,
                                        const nir_shader_compiler_options *options)
{
   nir_builder build = nir_builder_init_simple_shader(MESA_SHADER_FRAGMENT, options,
                                                       "indirect memory-to-image fs %s",
                                                       integer ? "uint" : "float");
   nir_builder *b = &build;
   const struct glsl_type *type = integer ? glsl_uvec4_type() : glsl_vec4_type();
   nir_variable *input = nir_create_variable_with_location(
      b->shader, nir_var_shader_in, VARYING_SLOT_VAR0, glsl_vec2_type());
   nir_variable *output = nir_create_variable_with_location(
      b->shader, nir_var_shader_out, FRAG_RESULT_DATA0, type);
   nir_variable *result = nir_local_variable_create(b->impl, type, "copy result");
   nir_store_var(b, result, nir_imm_zero(b, 4, 32), 0xf);

   nir_def *coord = nir_f2u32(b, nir_load_var(b, input));
   nir_def *c0 = load_const(b, 0, 4);
   nir_def *c1 = load_const(b, 4, 4);
   nir_def *c2 = load_const(b, 8, 4);
   nir_def *channels = load_const(b, 12, 4);
   nir_def *command = nir_pack_64_2x32(b, nir_trim_vector(b, c0, 2));
   nir_def *src = nir_pack_64_2x32_split(b, load_command_u32(b, command, 0),
                                       load_command_u32(b, command, 4));
   nir_def *width = load_command_u32(b, command,
                                     offsetof(VkCopyMemoryToImageIndirectCommandKHR, imageExtent.width));
   nir_def *height = load_command_u32(b, command,
                                      offsetof(VkCopyMemoryToImageIndirectCommandKHR, imageExtent.height));
   nir_def *row_length = load_command_u32(b, command,
                                          offsetof(VkCopyMemoryToImageIndirectCommandKHR, bufferRowLength));
   nir_def *image_height = load_command_u32(b, command,
                                            offsetof(VkCopyMemoryToImageIndirectCommandKHR, bufferImageHeight));
   row_length = nir_bcsel(b, nir_ieq_imm(b, row_length, 0), width, row_length);
   image_height = nir_bcsel(b, nir_ieq_imm(b, image_height, 0), height, image_height);
   nir_def *bytes_per_block = nir_u2u64(b, nir_channel(b, c1, 2));
   nir_def *row_pitch = nir_imul(
      b, nir_u2u64(b, div_round_up(b, row_length, nir_channel(b, c1, 0))), bytes_per_block);
   nir_def *layer_pitch = nir_imul(
      b, nir_u2u64(b, div_round_up(b, image_height, nir_channel(b, c1, 1))), row_pitch);
   nir_def *address = nir_iadd(
      b, src, nir_iadd(b, nir_imul(b, nir_u2u64(b, nir_channel(b, coord, 0)), bytes_per_block),
                       nir_iadd(b, nir_imul(b, nir_u2u64(b, nir_channel(b, coord, 1)), row_pitch),
                                 nir_imul(b, nir_u2u64(b, nir_channel(b, c0, 2)), layer_pitch))));
   nir_def *kind = nir_channel(b, c1, 3);

   nir_push_if(b, nir_ieq_imm(b, kind, TU_INDIRECT_COPY_GENERIC));
   nir_def *components[4];
   for (unsigned i = 0; i < 4; i++)
      components[i] = load_generic_channel(b, address, nir_channel(b, channels, i), integer);
   nir_store_var(b, result, nir_vec(b, components, 4), 0xf);
   nir_push_else(b, NULL);
   nir_push_if(b, nir_ieq_imm(b, kind, TU_INDIRECT_COPY_11F11F10F));
   nir_def *packed_float = nir_format_unpack_11f11f10f(b, load_pixel_word(b, address, 32, 0));
   nir_store_var(b, result,
                 nir_vec4(b, nir_channel(b, packed_float, 0), nir_channel(b, packed_float, 1),
                           nir_channel(b, packed_float, 2), nir_imm_float(b, 1)), 0xf);
   nir_push_else(b, NULL);
   if (!integer) {
      nir_push_if(b, nir_ieq_imm(b, kind, TU_INDIRECT_COPY_UNORM8));
      nir_variable *rgba = nir_local_variable_create(b->impl, glsl_vec4_type(), "byte pixel");
      nir_store_var(b, rgba, nir_imm_zero(b, 4, 32), 0xf);
      for (unsigned i = 0; i < 4; i++) {
         nir_push_if(b, nir_ult(b, nir_imm_int(b, i), nir_channel(b, c2, 0)));
         nir_def *value = nir_fmul_imm(b, nir_u2f32(b, load_pixel_word(b, address, 8, i)), 1.0 / 255.0);
         if (i != 3)
            value = nir_bcsel(b, nir_ine_imm(b, nir_channel(b, c2, 1), 0),
                              nir_format_srgb_to_linear(b, value), value);
         nir_store_var(b, rgba, nir_replicate(b, value, 4), 1u << i);
         nir_pop_if(b, NULL);
      }
      nir_def *value = nir_load_var(b, rgba);
      value = nir_bcsel(b, nir_ine_imm(b, nir_channel(b, c2, 2), 0),
                        nir_vec4(b, nir_imm_float(b, 0), nir_imm_float(b, 0),
                                  nir_imm_float(b, 0), nir_channel(b, value, 0)), value);
      nir_store_var(b, result, value, 0xf);
      nir_push_else(b, NULL);
      nir_push_if(b, nir_ieq_imm(b, kind, TU_INDIRECT_COPY_UNORM16));
      nir_store_var(b, result,
                    nir_vec4(b, nir_fmul_imm(b, nir_u2f32(b, load_pixel_word(b, address, 16, 0)),
                                             1.0 / 65535.0),
                              nir_imm_float(b, 0), nir_imm_float(b, 0), nir_imm_float(b, 0)), 0xf);
      nir_push_else(b, NULL);
      nir_store_var(b, result,
                    nir_vec4(b, load_pixel_word(b, address, 32, 0),
                              nir_imm_float(b, 0), nir_imm_float(b, 0), nir_imm_float(b, 0)), 0xf);
      nir_pop_if(b, NULL);
      nir_pop_if(b, NULL);
   } else {
      nir_push_if(b, nir_ieq_imm(b, kind, TU_INDIRECT_COPY_UINT8));
      nir_store_var(b, result,
                    nir_vec4(b, load_pixel_word(b, address, 8, 0), nir_imm_int(b, 0),
                              nir_imm_int(b, 0), nir_imm_int(b, 0)), 0xf);
      nir_push_else(b, NULL);
      nir_store_var(b, result,
                    nir_vec4(b, load_pixel_word(b, address, 32, 0), nir_imm_int(b, 0),
                              nir_imm_int(b, 0), nir_imm_int(b, 0)), 0xf);
      nir_pop_if(b, NULL);
   }
   nir_pop_if(b, NULL);
   nir_pop_if(b, NULL);
   nir_store_var(b, output, nir_load_var(b, result), 0xf);
   return b->shader;
}
