/* SPDX-License-Identifier: MIT */
/* Executed NIR regression fixtures; host-only, no Vulkan/GPU equivalence claim. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "compiler/nir/nir_builder.h"
#include "ir3/ir3_nir.h"
#include "vulkan/tu_nir_cooperative_matrix.h"

static const nir_shader_compiler_options options = {0};

static nir_def *
input(nir_builder *b, nir_def *offset)
{
   nir_def *coord = nir_u2f32(b, offset);
   return nir_load_interpolated_input(b, 1, 32, nir_vec2(b, coord, coord),
      nir_imm_int(b, 0), .base = 0,
      .io_semantics = {.location = VARYING_SLOT_VAR0, .num_slots = 1});
}

static void
hoist_fixture(unsigned depth, bool reject_phi)
{
   nir_builder b = nir_builder_init_simple_shader(MESA_SHADER_FRAGMENT,
                                                  &options, "hoist fixture");
   nir_block *start = nir_start_block(b.impl);
   nir_def *condition = nir_load_front_face(&b, 1);
   nir_push_if(&b, condition);
   nir_def *offset = nir_imm_int(&b, 0);
   for (unsigned i = 0; i < depth; i++)
      offset = nir_iadd(&b, offset, offset);
   nir_def *a = input(&b, offset);
   nir_block *old_block = nir_def_instr(a)->block;
   nir_store_global(&b, a, nir_imm_int64(&b, 0x1000), .align_mul = 4);
   nir_def *left = nir_imm_int(&b, 0);
   nir_push_else(&b, NULL);
   nir_def *right = nir_imm_int(&b, 1);
   nir_pop_if(&b, NULL);
   nir_def *phi = nir_if_phi(&b, left, right);
   nir_def *second = NULL;
   if (reject_phi) {
      second = input(&b, phi);
      nir_store_global(&b, second, nir_imm_int64(&b, 0x1004), .align_mul = 4);
   }
   nir_validate_shader(b.shader, "before hoist");
   bool progress = ir3_nir_move_varying_inputs(b.shader);
   assert(progress == !reject_phi);
   assert(nir_def_instr(a)->block == (reject_phi ? old_block : start));
   if (second)
      assert(nir_def_instr(second)->block != start);
   nir_validate_shader(b.shader, "after hoist");
   assert(!ir3_nir_move_varying_inputs(b.shader));
   ralloc_free(b.shader);
}

static nir_deref_instr *
matrix(nir_builder *b, const char *name, unsigned rows, unsigned cols,
       enum glsl_base_type element, enum glsl_cmat_use use)
{
   struct glsl_cmat_description desc = {
      .rows = rows, .cols = cols, .element_type = element,
      .scope = SCOPE_SUBGROUP, .use = use,
   };
   nir_variable *v = nir_local_variable_create(b->impl, glsl_cmat_type(&desc), name);
   return nir_build_deref_var(b, v);
}

static void
matrix_fixture(unsigned m, unsigned n, unsigned k, bool sign,
               bool saturate, bool existing_shared)
{
   nir_builder b = nir_builder_init_simple_shader(MESA_SHADER_COMPUTE,
                                                  &options, "matrix fixture");
   b.shader->info.workgroup_size[0] = 64;
   b.shader->info.workgroup_size[1] = b.shader->info.workgroup_size[2] = 1;
   b.shader->info.cs.has_cooperative_matrix = true;
   if (existing_shared)
      nir_variable_create(b.shader, nir_var_mem_shared, glsl_uint_type(), "user shared");
   enum glsl_base_type ab_type = sign ? GLSL_TYPE_INT8 : GLSL_TYPE_UINT8;
   enum glsl_base_type cd_type = sign ? GLSL_TYPE_INT : GLSL_TYPE_UINT;
   nir_deref_instr *a = matrix(&b, "input A", m, k, ab_type, GLSL_CMAT_USE_A);
   nir_deref_instr *bb = matrix(&b, "input B", k, n, ab_type, GLSL_CMAT_USE_B);
   nir_deref_instr *c = matrix(&b, "input C", m, n, cd_type, GLSL_CMAT_USE_ACCUMULATOR);
   nir_deref_instr *d = matrix(&b, "output D", m, n, cd_type, GLSL_CMAT_USE_ACCUMULATOR);
   nir_cmat_construct(&b, &a->def, nir_imm_intN_t(&b, 1, 8));
   nir_cmat_construct(&b, &bb->def, nir_imm_intN_t(&b, 1, 8));
   nir_cmat_construct(&b, &c->def, nir_imm_int(&b, 17));
   nir_cmat_muladd(&b, &d->def, &a->def, &bb->def, &c->def,
      .saturate = saturate,
      .cmat_signed_mask = sign ? NIR_CMAT_A_SIGNED | NIR_CMAT_B_SIGNED |
                                 NIR_CMAT_C_SIGNED | NIR_CMAT_RESULT_SIGNED : 0);
   /* Preserve both D and the original C as observable values after multiply. */
   nir_store_global(&b, nir_cmat_extract(&b, 32, &c->def, nir_imm_int(&b, 0)),
                     nir_imm_int64(&b, 0x1000), .align_mul = 4);
   nir_store_global(&b, nir_cmat_extract(&b, 32, &d->def, nir_imm_int(&b, 0)),
                     nir_imm_int64(&b, 0x1004), .align_mul = 4);
   nir_validate_shader(b.shader, "matrix input");

   nir_shader *split = nir_shader_clone(NULL, b.shader);
   const struct nir_lower_coopmat_args args = {8, 8, 8};
   nir_lower_cooperative_matrix_flexible_dimensions(split, &args);
   nir_validate_shader(split, "common matrix splitter");
   unsigned muladds = 0;
   nir_foreach_function_impl(impl, split) {
      nir_foreach_block(block, impl) {
         nir_foreach_instr(instr, block) {
            if (instr->type != nir_instr_type_intrinsic)
               continue;
            nir_intrinsic_instr *intr = nir_instr_as_intrinsic(instr);
            if (intr->intrinsic != nir_intrinsic_cmat_muladd)
               continue;
            nir_variable *dst = nir_intrinsic_get_var(intr, 0);
            assert(dst && strcmp(dst->name, "input C"));
            muladds++;
         }
      }
   }
   assert(muladds == (m / 8) * (n / 8) * (k / 8));
   ralloc_free(split);

   assert(tu_nir_lower_cooperative_matrix(b.shader));
   nir_validate_shader(b.shader, "TU matrix lowering");
   unsigned saturations = 0;
   nir_foreach_function_impl(impl, b.shader) {
      nir_foreach_block(block, impl) {
         nir_foreach_instr(instr, block) {
            if (instr->type == nir_instr_type_alu) {
               nir_op op = nir_instr_as_alu(instr)->op;
               saturations += op == nir_op_iadd_sat || op == nir_op_uadd_sat;
            }
         }
      }
   }
   assert(saturations == (saturate ? (m / 8) * (n / 8) : 0));
   ralloc_free(b.shader);
}

int main(void)
{
   glsl_type_singleton_init_or_ref();
   unsigned hoists = 0, matrices = 0;
   for (unsigned depth = 0; depth <= 64; depth++) {
      hoist_fixture(depth, false);
      hoist_fixture(depth, true);
      hoists += 2;
   }
   static const unsigned shapes[][3] = {{8,8,8}, {8,8,16}, {64,32,32}, {64,64,16}};
   for (unsigned s = 0; s < ARRAY_SIZE(shapes); s++)
      for (unsigned flags = 0; flags < 8; flags++) {
         matrix_fixture(shapes[s][0], shapes[s][1], shapes[s][2],
                         flags & 1, flags & 2, flags & 4);
         matrices++;
      }
   glsl_type_singleton_decref();
   printf("PASS %u hoist DAG/rejection fixtures; %u matrix splitter/lowering fixtures with NIR validation\n", hoists, matrices);
   return 0;
}
