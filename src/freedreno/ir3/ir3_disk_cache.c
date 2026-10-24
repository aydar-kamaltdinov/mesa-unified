/*
 * Copyright © 2020 Google, Inc.
 * SPDX-License-Identifier: MIT
 */

#include "nir_serialize.h"

#include "ir3_compiler.h"
#include "ir3_nir.h"

#define debug 0

/*
 * Shader disk-cache implementation.
 *
 * Note that at least in the EGL_ANDROID_blob_cache, we should never
 * rely on inter-dependencies between different cache entries:
 *
 *    No guarantees are made as to whether a given key/value pair is present in
 *    the cache after the set call.  If a different value has been associated
 *    with the given key in the past then it is undefined which value, if any,
 *    is associated with the key after the set call.  Note that while there are
 *    no guarantees, the cache implementation should attempt to cache the most
 *    recently set value for a given key.
 *
 * for this reason, because binning pass variants share const_state with
 * their draw-pass counterpart, both variants are serialized together.
 */

void
ir3_disk_cache_init(struct ir3_compiler *compiler)
{
   if (ir3_shader_debug & IR3_DBG_NOCACHE)
      return;

   const char *renderer = fd_dev_name(compiler->dev_id);
   const struct build_id_note *note =
      build_id_find_nhdr_for_addr(ir3_disk_cache_init);
   unsigned build_id_len = build_id_length(note);
   assert(note && build_id_len == BUILD_ID_EXPECTED_HASH_LENGTH); /* sha1 */

   const uint8_t *id_sha1 = build_id_data(note);
   assert(id_sha1);

   blake3_hasher ctx;
   uint8_t blake3[BLAKE3_KEY_LEN];
   _mesa_blake3_init(&ctx);
   _mesa_blake3_update(&ctx, id_sha1, build_id_len);
   _mesa_blake3_update(&ctx, &compiler->options.uche_trap_base,
                     sizeof(compiler->options.uche_trap_base));
   _mesa_blake3_final(&ctx, blake3);

   char timestamp[BLAKE3_HEX_LEN];
   _mesa_blake3_format(timestamp, blake3);

   uint64_t driver_flags = ir3_shader_debug_hash_key();
   compiler->disk_cache = disk_cache_create(renderer, timestamp, driver_flags);
}

void
ir3_disk_cache_init_shader_key(struct ir3_compiler *compiler,
                               struct ir3_shader *shader)
{
   if (!compiler->disk_cache && !ir3_shader_bisect_need_shader_key())
      return;

   blake3_hasher ctx;

   _mesa_blake3_init(&ctx);

   /* Serialize the NIR to a binary blob that we can hash for the disk
    * cache.  Drop unnecessary information (like variable names)
    * so the serialized NIR is smaller, and also to let us detect more
    * isomorphic shaders when hashing, increasing cache hits.
    */
   struct blob blob;
   blob_init(&blob);
   /* Includes A8 flat-bypass encoding and mesh input-record allocation. */
   const uint32_t variant_schema = 0x4753381c;
   _mesa_blake3_update(&ctx, &variant_schema, sizeof(variant_schema));
   nir_serialize(&blob, shader->nir, true);
   _mesa_blake3_update(&ctx, blob.data, blob.size);
   blob_finish(&blob);

   _mesa_blake3_update(&ctx, &shader->options.api_wavesize,
                     sizeof(shader->options.api_wavesize));
   _mesa_blake3_update(&ctx, &shader->options.real_wavesize,
                     sizeof(shader->options.real_wavesize));
   _mesa_blake3_update(&ctx, &shader->options.nir_options,
                     sizeof(shader->options.nir_options));
   /* This disables alias.rt for outputs whose render-target slots are
    * remapped by the driver. It affects backend code without changing NIR.
    */
   _mesa_blake3_update(&ctx, &shader->options.fragdata_dynamic_remap,
                     sizeof(shader->options.fragdata_dynamic_remap));
   _mesa_blake3_update(&ctx, &shader->options.mesh_multiview,
                     sizeof(shader->options.mesh_multiview));
   /* Sample-center lowering runs after the disk-cache lookup. */
   const bool sample_center = ir3_gamesir_sample_center_enabled();
   _mesa_blake3_update(&ctx, &sample_center, sizeof(sample_center));

   /* Hash named fields, avoiding padding in the mesh interface record. */
   _mesa_blake3_update(&ctx, &shader->options.mesh_fs_inputs.enabled,
                     sizeof(shader->options.mesh_fs_inputs.enabled));
   _mesa_blake3_update(&ctx, &shader->options.mesh_fs_inputs.primitive_base,
                     sizeof(shader->options.mesh_fs_inputs.primitive_base));
   _mesa_blake3_update(&ctx, &shader->options.mesh_fs_inputs.layer_viewport_base,
                     sizeof(shader->options.mesh_fs_inputs.layer_viewport_base));
   _mesa_blake3_update(&ctx, shader->options.mesh_fs_inputs.location_plus_one,
                     sizeof(shader->options.mesh_fs_inputs.location_plus_one));

