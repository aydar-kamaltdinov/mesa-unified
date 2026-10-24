/*
 * SPDX-License-Identifier: MIT
 *
 * Runtime mapper metadata interface recovered from GameSir Rutabaga.
 * It supplements the legacy layout with the platform's FourCC/modifier.
 */

#include "u_gralloc_internal.h"

#include <dlfcn.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "util/log.h"

/* Only the observed AIMapper v5 prefix is needed here.  Keeping it local
 * avoids requiring the newer Android platform headers at build time.  On
 * AArch64 the standard-metadata callback is at byte0x48, after version and
 * eight preceding callbacks.  No other callback is called through this view.
 */
struct runtime_aimapper {
   uint32_t version;
   void (*v5_prefix[8])(void);
   int32_t (*get_standard_metadata)(const native_handle_t *handle,
                                     int64_t type, void *data, size_t size);
};

#if UINTPTR_MAX == UINT64_MAX
_Static_assert(offsetof(struct runtime_aimapper, get_standard_metadata) == 0x48,
               "AIMapper v5 standard metadata callback offset");
#endif

struct mapper_runtime_gralloc {
   struct u_gralloc base;
   void *libui;
   void *libmapper;
   void *mapper;
   struct runtime_aimapper *aimapper;
   struct u_gralloc *fallback;
   int (*get_fourcc)(void *mapper, const native_handle_t *handle, uint32_t *value);
   int (*get_modifier)(void *mapper, const native_handle_t *handle, uint64_t *value);
};

static bool
get_standard_metadata(struct mapper_runtime_gralloc *gr,
                      const native_handle_t *handle, int64_t type,
                      void *value, size_t value_size)
{
   /* The original accepts encoded payloads of1..128 bytes.  Gralloc's
    * standard encoding is name-length:u64, name bytes, type:i64, value.
    */
   uint8_t encoded[128];
   static const char name[] =
      "android.hardware.graphics.common.StandardMetadataType";
   const size_t value_offset = sizeof(uint64_t) + sizeof(name) - 1 + sizeof(int64_t);
   int32_t size = gr->aimapper->get_standard_metadata(handle, type, NULL, 0);
   if (size <= 0 || size > (int32_t)sizeof(encoded) ||
       (size_t)size < value_offset + value_size)
      return false;

   if (gr->aimapper->get_standard_metadata(handle, type, encoded, size) != size)
      return false;

   uint64_t name_length;
   int64_t encoded_type;
   memcpy(&name_length, encoded, sizeof(name_length));
   memcpy(&encoded_type, encoded + sizeof(uint64_t) + sizeof(name) - 1,
          sizeof(encoded_type));
   if (name_length != sizeof(name) - 1 ||
       memcmp(encoded + sizeof(uint64_t), name, sizeof(name) - 1) ||
       encoded_type != type)
      return false;

   memcpy(value, encoded + value_offset, value_size);
   return true;
}

static int
mapper_runtime_get_buffer_basic_info(struct u_gralloc *gralloc,
                                     struct u_gralloc_buffer_handle *hnd,
                                     struct u_gralloc_buffer_basic_info *out)
{
   struct mapper_runtime_gralloc *gr = (struct mapper_runtime_gralloc *)gralloc;
   if (!hnd->handle)
      return -EINVAL;

   int ret = u_gralloc_get_buffer_basic_info(gr->fallback, hnd, out);
   if (ret)
      return ret;

   uint32_t fourcc;
   uint64_t modifier;
   if (gr->aimapper) {
      if (!get_standard_metadata(gr, hnd->handle, 7, &fourcc, sizeof(fourcc)) ||
          !get_standard_metadata(gr, hnd->handle, 8, &modifier, sizeof(modifier)))
         return 0;
   } else if (gr->get_fourcc(gr->mapper, hnd->handle, &fourcc) ||
              gr->get_modifier(gr->mapper, hnd->handle, &modifier)) {
      return 0;
   }

   out->drm_fourcc = fourcc;
   out->modifier = modifier;
   return 0;
}

static int
destroy(struct u_gralloc *gralloc)
{
   struct mapper_runtime_gralloc *gr = (struct mapper_runtime_gralloc *)gralloc;
   /* This private fallback was created directly while the gralloc cache
    * mutex was held.  Destroy it directly too, without re-entering the cache.
    */
   if (gr->fallback)
      gr->fallback->ops.destroy(gr->fallback);
   if (gr->libui)
      dlclose(gr->libui);
   if (gr->libmapper)
      dlclose(gr->libmapper);
   free(gr);
   return 0;
}

static void *
runtime_symbol(void *library, const char *name)
{
   void *symbol = library ? dlsym(library, name) : NULL;
   return symbol ? symbol : dlsym(RTLD_DEFAULT, name);
}

struct u_gralloc *
u_gralloc_mapper_runtime_create(void)
{
   struct mapper_runtime_gralloc *gr = calloc(1, sizeof(*gr));
   if (!gr)
      return NULL;

   typedef void *(*load_sphal_fn)(const char *name, int flags);
   load_sphal_fn load_sphal = (load_sphal_fn)dlsym(RTLD_DEFAULT,
                                                  "android_load_sphal_library");
   void *hook = NULL;
   if (!load_sphal) {
      hook = dlopen("libmain_hook.so", RTLD_NOW | RTLD_NOLOAD);
      if (!hook)
         hook = dlopen("libmain_hook.so", RTLD_NOW);
      if (hook)
         load_sphal = (load_sphal_fn)dlsym(hook, "android_load_sphal_library");
   }
   if (load_sphal)
      gr->libmapper = load_sphal("mapper.qti.so", RTLD_NOW);
   if (hook)
      dlclose(hook);

   if (gr->libmapper) {
      typedef int32_t (*load_mapper_fn)(struct runtime_aimapper **mapper);
      load_mapper_fn load_mapper = (load_mapper_fn)dlsym(gr->libmapper,
                                                         "AIMapper_loadIMapper");
      if (!load_mapper || load_mapper(&gr->aimapper) || !gr->aimapper ||
          gr->aimapper->version < 5 || !gr->aimapper->get_standard_metadata)
         gr->aimapper = NULL;
   }

   if (!gr->aimapper) {
      gr->libui = dlopen("libui.so", RTLD_NOW);
      typedef void *(*get_mapper_fn)(void);
      get_mapper_fn get_mapper = (get_mapper_fn)runtime_symbol(gr->libui,
         "_ZN7android9SingletonINS_19GraphicBufferMapperEE11getInstanceEv");
      gr->get_fourcc = (int (*)(void *, const native_handle_t *, uint32_t *))
         runtime_symbol(gr->libui,
            "_ZN7android19GraphicBufferMapper20getPixelFormatFourCCEPK13native_handlePj");
      gr->get_modifier = (int (*)(void *, const native_handle_t *, uint64_t *))
         runtime_symbol(gr->libui,
            "_ZN7android19GraphicBufferMapper22getPixelFormatModifierEPK13native_handlePm");
      if (get_mapper && gr->get_fourcc && gr->get_modifier)
         gr->mapper = get_mapper();
   }

   if (!gr->aimapper && !gr->mapper)
      goto fail;

   gr->fallback = u_gralloc_fallback_create();
   if (!gr->fallback)
      goto fail;

   gr->base.ops.get_buffer_basic_info = mapper_runtime_get_buffer_basic_info;
   gr->base.ops.destroy = destroy;
   mesa_logi("Using Android %s runtime metadata API",
             gr->aimapper ? "AIMapper v5" : "GraphicBufferMapper");
   return &gr->base;

fail:
   destroy(&gr->base);
   return NULL;
}
