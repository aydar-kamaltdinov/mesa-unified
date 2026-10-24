/* SPDX-License-Identifier: MIT */
#ifndef TU_NIR_INDIRECT_COPY_H
#define TU_NIR_INDIRECT_COPY_H

#include "compiler/nir/nir.h"

#ifdef __cplusplus
extern "C" {
#endif

nir_shader *
tu_nir_copy_memory_to_image_indirect_vs(const nir_shader_compiler_options *options);

nir_shader *
tu_nir_copy_memory_to_image_indirect_fs(bool integer,
                                        const nir_shader_compiler_options *options);

#ifdef __cplusplus
}
#endif

#endif