   /* Note that on some gens stream-out is lowered in ir3 to stg.  For later
    * gens we maybe don't need to include stream-out in the cache key.
    */
   _mesa_blake3_update(&ctx, &shader->stream_output,
                     sizeof(shader->stream_output));

   _mesa_blake3_final(&ctx, shader->cache_key);
}

static void
compute_variant_key(struct ir3_shader *shader, struct ir3_shader_variant *v,
                    cache_key cache_key)
{
   struct blob blob;
   blob_init(&blob);

   blob_write_bytes(&blob, &shader->cache_key, sizeof(shader->cache_key));
   blob_write_bytes(&blob, &v->key, sizeof(v->key));
   blob_write_uint8(&blob, v->binning_pass);

   disk_cache_compute_key(shader->compiler->disk_cache, blob.data, blob.size,
                          cache_key);

   blob_finish(&blob);
}

static bool
retrieve_variant(struct blob_reader *blob, struct ir3_shader_variant *v)
{
   /* Read and bound the complete payload before allocating or publishing any
    * of it. A cache miss must leave the compiler's original variant intact.
    */
   struct ir3_shader_variant cached = *v;
   blob_copy_bytes(blob, VARIANT_CACHE_PTR(&cached), VARIANT_CACHE_SIZE);
   const void *binary = blob_read_bytes(blob, cached.info.size);
   const void *const_state = v->binning_pass ? NULL :
      blob_read_bytes(blob, sizeof(*v->const_state));

   bool has_immediates = !v->compiler->info->props.load_shader_consts_via_preamble;
   uint32_t imm_count = 0;
   size_t immeds_size = 0;
   const void *immediates = NULL;
   if (has_immediates) {
      imm_count = blob_read_uint32(blob);
      if (blob->overrun || blob->current > blob->end ||
          imm_count > (size_t)(blob->end - blob->current) /
                         sizeof(v->imm_state.values[0])) {
         blob->overrun = true;
         return false;
      }
      immeds_size = (size_t) imm_count * sizeof(v->imm_state.values[0]);
      immediates = blob_read_bytes(blob, immeds_size);
   }
   if (blob->overrun)
      return false;

   uint32_t *bin = rzalloc_size(v, cached.info.size);
   if (!bin)
      return false;
   uint32_t *imm_values = NULL;
   if (has_immediates) {
      imm_values = ralloc_size(v, immeds_size);
      if (!imm_values) {
         ralloc_free(bin);
         return false;
      }
   }

   if (cached.info.size)
      memcpy(bin, binary, cached.info.size);
   if (immeds_size)
      memcpy(imm_values, immediates, immeds_size);
   if (!v->binning_pass)
      memcpy(v->const_state, const_state, sizeof(*v->const_state));
   memcpy(VARIANT_CACHE_PTR(v), VARIANT_CACHE_PTR(&cached), VARIANT_CACHE_SIZE);
   v->bin = bin;
   if (has_immediates) {
      v->imm_state.size = imm_count;
      v->imm_state.count = imm_count;
      v->imm_state.values = imm_values;
   }
   return true;
}

static void
store_variant(struct blob *blob, const struct ir3_shader_variant *v)
{
   blob_write_bytes(blob, VARIANT_CACHE_PTR(v), VARIANT_CACHE_SIZE);

   /*
    * pointers need special handling:
    */

   blob_write_bytes(blob, v->bin, v->info.size);

   /* No saving constant_data, it's already baked into bin at this point. */

   if (!v->binning_pass) {
      blob_write_bytes(blob, v->const_state, sizeof(*v->const_state));
   }

   /* When load_shader_consts_via_preamble, immediates are loaded in the
    * preamble and hence part of bin.
    */
   if (!v->compiler->info->props.load_shader_consts_via_preamble) {
      blob_write_uint32(blob, v->imm_state.size);
      uint32_t immeds_sz = v->imm_state.size * sizeof(v->imm_state.values[0]);
      blob_write_bytes(blob, v->imm_state.values, immeds_sz);
   }
}

