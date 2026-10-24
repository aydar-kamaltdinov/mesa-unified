/* SPDX-License-Identifier: MIT */
#ifndef TU_NIR_MESH_H
#define TU_NIR_MESH_H

#include "nir/nir.h"
#include "tu_mesh_layout.h"

bool tu_nir_lower_mesh_fragment_view_index(nir_shader *nir);
bool tu_nir_prepare_task(nir_shader *nir);
bool tu_nir_prepare_mesh(nir_shader *nir);
/* Explicit-I/O mesh/FS linkage. Rebuilds bases before reserving the primitive
 * header slot, so linking a retained snapshot does not accumulate offsets.
 * This deliberately does not run ordinary varying compaction.
 */
void tu_nir_link_mesh_fragment(nir_shader *mesh, nir_shader *fragment);
bool tu_nir_lower_task(nir_shader *nir);
bool tu_nir_lower_mesh_inputs(nir_shader *nir);

/* Recovered mesh I/O layout subset, in bytes. Vertex/primitive views map to
 * original layout offsets 0x0c/0x14, strides 0x10/0x18, capacities 0x34/0x38
 * and slot maps 0x158/0x1d8. This is not the complete hardware layout ABI.
 */
struct tu_mesh_output_buffer_layout {
   uint32_t shared_base;
   uint32_t stride;
   uint32_t max_count;
   uint16_t slot_offset[VARYING_SLOT_MAX];
};

struct tu_mesh_output_layout {
   struct tu_mesh_output_buffer_layout vertex;
   struct tu_mesh_output_buffer_layout primitive;
   uint32_t index_shared_base;
   uint32_t index_stride;
   uint32_t layer_shared_base;
   uint32_t viewport_shared_base;
   uint32_t shading_rate_shared_base;
   uint32_t cull_shared_base;
   unsigned vertices_per_primitive;
   bool index_store_volatile;
   bool uses_cull_bitmap;
};

/* Handles varyings and packed primitive output data. Count-wrapper policy,
 * backend stage integration and dispatch remain prerequisites for exposure.
 */
bool tu_nir_lower_mesh_outputs(nir_shader *nir,
                              const struct tu_mesh_output_layout *layout);

/* EXT SetMeshOutputs lowering at its uniform control-flow site. The recovered
 * compiler entry additionally hoists one constant entry-block site.
 * NV support and final publication-order validation remain separate.
 */
bool tu_nir_lower_mesh_counts(nir_shader *nir,
                             const struct tu_mesh_output_layout *layout);

/* Requires gathered mesh/FS information and lowered explicit I/O. */
bool tu_nir_collect_mesh_layout_inputs(const nir_shader *mesh,
                                      const nir_shader *fragment,
                                      tu_mesh_layout_inputs *inputs);
bool tu_nir_build_mesh_layout(nir_shader *mesh, const nir_shader *fragment,
                             bool index_store_volatile, tu_mesh_shader_layout *layout,
                             tu_mesh_output_layout *outputs);

/* Entry points for already explicit-I/O NIR. A successful mesh call
 * consumes the count/output intrinsics and returns the layout for later AQE
 * state setup. Unsupported NV/count/index-read paths fail before mutation.
 * The compiler calls these below its retained TASK/MESH rejection guard;
 * this source connection is not complete hardware stage support.
 */
bool tu_nir_lower_task_aqe(nir_shader *task);
bool tu_nir_lower_mesh_aqe(nir_shader *mesh, const nir_shader *fragment,
                          bool index_store_volatile,
                          tu_mesh_shader_layout *layout);
/* Compiler entry: derive packed-index volatility from the recovered layout
 * policy. The explicit override above remains available to isolated probes.
 */
bool tu_nir_lower_mesh_aqe_recovered(nir_shader *mesh, const nir_shader *fragment,
                                    tu_mesh_shader_layout *layout);

#endif
