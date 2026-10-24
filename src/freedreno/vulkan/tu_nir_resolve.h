/* SPDX-License-Identifier: MIT */
#ifndef TU_NIR_RESOLVE_H
#define TU_NIR_RESOLVE_H

#include "compiler/nir/nir.h"
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Extended sysmem resolve builder. Not the GMEM color-average prototype.
 * Only the five combinations used by GameSir are supported; others return NULL.
 */
nir_shader *tu_nir_resolve_fs(VkResolveModeFlagBits mode, bool integer,
                            const nir_shader_compiler_options *options);

/* Byte-encoded D24 + stencil: reduce depth and stencil independently, then
 * return normalized RGBA bytes. Requires the matching image-view mapping.
 */
nir_shader *tu_nir_resolve_d24_min_fs(const nir_shader_compiler_options *options);

/* MAX/AVERAGE operate on reconstructed depth, not individual encoded bytes.
 * AVERAGE preserves sample0 stencil; stencil averaging is not supported.
 * Only MIN/MAX/AVERAGE are accepted. MAX/AVERAGE are source corrections to
 * the recovered generic-byte path, pending GPU validation.
 */
nir_shader *tu_nir_resolve_d24_fs(VkResolveModeFlagBits mode,
                                const nir_shader_compiler_options *options);

#ifdef __cplusplus
}
#endif

#endif