struct ir3_shader_variant *
ir3_retrieve_variant(struct blob_reader *blob, struct ir3_compiler *compiler,
                     void *mem_ctx)
{
   struct ir3_shader_variant *v = rzalloc_size(mem_ctx, sizeof(*v));

   if (!v)
      return NULL;

   v->id = 0;
   v->compiler = compiler;
   v->binning_pass = false;
   v->nonbinning = NULL;
   v->binning = NULL;
   blob_copy_bytes(blob, &v->key, sizeof(v->key));
   v->type = blob_read_uint32(blob);
   v->mergedregs = blob_read_uint32(blob);
   /* TU needs the immutable mesh producer map when rebuilding VPC state
    * after a pipeline-cache hit. shader_options precedes VARIANT_CACHE_START
    * and is otherwise initialized only when compiling a variant.
    */
   blob_copy_bytes(blob, &v->shader_options.mesh_fs_inputs,
                   sizeof(v->shader_options.mesh_fs_inputs));
   v->shader_options.mesh_multiview = blob_read_uint8(blob);
   if (blob->overrun)
      goto fail;
   v->const_state = rzalloc_size(v, sizeof(*v->const_state));
   if (!v->const_state || !retrieve_variant(blob, v))
      goto fail;

   if (v->type == MESA_SHADER_VERTEX && ir3_has_binning_vs(&v->key)) {
      v->binning = rzalloc_size(v, sizeof(*v->binning));
      if (!v->binning)
         goto fail;
      v->binning->id = 0;
      v->binning->compiler = compiler;
      v->binning->binning_pass = true;
      v->binning->nonbinning = v;
      v->binning->key = v->key;
      v->binning->type = MESA_SHADER_VERTEX;
      v->binning->mergedregs = v->mergedregs;
      v->binning->const_state = v->const_state;

      if (!retrieve_variant(blob, v->binning))
         goto fail;
   }

   return v;

fail:
   ralloc_free(v);
   return NULL;
}

void
ir3_store_variant(struct blob *blob, const struct ir3_shader_variant *v)
{
   blob_write_bytes(blob, &v->key, sizeof(v->key));
   blob_write_uint32(blob, v->type);
   blob_write_uint32(blob, v->mergedregs);
   blob_write_bytes(blob, &v->shader_options.mesh_fs_inputs,
                    sizeof(v->shader_options.mesh_fs_inputs));
   blob_write_uint8(blob, v->shader_options.mesh_multiview);

   store_variant(blob, v);

   if (v->type == MESA_SHADER_VERTEX && ir3_has_binning_vs(&v->key)) {
      store_variant(blob, v->binning);
   }
}

bool
ir3_disk_cache_retrieve(struct ir3_shader *shader,
                        struct ir3_shader_variant *v)
{
   if (!shader->compiler->disk_cache)
      return false;

   cache_key cache_key;

   compute_variant_key(shader, v, cache_key);

   if (debug) {
      char blake3[BLAKE3_HEX_LEN];
      _mesa_blake3_format(blake3, cache_key);
      fprintf(stderr, "[mesa disk cache] retrieving variant %s: ", blake3);
   }

   size_t size;
   void *buffer = disk_cache_get(shader->compiler->disk_cache, cache_key, &size);

   if (debug)
      fprintf(stderr, "%s\n", buffer ? "found" : "missing");

   if (!buffer)
      return false;

   struct blob_reader blob;
   blob_reader_init(&blob, buffer, size);

   /* The binning variant shares const_state with the draw variant. If either
    * record is incomplete, restore both before falling back to compilation.
    */
   struct ir3_shader_variant saved = *v;
   struct ir3_shader_variant saved_binning;
   if (v->binning)
      saved_binning = *v->binning;
   struct ir3_const_state saved_const_state = *v->const_state;

   bool success = retrieve_variant(&blob, v);
   if (success && v->binning)
      success = retrieve_variant(&blob, v->binning);
   success &= !blob.overrun && blob.current == blob.end;

   if (!success) {
      if (v->bin != saved.bin)
         ralloc_free(v->bin);
      if (v->imm_state.values != saved.imm_state.values)
         ralloc_free(v->imm_state.values);
      if (v->binning) {
         if (v->binning->bin != saved_binning.bin)
            ralloc_free(v->binning->bin);
         if (v->binning->imm_state.values != saved_binning.imm_state.values)
            ralloc_free(v->binning->imm_state.values);
         *v->binning = saved_binning;
      }
      *v = saved;
      *v->const_state = saved_const_state;
   }

   free(buffer);

   return success;
}

void
ir3_disk_cache_store(struct ir3_shader *shader,
                     struct ir3_shader_variant *v)
{
   if (!shader->compiler->disk_cache)
      return;

   cache_key cache_key;

   compute_variant_key(shader, v, cache_key);

   if (debug) {
      char blake3[BLAKE3_HEX_LEN];
      _mesa_blake3_format(blake3, cache_key);
      fprintf(stderr, "[mesa disk cache] storing variant %s\n", blake3);
   }

   struct blob blob;
   blob_init(&blob);

   store_variant(&blob, v);

   if (v->binning)
      store_variant(&blob, v->binning);

   disk_cache_put(shader->compiler->disk_cache, cache_key, blob.data, blob.size, NULL);
   blob_finish(&blob);
}
