/* SPDX-License-Identifier: MIT */

#include "compiler/nir/nir.h"
#include "ir3_nir.h"

/* Recovered from GameSir ELF 0xa93918, 0xa93b98 and 0xa93e3c.
 * Check the whole shader before moving anything: a late phi, texture lookup
 * or non-reorderable intrinsic in any input's dependency chain cancels this
 * pass, including otherwise movable inputs in other blocks/functions.
 */
static bool
is_varying_input(nir_instr *instr)
{
   if (instr->type != nir_instr_type_intrinsic)
      return false;

   switch (nir_instr_as_intrinsic(instr)->intrinsic) {
   case nir_intrinsic_load_input:
   case nir_intrinsic_load_interpolated_input:
   case nir_intrinsic_load_per_primitive_input:
      return true;
   default:
      return false;
   }
}

static bool
check_precondition_src(nir_src *src, void *data)
{
   nir_block *start = data;
   nir_instr *instr = nir_def_instr(src->ssa);
   if (instr->block == start)
      return true;

   switch (instr->type) {
   case nir_instr_type_alu:
   case nir_instr_type_deref:
   case nir_instr_type_load_const:
   case nir_instr_type_undef:
      break;
   case nir_instr_type_intrinsic:
      if (!nir_intrinsic_can_reorder(nir_instr_as_intrinsic(instr)))
         return false;
      break;
   default:
      return false;
   }
   return nir_foreach_src(instr, check_precondition_src, start);
}

static bool
move_source_to_start(nir_src *src, void *data)
{
   nir_block *start = data;
   nir_instr *instr = nir_def_instr(src->ssa);
   if (instr->block != start) {
      nir_foreach_src(instr, move_source_to_start, start);
      nir_instr_move(nir_after_block(start), instr);
   }
   return true;
}

bool
ir3_nir_move_varying_inputs(nir_shader *shader)
{
   nir_foreach_function_impl(impl, shader) {
      nir_block *start = nir_start_block(impl);
      nir_foreach_block(block, impl) {
         if (block == start)
            continue;
         nir_foreach_instr(instr, block) {
            if (is_varying_input(instr) &&
                !nir_foreach_src(instr, check_precondition_src, start))
               return false;
         }
      }
   }

   bool progress = false;
   nir_foreach_function_impl(impl, shader) {
      bool impl_progress = false;
      nir_block *start = nir_start_block(impl);
      nir_foreach_block(block, impl) {
         if (block == start)
            continue;
         nir_foreach_instr_safe(instr, block) {
            if (!is_varying_input(instr))
               continue;
            nir_foreach_src(instr, move_source_to_start, start);
            nir_instr_move(nir_after_block(start), instr);
            impl_progress = true;
         }
      }
      progress |= nir_progress(impl_progress, impl, nir_metadata_control_flow);
   }
   return progress;
}
