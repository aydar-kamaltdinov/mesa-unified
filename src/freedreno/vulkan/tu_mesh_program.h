/* SPDX-License-Identifier: MIT */
#ifndef TU_MESH_PROGRAM_H
#define TU_MESH_PROGRAM_H

#include "tu_cs.h"
#include "tu_mesh_layout.h"

struct ir3_shader_variant;
struct ir3_mesh_fs_inputs;
struct tu_program_state;

/* Semantic records, not the original packed IR3 input flags. The adapter
 * copies per_primitive from the linked shader; do not confuse original flag
 * bit5 with this tree's existing per_vertex bitfield.
 */
struct tu_a8_mesh_fs_input {
   uint8_t slot, compmask, inloc;
   bool per_primitive;
};

struct tu_a8_mesh_program_inputs {
   const struct tu_mesh_shader_layout *layout;
   const struct ir3_shader_variant *mesh, *task;
   struct tu_draw_state mesh_upload, task_upload;
   const struct tu_a8_mesh_fs_input *fs_inputs;
   unsigned fs_input_count;
   bool has_fragment_shader;
   uint32_t view_mask;
   /* Borrowed from the selected FS; null/disabled keeps detached legacy
    * placement. Production passes the same immutable options as final VPC.
    */
   const struct ir3_mesh_fs_inputs *compiled_fs_inputs;
};

struct tu_a8_mesh_program_plan {
   /* SP_VS output and destination maps currently describe at most32 entries
    * in our register XML. Larger layouts fail the detached builder until
    * their hardware range is recovered; no feature is advertised from this.
    */
   struct tu_reg_value regs[80];
   unsigned reg_count;
   uint32_t fetch[32], destination[32];
   unsigned fetch_count;
   bool primitive_override;
   uint32_t primitive_span, primitive_start;
   uint32_t vertex_stride, primitive_stride;
   uint32_t metadata[20];
};

/* Sub-CS owns all returned storage. setup_map and its split are consumed
 * by tu_a8_splice_mesh_program_state; metadata is data for AQE, not PM4.
 */
struct tu_a8_mesh_program_upload {
   struct tu_cs_memory passthrough;
   const uint32_t *setup_map;
   uint32_t setup_dwords, vpc_split_dwords;
   struct tu_draw_state draw, binning, metadata;
};

/* Copy live varying records from named local IR3 fields; no original ABI
 * flag-byte interpretation. Production linkage additionally supplies the
 * selected FS immutable physical-placement options.
 */
bool tu_a8_collect_mesh_fs_inputs(
   const struct ir3_shader_variant *fs, struct tu_a8_mesh_fs_input *out,
   unsigned capacity, unsigned *count);

/* Splice setup-prefix, FS upload, mesh VPC and setup-suffix in original order.
 * Each input remains owned by its existing shader/sub-CS allocation.
 */
VkResult tu_a8_splice_mesh_program_state(
   struct tu_cs *sub_cs, const struct tu_a8_mesh_program_upload *setup,
   const uint32_t *fs, unsigned fs_dwords,
   const uint32_t *vpc, unsigned vpc_dwords, struct tu_draw_state *out);

bool tu_a8_build_mesh_program_plan(
   const struct tu_a8_mesh_program_inputs *in,
   struct tu_a8_mesh_program_plan *out);

VkResult tu_a8_upload_mesh_program(
   struct tu_cs *sub_cs, const struct tu_a8_mesh_program_inputs *in,
   struct tu_a8_mesh_program_upload *out);

/* Related allocations share sub_cs ownership. The caller publishes combined
 * into the original mesh state group only after the whole upload succeeds.
 */
struct tu_a8_linked_mesh_program {
   struct tu_a8_mesh_program_upload setup;
   struct tu_draw_state combined;
};
VkResult tu_a8_upload_linked_mesh_program(
   struct tu_cs *sub_cs, const struct tu_a8_mesh_program_inputs *in,
   const struct ir3_shader_variant *fs, const uint32_t *fs_state,
   unsigned fs_dwords, struct tu_a8_linked_mesh_program *out);

#endif
