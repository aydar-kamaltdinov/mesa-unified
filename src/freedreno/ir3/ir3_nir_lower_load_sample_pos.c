/*
 * Copyright © 2019 Google, Inc.
 * SPDX-License-Identifier: MIT
 */

#include "compiler/nir/nir_builder.h"
#include "ir3_nir.h"

/**
 * This pass lowers load_sample_pos to load_sample_pos_from_id, and adjusts
 * load_sample_pos_from_id for our 0.0-center RGETPOS instruction.  It needs to
 * happen early, before wpos_ytransform.
 */

static bool
ir3_nir_lower_load_sample_pos_instr(nir_builder *b, nir_intrinsic_instr *intr,
                                    void *data)
{
   b->cursor = nir_after_instr(&intr->instr);
   if (intr->intrinsic == nir_intrinsic_load_sample_pos_from_id) {
      /* RGETPOS has a zero-based center; replace the generic operation so
       * rerunning the pass cannot add the center correction a second time.
       */
      nir_def_replace(&intr->def,
                      nir_fadd_imm(b, nir_load_sample_pos_from_id_ir3(
                                        b, intr->src[0].ssa), 0.5));
      return true;
   } else if (intr->intrinsic == nir_intrinsic_load_sample_pos) {
      nir_def_replace(&intr->def,
                      nir_fadd_imm(b, nir_load_sample_pos_from_id_ir3(
                                        b, nir_load_sample_id(b)), 0.5));
      return true;
   } else {
      return false;
   }
}

bool
ir3_nir_lower_load_sample_pos(nir_shader *shader)
{
   assert(shader->info.stage == MESA_SHADER_FRAGMENT);

   return nir_shader_intrinsics_pass(shader,
                                     ir3_nir_lower_load_sample_pos_instr,
                                     nir_metadata_control_flow, NULL);
}
