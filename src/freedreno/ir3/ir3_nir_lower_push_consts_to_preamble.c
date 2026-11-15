/*
 * Copyright © 2023 Igalia S.L.
 * SPDX-License-Identifier: MIT
 */

#include "compiler/nir/nir.h"
#include "compiler/nir/nir_builder.h"
#include "util/u_math.h"
#include "ir3_compiler.h"
#include "ir3_nir.h"

bool
ir3_nir_lower_push_consts_to_preamble(nir_shader *nir,
                                      struct ir3_shader_variant *v)
{
   const struct ir3_const_state *const_state = ir3_const_state(v);
   nir_function_impl *preamble = nir_shader_get_preamble(nir);
   nir_builder _b = nir_builder_at(nir_before_impl(preamble));
   nir_builder *b = &_b;

   /* Native ELF 0xa8d8c4 scans the absolute ViewIndex slot before adding
    * copies. TASK/MESH and mesh-multiview FS reserve the prefix at TU level.
    */
   bool reads_view_index = false;
   nir_foreach_function_impl(impl, nir) {
      nir_foreach_block(block, impl) {
         nir_foreach_instr(instr, block) {
            if (instr->type != nir_instr_type_intrinsic)
               continue;
            nir_intrinsic_instr *intr = nir_instr_as_intrinsic(instr);
            if (intr->intrinsic == nir_intrinsic_load_const_ir3 &&
                nir_intrinsic_base(intr) == 33)
               reads_view_index = true;
         }
      }
   }

   if (v->shader_options.push_consts_type == IR3_PUSH_CONSTS_SHARED_PREAMBLE) {
      uint32_t offset_vec4 =
         const_state->allocs.consts[IR3_CONST_ALLOC_PUSH_CONSTS].offset_vec4 * 4;
      nir_copy_push_const_to_uniform_ir3(
         b, nir_imm_int(b, offset_vec4),
         .base = v->shader_options.push_consts_base,
         .range = v->shader_options.push_consts_dwords);
   }

   /* A8 SP_SHARED_CONSTANT_GFX starts at 0xab30: the AQE draw-state writes
    * DrawID/ViewIndex at 0xab78/0xab79, i.e. shared scalar words 72/73.
    * Copy to the shader's fixed AQE prefix without treating them as API
    * push constants. The existing macro expansion emits the shared copy.
    * Require the named FS prefix contract: ordinary UBO lowering can also
    * produce base33, which alone does not identify an AQE ViewIndex load.
    */
   if (mesa_shader_stage_is_mesh(nir->info.stage)) {
      nir_copy_push_const_to_uniform_ir3(
         b, nir_imm_int(b, 32), .base = 72,
         .range = reads_view_index ? 2 : 1);
   } else if (v->shader_options.mesh_multiview && reads_view_index) {
      nir_copy_push_const_to_uniform_ir3(
         b, nir_imm_int(b, 33), .base = 73, .range = 1);
   }

   nir_foreach_function_impl(impl, nir) {
      nir_progress(true, impl, nir_metadata_none);
   }
   return true;
}
