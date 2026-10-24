/* SPDX-License-Identifier: MIT */
#ifndef TU_INDIRECT_COPY_FORMAT_H
#define TU_INDIRECT_COPY_FORMAT_H

#include "util/format/u_format.h"
#include "vulkan/vulkan_core.h"

/* Description consumed by the indirect memory-to-image fragment shader.
 * The generic channel word contains shift, width and conversion kind.
 */
enum tu_indirect_copy_kind {
   TU_INDIRECT_COPY_UNORM8,
   TU_INDIRECT_COPY_UNORM16,
   TU_INDIRECT_COPY_FLOAT32,
   TU_INDIRECT_COPY_UINT8,
   TU_INDIRECT_COPY_UINT32,
   TU_INDIRECT_COPY_GENERIC,
   TU_INDIRECT_COPY_11F11F10F,
};

struct tu_indirect_copy_format {
   enum tu_indirect_copy_kind kind;
   unsigned components;
   bool integer;
   bool srgb;
   bool stencil_in_z24;
   uint32_t channel[4];
};

/* GameSir's format-feature whitelist, independently checked against the
 * original jump table. Other formats may have valid metadata but are not
 * supported indirect-copy image destinations.
 */
static inline bool
tu_indirect_copy_image_format_supported(VkFormat format)
{
   switch (format) {
   case VK_FORMAT_R8_UNORM:
   case VK_FORMAT_R8_SNORM:
   case VK_FORMAT_R8_UINT:
   case VK_FORMAT_R8_SINT:
   case VK_FORMAT_R8_SRGB:
   case VK_FORMAT_R8G8_UNORM:
   case VK_FORMAT_R8G8_SNORM:
   case VK_FORMAT_R8G8_UINT:
   case VK_FORMAT_R8G8_SINT:
   case VK_FORMAT_R8G8_SRGB:
   case VK_FORMAT_R8G8B8A8_UNORM:
   case VK_FORMAT_R8G8B8A8_SNORM:
   case VK_FORMAT_R8G8B8A8_UINT:
   case VK_FORMAT_R8G8B8A8_SINT:
   case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
   case VK_FORMAT_A2B10G10R10_UINT_PACK32:
   case VK_FORMAT_R16_UNORM:
   case VK_FORMAT_R16_SNORM:
   case VK_FORMAT_R16_UINT:
   case VK_FORMAT_R16_SINT:
   case VK_FORMAT_R16_SFLOAT:
   case VK_FORMAT_R16G16_UNORM:
   case VK_FORMAT_R16G16_SNORM:
   case VK_FORMAT_R16G16_UINT:
   case VK_FORMAT_R16G16_SINT:
   case VK_FORMAT_R16G16_SFLOAT:
   case VK_FORMAT_R16G16B16A16_UNORM:
   case VK_FORMAT_R16G16B16A16_SNORM:
   case VK_FORMAT_R16G16B16A16_UINT:
   case VK_FORMAT_R16G16B16A16_SINT:
   case VK_FORMAT_R16G16B16A16_SFLOAT:
   case VK_FORMAT_R32_UINT:
   case VK_FORMAT_R32_SINT:
   case VK_FORMAT_R32_SFLOAT:
   case VK_FORMAT_R32G32_UINT:
   case VK_FORMAT_R32G32_SINT:
   case VK_FORMAT_R32G32_SFLOAT:
   case VK_FORMAT_R32G32B32A32_UINT:
   case VK_FORMAT_R32G32B32A32_SINT:
   case VK_FORMAT_R32G32B32A32_SFLOAT:
   case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
   case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
   case VK_FORMAT_D16_UNORM:
   case VK_FORMAT_X8_D24_UNORM_PACK32:
   case VK_FORMAT_D32_SFLOAT:
   case VK_FORMAT_D24_UNORM_S8_UINT:
   case VK_FORMAT_D32_SFLOAT_S8_UINT:
      return true;
   default:
      return false;
   }
}

static inline struct tu_indirect_copy_format
tu_get_indirect_copy_format(enum pipe_format dst, enum pipe_format src,
                             VkImageAspectFlags aspect)
{
   struct tu_indirect_copy_format result = {.kind = TU_INDIRECT_COPY_UNORM8};
   if (src == PIPE_FORMAT_Z24_UNORM_S8_UINT ||
       src == PIPE_FORMAT_Z24_UNORM_S8_UINT_AS_R8G8B8A8 ||
       src == PIPE_FORMAT_Z24X8_UNORM) {
      result.components = aspect == VK_IMAGE_ASPECT_STENCIL_BIT ? 1 : 4;
      result.stencil_in_z24 = aspect == VK_IMAGE_ASPECT_STENCIL_BIT;
      return result;
   }

   const struct util_format_description *desc = util_format_description(dst);
   switch (dst) {
   case PIPE_FORMAT_R8_UNORM:
   case PIPE_FORMAT_R8G8_UNORM:
   case PIPE_FORMAT_R8G8B8A8_UNORM:
   case PIPE_FORMAT_R8_SRGB:
   case PIPE_FORMAT_R8G8_SRGB:
   case PIPE_FORMAT_R8G8B8A8_SRGB:
      result.kind = TU_INDIRECT_COPY_UNORM8;
      result.components = desc->nr_channels;
      result.srgb = desc->colorspace == UTIL_FORMAT_COLORSPACE_SRGB;
      return result;
   case PIPE_FORMAT_Z16_UNORM:
      result.kind = TU_INDIRECT_COPY_UNORM16;
      result.components = 1;
      return result;
   case PIPE_FORMAT_Z32_FLOAT:
      result.kind = TU_INDIRECT_COPY_FLOAT32;
      result.components = 1;
      return result;
   case PIPE_FORMAT_S8_UINT:
      result.kind = TU_INDIRECT_COPY_UINT8;
      result.components = 1;
      result.integer = true;
      return result;
   case PIPE_FORMAT_R32_UINT:
      result.kind = TU_INDIRECT_COPY_UINT32;
      result.components = 1;
      result.integer = true;
      return result;
   case PIPE_FORMAT_R11G11B10_FLOAT:
      result.kind = TU_INDIRECT_COPY_11F11F10F;
      result.components = 3;
      return result;
   default:
      break;
   }

   result.kind = TU_INDIRECT_COPY_GENERIC;
   result.components = desc->nr_channels;
   result.integer = util_format_is_pure_integer(dst);
   for (unsigned i = 0; i < 4; i++) {
      unsigned swizzle = desc->swizzle[i];
      if (swizzle == PIPE_SWIZZLE_0 || swizzle == PIPE_SWIZZLE_NONE)
         continue;
      if (swizzle == PIPE_SWIZZLE_1) {
         result.channel[i] = 1u << 16;
         continue;
      }
      const struct util_format_channel_description *c = &desc->channel[swizzle];
      unsigned conversion;
      if (c->type == UTIL_FORMAT_TYPE_FLOAT)
         conversion = c->size == 16 ? 6 : 7;
      else if (c->normalized)
         conversion = c->type == UTIL_FORMAT_TYPE_SIGNED ? 3 : 2;
      else
         conversion = c->type == UTIL_FORMAT_TYPE_SIGNED ? 5 : 4;
      result.channel[i] = c->shift | (c->size << 8) | (conversion << 16);
   }
   return result;
}

#endif
