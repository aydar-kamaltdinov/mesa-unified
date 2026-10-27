/*
 * Copyright © 2025 Red Hat
 * SPDX-License-Identifier: MIT
 *
 * Software subgroup matrix layout recovered from GameSir's Turnip build.
 * Type/deref lowering follows Mesa's lavapipe cooperative-matrix pass.
 */
#include "tu_nir_cooperative_matrix.h"
#include "compiler/nir/nir_builder.h"
#include "util/hash_table.h"

struct lower_cmat_ctx {
   struct hash_table *types;
   nir_variable *scratch;
   bool use_shared;
};

static unsigned
cmat_length(const struct glsl_cmat_description *desc)
{
   return desc->rows * desc->cols / 64;
}

static const struct glsl_type *
remap_type(struct lower_cmat_ctx *ctx, const struct glsl_type *type)
{
   struct hash_entry *entry = _mesa_hash_table_search(ctx->types, type);
   if (entry)
      return entry->data;
   const struct glsl_type *base = glsl_without_array(type);
   const struct glsl_type *result = type;
   if (glsl_type_is_cmat(base)) {
      const struct glsl_cmat_description *desc = glsl_get_cmat_description(base);
      result = glsl_type_wrap_in_arrays(
         glsl_vector_type(desc->element_type, cmat_length(desc)), type);
   }
   _mesa_hash_table_insert(ctx->types, type, (void *)result);
   return result;
}

static const struct glsl_cmat_description *
cmat_desc(nir_src src)
{
   return glsl_get_cmat_description(nir_src_as_deref(src)->type);
}

static nir_def *
load_cmat(nir_builder *b, nir_src src)
{
   const struct glsl_cmat_description *desc = cmat_desc(src);
   return nir_build_load_deref(b, cmat_length(desc),
                               glsl_base_type_bit_size(desc->element_type),
                               src.ssa, 0);
}

static void
store_cmat(nir_builder *b, nir_src dst, nir_def *value)
{
   nir_store_deref(b, nir_src_as_deref(dst), value, ~0u);
}

static bool
lower_saturating_muladd(nir_builder *b, nir_intrinsic_instr *intr, void *data)
{
   if (intr->intrinsic != nir_intrinsic_cmat_muladd ||
       !nir_intrinsic_saturate(intr))
      return false;

   const struct glsl_cmat_description *desc = cmat_desc(intr->src[0]);
   assert(glsl_base_type_is_integer(desc->element_type));
   b->cursor = nir_before_instr(&intr->instr);
   nir_variable *tmp = nir_local_variable_create(
      b->impl, nir_src_as_deref(intr->src[0])->type, "cmat product");
   nir_deref_instr *product = nir_build_deref_var(b, tmp);
   nir_cmat_construct(b, &product->def,
                       nir_imm_zero(b, 1, glsl_base_type_bit_size(desc->element_type)));
   nir_cmat_muladd(b, &product->def, intr->src[1].ssa, intr->src[2].ssa,
                   &product->def,
                   .cmat_signed_mask = nir_intrinsic_cmat_signed_mask(intr));
   /* SPV_KHR_cooperative_matrix saturates A*B + C, not each product or
    * each partial K tile. Split the full product before adding C once.
    * Overflow in the intermediate product is undefined for this operation.
    */
   nir_cmat_binary_op(b, intr->src[0].ssa, &product->def, intr->src[3].ssa,
                      .alu_op = (nir_intrinsic_cmat_signed_mask(intr) &
                                 NIR_CMAT_RESULT_SIGNED) ?
                         nir_op_iadd_sat : nir_op_uadd_sat);
   nir_instr_remove(&intr->instr);
   return true;
}

static void
cmat_barrier(nir_builder *b)
{
   nir_barrier(b, .execution_scope = SCOPE_SUBGROUP,
                .memory_scope = SCOPE_SUBGROUP,
                .memory_semantics = NIR_MEMORY_ACQ_REL,
                .memory_modes = nir_var_mem_shared);
}

static nir_deref_instr *
cmat_scratch(nir_builder *b, struct lower_cmat_ctx *ctx, nir_def *index)
{
   if (!ctx->scratch)
      ctx->scratch = nir_variable_create(
         b->shader, nir_var_mem_shared,
         glsl_array_type(glsl_uint_type(), 2048, 4), "cmat scratch");
   return nir_build_deref_array(b, nir_build_deref_var(b, ctx->scratch), index);
}

