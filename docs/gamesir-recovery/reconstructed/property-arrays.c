/* SPDX-License-Identifier: MIT
 * Original GameSir caller-array property contracts, analysis only.
 * Six array routes in four public structures. Existing Mesa's property
 * generator already implements these rules; do not link a second backend.
 * Numeric physical offsets below describe ONLY the original v3 ABI.
 * Native pointer width64; ordinary valid disjoint buffers, no wrapping sizes.
 * memcpy/struct assignment is represented as byte copying, including padding.
 * This does not preserve SIMD/fault/race timing or validate pointer lifetime.
 * No driver build, C execution or integrated API/GPU validation performed.
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct gs_native_property_array {
   const char *structure, *pointer, *count;
   uint16_t api_count_offset, api_pointer_offset;
   uint16_t physical_count_offset, physical_pointer_offset, element_bytes;
};

const struct gs_native_property_array gs_native_property_arrays[] = {
   {"VkPhysicalDeviceHostImageCopyProperties", "pCopySrcLayouts", "copySrcLayoutCount", 16, 24, 0xec4, 0xec8, 4},
   {"VkPhysicalDeviceHostImageCopyProperties", "pCopyDstLayouts", "copyDstLayoutCount", 32, 40, 0xed0, 0xed8, 4},
   {"VkPhysicalDeviceVulkan14Properties", "pCopySrcLayouts", "copySrcLayoutCount", 92, 96, 0xec4, 0xec8, 4},
   {"VkPhysicalDeviceVulkan14Properties", "pCopyDstLayouts", "copyDstLayoutCount", 104, 112, 0xed0, 0xed8, 4},
   {"VkPhysicalDeviceLayeredApiPropertiesListKHR", "pLayeredApis", "layeredApiCount", 16, 24, 0xadc, 0xae0, 288},
   {"VkPhysicalDeviceGpaPropertiesAMD", "pPerfBlocks", "perfBlockCount", 36, 40, 0xf00, 0xf08, 28},
};

/* Called for each independent array. The enclosing getter also copies scalar
 * properties and follows pNext. Null dst returns total without reading source.
 * Non-null dst/capacity0 performs no source read or destination element write.
 * capacity and total are unsigned32, including values above INT32_MAX.
 */
void
gs_native_copy_property_array(uint32_t *capacity, void *dst,
                              uint32_t total, const void *src, size_t stride)
{
   if (!dst) {
      *capacity = total;
      return;
   }
   uint32_t count = *capacity < total ? *capacity : total;
   for (uint32_t i = 0; i < count; i++)
      memcpy((uint8_t *)dst + (size_t)i * stride,
             (const uint8_t *)src + (size_t)i * stride, stride);
   *capacity = count;
}
