/* SPDX-License-Identifier: MIT */
#ifndef TU_MESH_VPC_H
#define TU_MESH_VPC_H

#include "ir3/ir3_shader.h"
#include "tu_mesh_layout.h"
#include "tu_cs.h"

bool tu_a8_build_mesh_fs_inputs(const struct tu_mesh_shader_layout *layout,
                                struct ir3_mesh_fs_inputs *out);

bool tu_a8_mesh_fs_inputs_match(const struct tu_mesh_shader_layout *layout,
                                const struct ir3_mesh_fs_inputs *compiled);

/* Detached linkage preparation, recovered from the mesh branch of GameSir
 * tu_emit_program_state, ELF0x9e9070. These variant copies borrow all pointers
 * from the real FS. They must not be destroyed as independently owned shaders.
 * No command stream or live shader is changed by this helper.
 */
struct tu_a8_mesh_vpc_plan {
   /* The original synthetic producer can name every layout slot plus layer
    * and viewport. Keep this separate from Mesa's smaller ordinary VS array;
    * the mesh emitter merges physical registers before HW upload.
    */
   struct ir3_shader_output producer_outputs[66];
   unsigned producer_output_count;
   uint8_t clip_mask, cull_mask;
   struct ir3_shader_variant full_fs, vertex_fs;
   bool has_fs, primitive_id_per_vertex;
   uint64_t skip_output_slots;
   uint32_t initial_location, primitive_base;
};

/* The caller must discard the plan on failure. Locations outside the current
 * 128-component VPC mode tables are rejected pending capability reconciliation.
 * This prepares linkage only, not a complete mesh VPC draw state.
 */
bool tu_a8_prepare_mesh_vpc(const struct tu_mesh_shader_layout *layout,
                            const struct ir3_shader_variant *fs,
                            struct tu_a8_mesh_vpc_plan *out);

/* Input is the semantic linkage list after fixed-function outputs were added,
 * before compacting aliases. loc is not needed: AQE destinations use regid.
 */
struct tu_a8_mesh_vpc_link {
   uint8_t slot, regid, compmask;
};
struct tu_a8_mesh_vpc_maps {
   uint32_t output[16], destination[8];
   unsigned count;
};
bool tu_a8_build_mesh_vpc_maps(const struct tu_a8_mesh_vpc_link *links,
                               unsigned count, uint64_t skip_output_slots,
                               struct tu_a8_mesh_vpc_maps *out);

/* Emits into a caller-owned unpublished sub-stream. On false the caller must
 * discard that entire stream; allocation/CS status must also be checked by the
 * uploader. No program-state publication or feature exposure is done here.
 */
bool tu_a8_emit_mesh_vpc(struct tu_cs *cs,
                         const struct tu_mesh_shader_layout *layout,
                         const struct ir3_shader_variant *fs);

struct tu_a8_mesh_vpc_upload {
   const uint32_t *map;
   uint32_t dwords;
   struct tu_draw_state state;
};
VkResult tu_a8_upload_mesh_vpc(struct tu_cs *sub_cs,
                               const struct tu_mesh_shader_layout *layout,
                               const struct ir3_shader_variant *fs,
                               struct tu_a8_mesh_vpc_upload *out);

#endif