static nir_def *
lower_muladd(nir_builder *b, struct lower_cmat_ctx *ctx, nir_intrinsic_instr *intr)
{
   assert(!nir_intrinsic_saturate(intr));
   const struct glsl_cmat_description *desc = cmat_desc(intr->src[0]);
   assert(desc->rows == 8 && desc->cols == 8);
   nir_def *a = load_cmat(b, intr->src[1]);
   nir_def *bb = load_cmat(b, intr->src[2]);
   nir_def *result = load_cmat(b, intr->src[3]);
   assert(a->num_components == 1 && bb->num_components == 1);
   nir_def *lane = nir_load_subgroup_invocation(b);
   nir_def *row_base = nir_iand_imm(b, lane, ~7u);
   nir_def *col = nir_iand_imm(b, lane, 7);
   nir_def *scratch_base = NULL;
   if (ctx->use_shared) {
      scratch_base = nir_imul_imm(b, nir_load_subgroup_id(b), 128);
      nir_def *index = nir_iadd(b, scratch_base, lane);
      nir_store_deref(b, cmat_scratch(b, ctx, index), nir_u2u32(b, a), 1);
      nir_store_deref(b, cmat_scratch(b, ctx, nir_iadd_imm(b, index, 64)),
                       nir_u2u32(b, bb), 1);
      cmat_barrier(b);
   }
   const bool integer = glsl_base_type_is_integer(desc->element_type);
   const unsigned bits = result->bit_size;
   const nir_cmat_signed sign = nir_intrinsic_cmat_signed_mask(intr);
   for (unsigned k = 0; k < 8; k++) {
      nir_def *ai = nir_iadd_imm(b, row_base, k);
      nir_def *bi = nir_iadd_imm(b, col, k * 8);
      nir_def *av, *bv;
      if (ctx->use_shared) {
         av = nir_u2uN(b, nir_load_deref(b, cmat_scratch(
            b, ctx, nir_iadd(b, scratch_base, ai))), a->bit_size);
         bv = nir_u2uN(b, nir_load_deref(b, cmat_scratch(
            b, ctx, nir_iadd_imm(b, nir_iadd(b, scratch_base, bi), 64))), bb->bit_size);
      } else {
         av = nir_shuffle(b, a, ai);
         bv = nir_shuffle(b, bb, bi);
      }
      if (integer) {
         av = (sign & NIR_CMAT_A_SIGNED) ? nir_i2iN(b, av, bits) : nir_u2uN(b, av, bits);
         bv = (sign & NIR_CMAT_B_SIGNED) ? nir_i2iN(b, bv, bits) : nir_u2uN(b, bv, bits);
         nir_def *product = nir_imul(b, av, bv);
         result = nir_iadd(b, result, product);
      } else {
         av = nir_f2fN(b, av, bits);
         bv = nir_f2fN(b, bv, bits);
         result = nir_fadd(b, result, nir_fmul(b, av, bv));
      }
   }
   if (ctx->use_shared)
      cmat_barrier(b);
   return result;
}

static void
lower_load_store(nir_builder *b, nir_intrinsic_instr *intr)
{
   const bool load = intr->intrinsic == nir_intrinsic_cmat_load;
   const struct glsl_cmat_description *desc = cmat_desc(intr->src[!load]);
   assert(desc->rows == 8 && desc->cols == 8);
   nir_deref_instr *ptr = nir_src_as_deref(intr->src[load]);
   const unsigned ptr_stride = glsl_get_bit_size(ptr->type) / 8 *
                               glsl_get_vector_elements(ptr->type);
   ptr = nir_build_deref_cast(b, &ptr->def, ptr->modes, ptr->type, ptr_stride);
   nir_def *lane = nir_load_subgroup_invocation(b);
   nir_def *row = nir_udiv_imm(b, lane, desc->cols);
   nir_def *col = nir_umod_imm(b, lane, desc->cols);
   if (nir_intrinsic_matrix_layout(intr) == GLSL_MATRIX_LAYOUT_COLUMN_MAJOR)
      SWAP(row, col);
   const unsigned index_bits = ptr->def.bit_size;
   nir_def *major = nir_imul(b, nir_u2uN(b, row, index_bits),
                                nir_u2uN(b, intr->src[2].ssa, index_bits));
   ptr = nir_build_deref_ptr_as_array(b, ptr, major);
   ptr = nir_build_deref_cast(b, &ptr->def, ptr->modes,
                              glsl_scalar_type(desc->element_type),
                              glsl_base_type_bit_size(desc->element_type) / 8);
   ptr = nir_build_deref_ptr_as_array(b, ptr, nir_u2uN(b, col, index_bits));
   if (load)
      store_cmat(b, intr->src[0], nir_load_deref(b, ptr));
   else
      nir_store_deref(b, ptr, load_cmat(b, intr->src[1]), 1);
}

