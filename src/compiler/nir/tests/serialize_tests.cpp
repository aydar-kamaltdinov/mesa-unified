/*
 * Copyright © 2019 Red Hat, Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <gtest/gtest.h>

#include "nir.h"
#include "nir_builder.h"
#include "nir_serialize.h"

namespace {

class nir_serialize_test : public ::testing::TestWithParam<int> {
protected:
   nir_serialize_test();
   ~nir_serialize_test();

   void serialize();
   nir_alu_instr *get_last_alu(nir_shader *);
   void ASSERT_SWIZZLE_EQ(nir_alu_instr *, nir_alu_instr *, unsigned count, unsigned src);

   nir_builder *b, _b;
   nir_shader *dup;
   const nir_shader_compiler_options options;
};

nir_serialize_test::nir_serialize_test()
:  dup(NULL), options()
{
   glsl_type_singleton_init_or_ref();

   _b = nir_builder_init_simple_shader(MESA_SHADER_COMPUTE, &options, "serialize test");
   b = &_b;
}

nir_serialize_test::~nir_serialize_test()
{
   if (HasFailure()) {
      printf("\nShader from the failed test\n\n");
      printf("original Shader:\n");
      nir_print_shader(b->shader, stdout);
      printf("serialized Shader:\n");
      nir_print_shader(dup, stdout);
   }

   ralloc_free(b->shader);

   glsl_type_singleton_decref();
}

void
nir_serialize_test::serialize() {
   struct blob blob;
   struct blob_reader reader;

   blob_init(&blob);

   nir_serialize(&blob, b->shader, false);
   blob_reader_init(&reader, blob.data, blob.size);
   nir_shader *cloned = nir_deserialize(b->shader, &options, &reader);
   blob_finish(&blob);

   dup = cloned;

   nir_validate_shader(b->shader, "original");
   nir_validate_shader(b->shader, "cloned");
}

nir_alu_instr *
nir_serialize_test::get_last_alu(nir_shader *nir)
{
   nir_function_impl *impl = nir_shader_get_entrypoint(nir);
   return nir_instr_as_alu(nir_block_last_instr(nir_impl_last_block(impl)));
}

void
nir_serialize_test::ASSERT_SWIZZLE_EQ(nir_alu_instr *a, nir_alu_instr *b, unsigned c, unsigned s)
{
   ASSERT_EQ(memcmp(a->src[s].swizzle, b->src[s].swizzle, c), 0);
}

class nir_serialize_all_test : public nir_serialize_test {};
class nir_serialize_all_but_one_test : public nir_serialize_test {};

class nir_serialize_spec_test : public ::testing::TestWithParam<bool> {
protected:
   nir_serialize_spec_test()
   {
      glsl_type_singleton_init_or_ref();
      b = nir_builder_init_simple_shader(MESA_SHADER_VERTEX, &options, "spec test");
      nir_imm_int(&b, 42);
      b.shader->info.spec = ralloc_strdup(b.shader, "spec[0] = 0x0000002a\n");
   }

   ~nir_serialize_spec_test()
   {
      ralloc_free(b.shader);
      glsl_type_singleton_decref();
   }

   const nir_shader_compiler_options options = {};
   nir_builder b;
};

} // namespace

INSTANTIATE_TEST_SUITE_P(nir_serialize_spec, nir_serialize_spec_test,
                        ::testing::Bool());

TEST_P(nir_serialize_spec_test, clone_and_roundtrip_are_pointer_independent)
{
   const bool strip = GetParam();
   const char *original_spec = b.shader->info.spec;
   nir_shader *clone = nir_shader_clone(b.shader, b.shader);
   ASSERT_NE(original_spec, clone->info.spec);
   ASSERT_STREQ(original_spec, clone->info.spec);

   struct blob original, copied, restored;
   blob_init(&original);
   blob_init(&copied);
   blob_init(&restored);
   nir_serialize(&original, b.shader, strip);
   nir_serialize(&copied, clone, strip);
   EXPECT_EQ(original.size, copied.size);
   if (original.size == copied.size)
      EXPECT_EQ(memcmp(original.data, copied.data, original.size), 0);
   EXPECT_EQ(b.shader->info.spec, original_spec);

   struct blob_reader reader;
   blob_reader_init(&reader, original.data, original.size);
   nir_shader *roundtrip = nir_deserialize(b.shader, &options, &reader);
   EXPECT_FALSE(reader.overrun);
   EXPECT_EQ(reader.current, reader.end);
   if (strip)
      EXPECT_EQ(roundtrip->info.spec, nullptr);
   else
      EXPECT_STREQ(roundtrip->info.spec, original_spec);
   nir_serialize(&restored, roundtrip, strip);
   EXPECT_EQ(original.size, restored.size);
   if (original.size == restored.size)
      EXPECT_EQ(memcmp(original.data, restored.data, original.size), 0);

   blob_finish(&original);
   blob_finish(&copied);
   blob_finish(&restored);
}

TEST_P(nir_serialize_spec_test, description_is_stripped_but_shader_code_is_not)
{
   const bool strip = GetParam();
   nir_shader *clone = nir_shader_clone(b.shader, b.shader);
   clone->info.spec = ralloc_strdup(clone, "another debug description");

   struct blob original, copied, modified;
   blob_init(&original);
   blob_init(&copied);
   blob_init(&modified);
   nir_serialize(&original, b.shader, strip);
   nir_serialize(&copied, clone, strip);
   const bool same = original.size == copied.size &&
      memcmp(original.data, copied.data, original.size) == 0;
   EXPECT_EQ(same, strip);

   nir_imm_int(&b, 43);
   nir_serialize(&modified, b.shader, strip);
   EXPECT_FALSE(original.size == modified.size &&
                memcmp(original.data, modified.data, original.size) == 0);

   blob_finish(&original);
   blob_finish(&copied);
   blob_finish(&modified);
}

#if NIR_MAX_VEC_COMPONENTS == 16
#define COMPONENTS 2, 3, 4, 8, 16
#else
#define COMPONENTS 2, 3, 4
#endif


INSTANTIATE_TEST_SUITE_P(
   nir_serialize_all_test,
   nir_serialize_all_test,
   ::testing::Values(1, COMPONENTS)
);

INSTANTIATE_TEST_SUITE_P(
   nir_serialize_all_but_one_test,
   nir_serialize_all_but_one_test,
   ::testing::Values(COMPONENTS)
);

TEST_P(nir_serialize_all_test, alu_single_value_src_swizzle)
{
   nir_def *zero = nir_imm_zero(b, GetParam(), 32);
   nir_def *fmax = nir_fmax(b, zero, zero);

   nir_alu_instr *fmax_alu = nir_def_as_alu(fmax);

   memset(fmax_alu->src[0].swizzle, GetParam() - 1, NIR_MAX_VEC_COMPONENTS);
   memset(fmax_alu->src[1].swizzle, GetParam() - 1, NIR_MAX_VEC_COMPONENTS);

   serialize();

   nir_alu_instr *fmax_alu_dup = get_last_alu(dup);

   ASSERT_SWIZZLE_EQ(fmax_alu, fmax_alu_dup, GetParam(), 0);
   ASSERT_SWIZZLE_EQ(fmax_alu, fmax_alu_dup, GetParam(), 1);
}

TEST_P(nir_serialize_all_test, alu_vec)
{
   nir_def *undef = nir_undef(b, GetParam(), 32);
   nir_def *undefs[] = {
      undef, undef, undef, undef,
      undef, undef, undef, undef,
      undef, undef, undef, undef,
      undef, undef, undef, undef,
   };

   nir_def *vec = nir_vec(b, undefs, GetParam());
   nir_alu_instr *vec_alu = nir_def_as_alu(vec);
   for (int i = 0; i < GetParam(); i++)
      vec_alu->src[i].swizzle[0] = (GetParam() - 1) - i;

   serialize();

   nir_alu_instr *vec_alu_dup = get_last_alu(dup);

   ASSERT_SWIZZLE_EQ(vec_alu, vec_alu_dup, 1, 0);
}

TEST_P(nir_serialize_all_test, alu_two_components_full_swizzle)
{
   nir_def *undef = nir_undef(b, 2, 32);
   nir_def *fma = nir_ffma_weak(b, undef, undef, undef);
   nir_alu_instr *fma_alu = nir_def_as_alu(fma);

   fma->num_components = GetParam();

   memset(fma_alu->src[0].swizzle, 1, GetParam());
   memset(fma_alu->src[1].swizzle, 1, GetParam());
   memset(fma_alu->src[2].swizzle, 1, GetParam());

   serialize();

   nir_alu_instr *fma_alu_dup = get_last_alu(dup);

   ASSERT_SWIZZLE_EQ(fma_alu, fma_alu_dup, GetParam(), 0);
   ASSERT_SWIZZLE_EQ(fma_alu, fma_alu_dup, GetParam(), 1);
   ASSERT_SWIZZLE_EQ(fma_alu, fma_alu_dup, GetParam(), 2);
}

TEST_P(nir_serialize_all_but_one_test, single_channel)
{
   nir_def *zero = nir_undef(b, GetParam(), 32);
   nir_def *vec = nir_channel(b, zero, GetParam() - 1);
   nir_alu_instr *vec_alu = nir_def_as_alu(vec);

   serialize();

   nir_alu_instr *vec_alu_dup = get_last_alu(dup);

   ASSERT_SWIZZLE_EQ(vec_alu, vec_alu_dup, 1, 0);
}
