/* SPDX-License-Identifier: MIT */
#ifndef TU_MESH_DISPATCH_H
#define TU_MESH_DISPATCH_H

#include <stdint.h>

struct tu_mesh_shader_layout;

#define TU_A8_MESH_AQE_SIZE 0x2073000u

/* Original groups2/29 match the current VS_BINNING/dynamic vertex-input
 * enum values. tu_mesh_draw.cc supplies the typed caller/ownership wiring;
 * the integrated AQE program still requires execution validation.
 */
struct tu_a8_mesh_draw_state {
   uint64_t program_iova;
   uint16_t program_size;
   uint64_t restore_iova;
   uint16_t restore_size;
   bool restore_writeable;
   uint32_t view_mask;
   /* Original cmd+0x3ec: dynamic rasterizer provoking-vertex mode. */
   bool provoking_vertex_last;
};

unsigned tu_a8_build_mesh_draw_begin(
   uint32_t packet[21], const struct tu_a8_mesh_draw_state *state,
   const struct tu_mesh_shader_layout *layout);
unsigned tu_a8_build_mesh_draw_end(
   uint32_t packet[6], const struct tu_a8_mesh_draw_state *state);

/* Recovered A8 packet inputs, deliberately separate from tu_cmd_buffer ABI.
 * The caller must own/pin the complete AQE allocation and GMEM reservation
 * until execution finishes. Allocation, synchronization and draw lifecycle
 * are not supplied by these packet builders.
 */
struct tu_a8_mesh_dispatch_state {
   uint64_t resource_iova;
   uint64_t gmem_base;
   /* cmd+0x1a20 = program+0xb0: twenty-word AQE metadata allocation. */
   uint64_t metadata_iova;
   /* Original cmd+0x5734: conservative query-accounting latch, initialized
    * from inherited pipeline statistics and set by primitive/stat queries.
    */
   uint8_t statistics_enabled;
};

/* Return the written dword count (32 direct, 31 indirect). A nonzero indirect
 * address selects the indirect packet, as in the original helper. This only
 * constructs bytes; it neither allocates resources nor submits commands.
 */
unsigned tu_a8_build_mesh_dispatch_packet(
   uint32_t packet[32], const struct tu_a8_mesh_dispatch_state *state,
   const struct tu_mesh_shader_layout *layout,
   uint32_t x, uint32_t y, uint32_t z, uint64_t indirect_iova,
   uint32_t ordinal);

/* Return 2 or 8 dwords. Original userspace writes draw_index to ab78 and,
 * for multiview, view_index to ab79. The IR3 preamble copies shared GFX
 * words72/73 into shader prefix32/33; execution ordering remains unvalidated.
 */
unsigned tu_a8_build_mesh_view_state(uint32_t packet[8], bool multiview,
                                     uint32_t view_index, uint32_t draw_index);

/* A complete conditional body for one view/draw, bounded to one IB by the
 * caller in tu_mesh_draw.cc. always_true_iova must point to device-visible
 * UINT32_MAX owned by the command buffer. No host read of the indirect count.
 */
unsigned tu_a8_build_mesh_indirect_count_packet(
   uint32_t packet[47], const struct tu_a8_mesh_dispatch_state *state,
   const struct tu_mesh_shader_layout *layout, uint32_t view_mask,
   uint32_t view_index, uint32_t draw_index, uint64_t indirect_iova,
   uint64_t count_iova, uint64_t always_true_iova, uint32_t ordinal);

/* Internal begin/end snapshot. Pipeline/cache owns layout/program/metadata;
 * device owns AQE BO. The caller must not rebind shaders between boundaries.
 */
struct tu_a8_mesh_draw_session {
   const struct tu_mesh_shader_layout *layout;
   struct tu_a8_mesh_draw_state draw;
   struct tu_a8_mesh_dispatch_state dispatch;
   bool active;
};

struct tu_cmd_buffer;
struct tu_bo;
/* Capture typed packet inputs only. The caller owns/pins AQE/metadata storage
 * and reserves GMEM, and still supplies lifecycle/cache/synchronization.
 */
bool tu_a8_prepare_mesh_dispatch_state(
   const struct tu_cmd_buffer *cmd, const struct tu_bo *aqe_bo,
   uint64_t gmem_base, struct tu_a8_mesh_dispatch_state *out);

struct tu_cs;
/* Dispatch-only command emission. The caller supplies fully prepared AQE
 * program/descriptors, begin/end state, BO ownership and cache/CCU handling.
 * These helpers are deliberately not Vulkan entrypoints.
 */
void tu_a8_emit_mesh_direct_dispatches(
   struct tu_cs *cs, const struct tu_a8_mesh_dispatch_state *state,
   const struct tu_mesh_shader_layout *layout, uint32_t view_mask,
   uint32_t x, uint32_t y, uint32_t z);
void tu_a8_emit_mesh_indirect_dispatches(
   struct tu_cs *cs, const struct tu_a8_mesh_dispatch_state *state,
   const struct tu_mesh_shader_layout *layout, uint32_t view_mask,
   uint64_t indirect_iova, uint32_t draw_count, uint32_t stride,
   uint64_t count_iova, uint64_t always_true_iova);

#endif
