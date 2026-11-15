/* SPDX-License-Identifier: MIT */

#include "tu_nir_resolve.h"
#include "compiler/nir/nir_builder.h"

static nir_def *
d24_stencil_from_bytes(nir_builder *b, nir_def *value)
{
   nir_def *bytes = nir_f2u32(b, nir_fround_even(b, nir_fmul_imm(b, value, 255.0f)));
   nir_def *depth = nir_ior(b, nir_channel(b, bytes, 0),
                          nir_ior(b, nir_ishl_imm(b, nir_channel(b, bytes, 1), 8),
                                  nir_ishl_imm(b, nir_channel(b, bytes, 2), 16)));
   return nir_vec2(b, depth, nir_channel(b, bytes, 3));
}

static nir_def *
d24_stencil_to_bytes(nir_builder *b, nir_def *value)
{
   nir_def *depth = nir_channel(b, value, 0);
   nir_def *bytes = nir_vec4(b, nir_iand_imm(b, depth, 0xff),
                              nir_iand_imm(b, nir_ushr_imm(b, depth, 8), 0xff),
                              nir_iand_imm(b, nir_ushr_imm(b, depth, 16), 0xff),
                              nir_channel(b, value, 1));
   /* Original literal at ELF0xb0568 is the F32 reciprocal promoted to F64. */
   return nir_fmul_imm(b, nir_u2f32(b, bytes), 1.0f / 255.0f);
}

static nir_def *
d24_average(nir_builder *b, nir_def *sum, nir_def *samples)
{
   /* Sum at most 64 unsigned 24-bit samples without losing low bits through
    * F32 conversion. Round the rational average to nearest, ties to even.
    */
   nir_def *quotient = nir_udiv(b, sum, samples);
   nir_def *twice_remainder = nir_ishl_imm(b, nir_umod(b, sum, samples), 1);
   nir_def *round_up = nir_ior(
      b, nir_ult(b, samples, twice_remainder),
      nir_iand(b, nir_ieq(b, twice_remainder, samples),
                  nir_ine_imm(b, nir_iand_imm(b, quotient, 1), 0)));
   return nir_iadd(b, quotient, nir_b2i32(b, round_up));
}

static nir_shader *
build_resolve_fs(VkResolveModeFlagBits mode, bool integer, bool d24,
                 const nir_shader_compiler_options *options)
{
   nir_builder build = nir_builder_init_simple_shader(
      MESA_SHADER_FRAGMENT, options, "extended resolve fs mode=%u integer=%u d24=%u",
      mode, integer, d24);
   nir_builder *b = &build;
   const struct glsl_type *type = integer ? glsl_uvec4_type() : glsl_vec4_type();
   nir_variable *out = nir_create_variable_with_location(
      b->shader, nir_var_shader_out, FRAG_RESULT_DATA0, type);
   nir_variable *in = nir_create_variable_with_location(
      b->shader, nir_var_shader_in, VARYING_SLOT_VAR0, glsl_vec2_type());
   nir_variable *accum = nir_local_variable_create(
      b->impl, d24 ? glsl_uvec2_type() : type, "accum");
   nir_variable *index = nir_local_variable_create(b->impl, glsl_uint_type(), "sample");

   b->shader->info.num_textures = 1;
   BITSET_SET(b->shader->info.textures_used, 0);
   BITSET_SET(b->shader->info.textures_used_by_txf, 0);
   nir_def *coord = nir_f2i32(b, nir_load_var(b, in));
   nir_def *samples = nir_texture_samples(b, .dim = GLSL_SAMPLER_DIM_MS,
                                         .dest_type = nir_type_int32);
   nir_alu_type dest_type = integer ? nir_type_uint32 : nir_type_float32;
   nir_def *first = nir_txf_ms(b, coord, nir_imm_int(b, 0),
                               .dim = GLSL_SAMPLER_DIM_MS, .dest_type = dest_type);
   if (d24)
      first = d24_stencil_from_bytes(b, first);
   nir_store_var(b, accum, first, d24 ? 0x3 : 0xf);
   nir_store_var(b, index, nir_imm_int(b, 1), 1);

   nir_push_loop(b);
   nir_def *i = nir_load_var(b, index);
   nir_push_if(b, nir_uge(b, i, samples));
   nir_jump(b, nir_jump_break);
   nir_pop_if(b, NULL);

   nir_def *value = nir_txf_ms(b, coord, i, .dim = GLSL_SAMPLER_DIM_MS,
                               .dest_type = dest_type);
   if (d24)
      value = d24_stencil_from_bytes(b, value);
   nir_def *sum = nir_load_var(b, accum);
   if (mode == VK_RESOLVE_MODE_AVERAGE_BIT) {
      if (d24)
         sum = nir_vec2(b, nir_iadd(b, nir_channel(b, sum, 0),
                                      nir_channel(b, value, 0)),
                           nir_channel(b, sum, 1));
      else
         sum = nir_fadd(b, sum, value);
   }
   else if (mode == VK_RESOLVE_MODE_MIN_BIT)
      sum = integer || d24 ? nir_umin(b, sum, value) : nir_fmin(b, sum, value);
   else
      sum = integer || d24 ? nir_umax(b, sum, value) : nir_fmax(b, sum, value);
   nir_store_var(b, accum, sum, d24 ? 0x3 : 0xf);
   nir_store_var(b, index, nir_iadd_imm(b, i, 1), 1);
   nir_pop_loop(b, NULL);

   nir_def *result = nir_load_var(b, accum);
   if (mode == VK_RESOLVE_MODE_AVERAGE_BIT) {
      if (d24)
         result = nir_vec2(b, d24_average(b, nir_channel(b, result, 0), samples),
                              nir_channel(b, result, 1));
      else
         result = nir_fdiv(b, result, nir_i2f32(b, samples));
   }
   if (d24)
      result = d24_stencil_to_bytes(b, result);
   nir_store_var(b, out, result, 0xf);
   return b->shader;
}

nir_shader *
tu_nir_resolve_fs(VkResolveModeFlagBits mode, bool integer,
                  const nir_shader_compiler_options *options)
{
   /* GameSir global shader slots 9..13 instantiate precisely these modes.
    * The integer path is for unsigned stencil, not signed color resolves.
    */
   if (mode != VK_RESOLVE_MODE_MIN_BIT && mode != VK_RESOLVE_MODE_MAX_BIT &&
       (mode != VK_RESOLVE_MODE_AVERAGE_BIT || integer))
      return NULL;

   return build_resolve_fs(mode, integer, false, options);
}

nir_shader *
tu_nir_resolve_d24_min_fs(const nir_shader_compiler_options *options)
{
   /* GameSir global slot14. Do not compare the three depth bytes separately:
    * e.g. depth0x010000 must lose to0x00ffff despite having a smaller R byte.
    * Stencil uses its own minimum, independently of the winning depth sample.
    */
   return build_resolve_fs(VK_RESOLVE_MODE_MIN_BIT, false, true, options);
}

nir_shader *
tu_nir_resolve_d24_fs(VkResolveModeFlagBits mode,
                     const nir_shader_compiler_options *options)
{
   if (mode != VK_RESOLVE_MODE_MIN_BIT && mode != VK_RESOLVE_MODE_MAX_BIT &&
       mode != VK_RESOLVE_MODE_AVERAGE_BIT)
      return NULL;
   return build_resolve_fs(mode, false, true, options);
}
