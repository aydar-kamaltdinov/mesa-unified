/* SPDX-License-Identifier: MIT */
#ifndef TU_NIR_COOPERATIVE_MATRIX_H
#define TU_NIR_COOPERATIVE_MATRIX_H
#include "compiler/nir/nir.h"
#ifdef __cplusplus
extern "C" {
#endif
bool tu_nir_lower_cooperative_matrix(nir_shader *shader);
#ifdef __cplusplus
}
#endif
#endif