static bool
lower_intrinsic(nir_builder *b, struct lower_cmat_ctx *ctx, nir_intrinsic_instr *intr)
{
   nir_def *value = NULL;
   switch (intr->intrinsic) {
   case nir_intrinsic_cmat_binary_op:
   case nir_intrinsic_cmat_scalar_op:
   case nir_intrinsic_cmat_unary_op: {
      b->fp_math_ctrl = nir_intrinsic_fp_math_ctrl(intr);
      nir_op op = nir_intrinsic_alu_op(intr);
      nir_def *src = load_cmat(b, intr->src[1]);
      value = intr->intrinsic == nir_intrinsic_cmat_unary_op ? nir_build_alu1(b, op, src) :
         nir_build_alu2(b, op, src, intr->intrinsic == nir_intrinsic_cmat_scalar_op ?
                                      intr->src[2].ssa : load_cmat(b, intr->src[2]));
      b->fp_math_ctrl = nir_fp_fast_math;
      break;
   }
   case nir_intrinsic_cmat_bitcast:
      value = load_cmat(b, intr->src[1]);
      break;
   case nir_intrinsic_cmat_convert: {
      b->fp_math_ctrl = nir_intrinsic_fp_math_ctrl(intr);
      value = nir_type_convert(b, load_cmat(b, intr->src[1]),
         nir_get_nir_type_for_glsl_base_type(cmat_desc(intr->src[1])->element_type),
         nir_get_nir_type_for_glsl_base_type(cmat_desc(intr->src[0])->element_type),
         nir_rounding_mode_undef);
      b->fp_math_ctrl = nir_fp_fast_math;
      break;
   }
   case nir_intrinsic_cmat_construct:
      value = nir_replicate(b, intr->src[1].ssa, cmat_length(cmat_desc(intr->src[0])));
      break;
   case nir_intrinsic_cmat_copy:
      nir_build_copy_deref(b, intr->src[0].ssa, intr->src[1].ssa);
      break;
   case nir_intrinsic_cmat_extract:
      nir_def_replace(&intr->def, nir_vector_extract(b, load_cmat(b, intr->src[0]),
                                                      intr->src[1].ssa));
      return true;
   case nir_intrinsic_cmat_insert:
      value = nir_vector_insert(b, load_cmat(b, intr->src[2]), intr->src[1].ssa,
                                 intr->src[3].ssa);
      break;
   case nir_intrinsic_cmat_length: {
      const struct glsl_cmat_description desc = nir_intrinsic_cmat_desc(intr);
      nir_def_replace(&intr->def, nir_imm_int(b, cmat_length(&desc)));
      return true;
   }
   case nir_intrinsic_cmat_load:
   case nir_intrinsic_cmat_store:
      lower_load_store(b, intr);
      break;
   case nir_intrinsic_cmat_muladd:
      value = lower_muladd(b, ctx, intr);
      break;
   default:
      return false;
   }
   if (value)
      store_cmat(b, intr->src[0], value);
   nir_instr_remove(&intr->instr);
   return true;
}


bool
tu_nir_lower_cooperative_matrix(nir_shader *shader)
{
   if (shader->info.stage != MESA_SHADER_COMPUTE || !shader->info.cs.has_cooperative_matrix)
      return false;
   const struct nir_lower_coopmat_args args = {8, 8, 8};
   bool progress = nir_shader_intrinsics_pass(shader, lower_saturating_muladd,
                                              nir_metadata_control_flow, NULL);
   progress |= nir_lower_cooperative_matrix_flexible_dimensions(shader, &args);
   if (progress) {
      nir_opt_deref(shader);
      nir_opt_dce(shader);
      nir_remove_dead_variables(shader, nir_var_shader_temp | nir_var_function_temp, NULL);
   }
   struct lower_cmat_ctx ctx = { .types = _mesa_pointer_hash_table_create(NULL),
                                 .use_shared = true };
   nir_foreach_variable_with_modes(var, shader, nir_var_mem_shared)
      ctx.use_shared = false;
   nir_foreach_variable_with_modes(var, shader, nir_var_shader_temp) {
      const struct glsl_type *type = remap_type(&ctx, var->type);
      progress |= type != var->type;
      var->type = type;
   }
   nir_foreach_function_impl(impl, shader) {
      bool changed = false;
      nir_foreach_function_temp_variable(var, impl) {
         const struct glsl_type *type = remap_type(&ctx, var->type);
         changed |= type != var->type;
         var->type = type;
      }
      nir_builder b = nir_builder_create(impl);
      nir_foreach_block_reverse_safe(block, impl) {
         nir_foreach_instr_reverse_safe(instr, block) {
            b.cursor = nir_before_instr(instr);
            if (instr->type == nir_instr_type_intrinsic)
               changed |= lower_intrinsic(&b, &ctx, nir_instr_as_intrinsic(instr));
            else if (instr->type == nir_instr_type_deref) {
               nir_deref_instr *deref = nir_instr_as_deref(instr);
               const struct glsl_type *type = remap_type(&ctx, deref->type);
               changed |= type != deref->type;
               deref->type = type;
            }
         }
      }
      progress |= nir_progress(changed, impl, nir_metadata_none);
   }
   _mesa_hash_table_destroy(ctx.types, NULL);
   return progress;
}
