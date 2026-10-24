/*
 * Copyright © 2019 Google, Inc.
 * SPDX-License-Identifier: MIT
 */

#include "compiler/nir/nir_builder.h"
#include "ir3_nir.h"
#include "util/u_debug.h"

/**
 * This pass lowers load_barycentric_at_offset to dsx.3d/dsy.3d and alu
 * instructions.
 */

bool
ir3_gamesir_sample_center_enabled(void)
{
   return debug_get_bool_option("IR3_GAMESIR_SAMPLE_CENTER", false);
}

struct sample_center_options {
   bool enable;
};

/* Original walks ALU sources only, without crossing phi/control flow.
 * Cap the optional recognition walk so a large shared expression cannot
 * cause unbounded compiler recursion or repeated DAG expansion. A miss
 * keeps the existing derivative-based lowering.
 */
static bool
uses_sample_pos_ir3(nir_def *def, unsigned depth, unsigned *remaining)
{
   if (!depth || !*remaining)
      return false;
   --*remaining;
   nir_instr *instr = nir_def_instr(def);
   if (instr->type == nir_instr_type_intrinsic)
      return nir_instr_as_intrinsic(instr)->intrinsic ==
             nir_intrinsic_load_sample_pos_from_id_ir3;
   if (instr->type != nir_instr_type_alu)
      return false;
   nir_alu_instr *alu = nir_instr_as_alu(instr);
   for (unsigned i = 0; i < nir_op_infos[alu->op].num_inputs; i++) {
      if (uses_sample_pos_ir3(alu->src[i].src.ssa, depth - 1, remaining))
         return true;
   }
   return false;
}

static nir_def *
ir3_nir_lower_load_barycentric_at_offset_instr(nir_builder *b, nir_instr *instr,
                                               void *data)
{
   nir_intrinsic_instr *intr = nir_instr_as_intrinsic(instr);
   enum glsl_interp_mode interp_mode = nir_intrinsic_interp_mode(intr);
   const struct sample_center_options *options = data;

#define chan(var, c) nir_channel(b, var, c)

   nir_def *off = intr->src[0].ssa;
   /* note: at_offset is defined to be relative to the center of the pixel */
   nir_def *ij = nir_load_barycentric_pixel(b, 32, .interp_mode = interp_mode);

   unsigned remaining = 1024;
   nir_def *at_center = options->enable &&
                       uses_sample_pos_ir3(off, 64, &remaining) ?
      nir_ball_fequal2(b, off, nir_imm_vec2(b, 0.0, 0.0)) : NULL;
   nir_def *result;

   /* Need helper invocations for our ddx/ddys to work. */
   if (b->shader->info.stage == MESA_SHADER_FRAGMENT)
      b->shader->info.fs.needs_coarse_quad_helper_invocations = true;

   if (interp_mode != INTERP_MODE_SMOOTH) {
      /* Offset our pixel center ij by the offset argument (units of pixels)
       * times the derivatives of ij in screen space.
       */
      nir_def *new_ij = ij;
      new_ij = nir_fmad(b, chan(off, 0), nir_ddx(b, ij), new_ij);
      new_ij = nir_fmad(b, chan(off, 1), nir_ddy(b, ij), new_ij);

      result = new_ij;
   } else {
      nir_def *center_w = nir_frcp(b, nir_load_persp_center_rhw_ir3(b, 32));

      /* scaled ij -- ij comes in multiplied by by 1/center_w so multiply that
       * back out, plus add center_w as the 3rd component for taking the
       * derivatives.
       *
       * We actually suspect that we should be using rhw here instead of center_w,
       * but no tests seem to distinguish between the two.
       */
      nir_def *sij =
         nir_vec3(b, nir_fmul(b, chan(ij, 0), center_w), nir_fmul(b, chan(ij, 1), center_w), center_w);

      /* Get the offset value from pixel center for ij, and also for w. */
      nir_def *pos = sij;
      pos = nir_fmad(b, chan(off, 0), nir_ddx_3d_ir3(b, sij), pos);
      pos = nir_fmad(b, chan(off, 1), nir_ddy_3d_ir3(b, sij), pos);

      /* convert back into screen space, dividing by the offset 1/w */
      result = nir_fmul(b, nir_trim_vector(b, pos, 2),
                       nir_frcp(b, chan(pos, 2)));
   }
   /* Preserve original source order. Its center predicate/selection
    * combination is controversial; do not turn this on as a generic fix.
    */
   return at_center ?
      nir_select_barycentric_at_center_ir3(b, at_center, result, ij) : result;
}

static bool
ir3_nir_lower_load_barycentric_at_offset_filter(const nir_instr *instr,
                                                const void *data)
{
   return (instr->type == nir_instr_type_intrinsic &&
           nir_instr_as_intrinsic(instr)->intrinsic ==
              nir_intrinsic_load_barycentric_at_offset);
}

bool
ir3_nir_lower_load_barycentric_at_offset(nir_shader *shader,
                                             struct ir3_compiler *compiler)
{
   assert(shader->info.stage == MESA_SHADER_FRAGMENT);
   const bool request_sample_center = ir3_gamesir_sample_center_enabled();
   const struct sample_center_options options = {
      .enable = compiler->gen < 8 && request_sample_center,
   };
   return nir_shader_lower_instructions(
      shader, ir3_nir_lower_load_barycentric_at_offset_filter,
      ir3_nir_lower_load_barycentric_at_offset_instr, (void *)&options);
}


static bool
lower_coord_at_offset(nir_builder *b, nir_intrinsic_instr *intr, void *data)
{
   if (intr->intrinsic != nir_intrinsic_load_barycentric_coord_at_offset &&
       intr->intrinsic != nir_intrinsic_load_barycentric_coord_at_sample)
      return false;

   b->cursor = nir_before_instr(&intr->instr);
   nir_def *offset = intr->src[0].ssa;
   if (intr->intrinsic == nir_intrinsic_load_barycentric_coord_at_sample)
      offset = nir_fadd_imm(b, nir_load_sample_pos_from_id(
                                 b, intr->def.bit_size, offset), -0.5);

   enum glsl_interp_mode mode = nir_intrinsic_interp_mode(intr);
   nir_def *ij = nir_load_barycentric_at_offset(
      b, intr->def.bit_size, offset, .interp_mode = mode);
   nir_def_replace(&intr->def, nir_load_barycentric_coord_ir3(
      b, intr->def.bit_size, ij, .interp_mode = mode));
   return true;
}

bool
ir3_nir_lower_barycentric_coord_at_offset(nir_shader *shader)
{
   assert(shader->info.stage == MESA_SHADER_FRAGMENT);
   return nir_shader_intrinsics_pass(shader, lower_coord_at_offset,
                                    nir_metadata_control_flow, NULL);
}
