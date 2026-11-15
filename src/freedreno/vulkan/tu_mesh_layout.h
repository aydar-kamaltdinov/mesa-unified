/* SPDX-License-Identifier: MIT */
#ifndef TU_MESH_LAYOUT_H
#define TU_MESH_LAYOUT_H

#include <stdint.h>

/* Fixed AQE ABI prefix: GameSir reserves nine vec4 registers before API
 * constants. DrawID and ViewIndex occupy scalar slots 32 and 33. */
enum { TU_AQE_CONST_PREFIX_VEC4 = 9 };

/* Precollected I/O, not raw nir_shader fields. Keys combine the store source
 * type (low byte) and whether the fragment shader interpolates that slot (bit8).
 */
struct tu_mesh_layout_inputs {
   uint32_t shared_size;
   uint32_t unpacked_variable_mask;
   uint16_t max_vertices, max_primitives;
   uint8_t vertices_per_primitive;
   uint8_t clip_distance_count, cull_distance_count;
   bool view_mask, layer, viewport, cull_primitive, primitive_shading_rate;
   bool primitive_id, no_fragment_shader;
   bool uses_draw_id, uses_num_workgroups;
   uint8_t vertex_components[64], primitive_components[64];
   bool vertex_used[64], primitive_used[64];
   uint16_t qualifier_key[64];
};

/* Recovered 600-byte layout; raw nir_shader offsets are not part of this ABI. */
struct tu_mesh_shader_layout {
   uint32_t shared_base, index_shared_base, index_stride;
   uint32_t vertex_shared_base, vertex_stride;
   uint32_t primitive_shared_base, primitive_stride;
   uint32_t layer_shared_base, viewport_shared_base, cull_shared_base;
   uint32_t shading_rate_shared_base, nv_counts_shared_base, shared_size;
   uint32_t max_vertices, max_primitives, static_vertices, static_primitives;
   uint8_t vertices_per_primitive, vertex_attributes, primitive_attributes;
   uint8_t vertex_attributes_before_specials, primitive_id_attributes;
   uint8_t layer_register, viewport_register, clip_mask, cull_mask;
   uint8_t has_layer, synthesize_layer, has_viewport, uses_cull_bitmap;
   uint8_t has_shading_rate, uses_primitive_id, uses_draw_id, uses_num_workgroups;
   uint8_t dynamic_counts, counts_initialized;
   uint8_t vertex_components[64], primitive_components[64];
   uint8_t vertex_register[64], vertex_component[64];
   uint8_t padding;
   uint16_t vertex_offset[64], primitive_offset[64];
};

void tu_mesh_build_layout(const struct tu_mesh_layout_inputs *in,
                          struct tu_mesh_shader_layout *out);

/* Packed index writes preserve flag updates made by these primitive outputs.
 * Cull bitmap uses separate storage and is not part of this predicate.
 */
static inline bool
tu_mesh_indices_require_volatile(const struct tu_mesh_shader_layout *layout)
{
   return layout->has_layer || layout->synthesize_layer ||
          layout->has_viewport || layout->has_shading_rate;
}

#endif
