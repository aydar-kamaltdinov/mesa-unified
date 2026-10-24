/* SPDX-License-Identifier: MIT */
#include "tu_mesh_layout.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>

static_assert(sizeof(tu_mesh_shader_layout) == 600);
static_assert(offsetof(tu_mesh_shader_layout, vertex_components) == 0x57);
static_assert(offsetof(tu_mesh_shader_layout, max_vertices) == 0x34);
static_assert(offsetof(tu_mesh_shader_layout, max_primitives) == 0x38);
static_assert(offsetof(tu_mesh_shader_layout, vertices_per_primitive) == 0x44);
static_assert(offsetof(tu_mesh_shader_layout, primitive_components) == 0x97);
static_assert(offsetof(tu_mesh_shader_layout, vertex_register) == 0xd7);
static_assert(offsetof(tu_mesh_shader_layout, vertex_component) == 0x117);
static_assert(offsetof(tu_mesh_shader_layout, vertex_offset) == 0x158);
static_assert(offsetof(tu_mesh_shader_layout, primitive_offset) == 0x1d8);
static_assert(offsetof(tu_mesh_shader_layout, has_layer) == 0x4d);
static_assert(offsetof(tu_mesh_shader_layout, synthesize_layer) == 0x4e);
static_assert(offsetof(tu_mesh_shader_layout, has_viewport) == 0x4f);
static_assert(offsetof(tu_mesh_shader_layout, has_shading_rate) == 0x51);

static uint32_t align_to(uint32_t n, uint32_t a) { return (n + a - 1) & ~(a - 1); }

void
tu_mesh_build_layout(const tu_mesh_layout_inputs *in, tu_mesh_shader_layout *out)
{
   assert(in->vertices_per_primitive >= 1 && in->vertices_per_primitive <= 3);
   assert(in->clip_distance_count <= 8 && in->cull_distance_count <= 8);
   memset(out, 0, sizeof(*out));
   memset(out->vertex_register, 0xff, sizeof(out->vertex_register));
   memset(out->vertex_component, 0xff, sizeof(out->vertex_component));
   memset(out->vertex_offset, 0xff, sizeof(out->vertex_offset));
   memset(out->primitive_offset, 0xff, sizeof(out->primitive_offset));
   memcpy(out->vertex_components, in->vertex_components, 64);
   memcpy(out->primitive_components, in->primitive_components, 64);
   out->layer_register = out->viewport_register = 0xff;
   out->has_layer = in->layer;
   out->synthesize_layer = in->view_mask && !in->layer;
   out->has_viewport = in->viewport;
   out->uses_cull_bitmap = in->cull_primitive;
   out->has_shading_rate = in->primitive_shading_rate;
   out->uses_primitive_id = in->primitive_id;
   out->uses_draw_id = in->uses_draw_id;
   out->uses_num_workgroups = in->uses_num_workgroups;
   out->clip_mask = (1u << in->clip_distance_count) - 1;
   out->cull_mask = ((1u << in->cull_distance_count) - 1) << in->clip_distance_count;
   if (!out->clip_mask && !out->cull_mask) {
      unsigned lo = in->vertex_components[17], hi = in->vertex_components[18];
      assert(lo <= 32 && hi <= 32);
      out->clip_mask = (lo == 32 ? 0xff : (1u << lo) - 1) |
                       (hi == 32 ? 0xf0 : ((1u << hi) - 1) << 4);
   }
   out->max_vertices = in->max_vertices;
   out->max_primitives = in->max_primitives;
   out->vertices_per_primitive = in->vertices_per_primitive;
   out->index_stride = in->vertices_per_primitive == 2 ? 6 : 8;
   out->shared_base = out->index_shared_base = align_to(in->shared_size, 64);
   out->vertex_shared_base = align_to(out->index_shared_base +
                                      out->index_stride * in->max_primitives, 64);

   /* Primitive shading rate has its own shared area. The original kernel
    * clears its used-map byte before generic primitive packing. */
   for (unsigned slot = 32; slot < 64; slot++) {
      if (in->primitive_used[slot] && in->primitive_components[slot] == 4) {
         out->primitive_offset[slot] = out->primitive_stride;
         out->primitive_stride += 16;
         out->primitive_attributes++;
      }
   }
   for (unsigned slot = 0; slot < 64; slot++) {
      if (!in->primitive_used[slot] || slot == 24 ||
          out->primitive_offset[slot] != UINT16_MAX)
         continue;
      out->primitive_offset[slot] = out->primitive_stride;
      out->primitive_stride += in->primitive_components[slot] ?
                               in->primitive_components[slot] * 4 : 16;
      out->primitive_attributes++;
   }

   auto allocate_vertex = [&](unsigned slot, unsigned bytes) {
      out->vertex_register[slot] = out->vertex_attributes++;
      out->vertex_component[slot] = 0;
      out->vertex_offset[slot] = out->vertex_stride;
      out->vertex_stride += bytes;
   };
   if (!in->unpacked_variable_mask) {
      uint16_t keys[32] = {};
      uint8_t occupied[32] = {};
      for (unsigned components = 3; components; components--) {
         for (unsigned slot = 32; slot < 64; slot++) {
            if (!in->vertex_used[slot] || in->vertex_components[slot] != components)
               continue;
            unsigned reg = 0;
            while (reg < out->vertex_attributes &&
                   (keys[reg] != in->qualifier_key[slot] || occupied[reg] + components > 4))
               reg++;
            if (reg == out->vertex_attributes) {
               keys[reg] = in->qualifier_key[slot];
               out->vertex_attributes++;
            }
            out->vertex_register[slot] = reg;
            out->vertex_component[slot] = occupied[reg];
            out->vertex_offset[slot] = reg * 16 + occupied[reg] * 4;
            occupied[reg] += components;
         }
      }
      out->vertex_stride = out->vertex_attributes * 16;
      for (unsigned slot = 32; slot < 64; slot++) {
         if (in->vertex_used[slot] &&
             (in->vertex_components[slot] == 0 || in->vertex_components[slot] == 4))
            allocate_vertex(slot, 16);
      }
   } else {
      for (unsigned priority = 0; priority < 2; priority++) {
         for (unsigned slot = 32; slot < 64; slot++) {
            bool selected = in->unpacked_variable_mask & (1u << (slot - 32));
            if (in->vertex_used[slot] && selected == (priority == 0))
               allocate_vertex(slot, 16);
         }
      }
   }
   for (unsigned slot = 1; slot < 32; slot++) {
      if (in->vertex_used[slot] && slot != 12)
         allocate_vertex(slot, 16);
   }
   out->vertex_attributes_before_specials = out->vertex_attributes;
   if (in->no_fragment_shader && in->primitive_id) {
      out->primitive_id_attributes = align_to(out->primitive_attributes, 2);
      out->vertex_attributes += out->primitive_id_attributes;
      out->vertex_stride += out->primitive_id_attributes * 16;
      out->primitive_stride = align_to(out->primitive_stride, 16);
   }
   if (in->vertex_used[0]) allocate_vertex(0, 16);
   if (in->vertex_used[12]) allocate_vertex(12, 4);
   if (in->layer || in->view_mask) out->layer_register = out->vertex_attributes;
   if (in->viewport) out->viewport_register = out->vertex_attributes;

   out->primitive_shared_base = align_to(out->vertex_shared_base +
                                         out->vertex_stride * in->max_vertices, 64);
   out->layer_shared_base = out->primitive_shared_base +
                            out->primitive_stride * in->max_primitives;
   out->viewport_shared_base = align_to(out->layer_shared_base +
      ((in->layer || in->view_mask) ? in->max_primitives * 4 : 0), 4);
   out->cull_shared_base = out->viewport_shared_base +
                           (in->viewport ? in->max_primitives * 4 : 0);
   out->shading_rate_shared_base = out->cull_shared_base +
                                   (in->cull_primitive ? in->max_primitives * 4 : 0);
   out->nv_counts_shared_base = (out->shading_rate_shared_base +
      (in->primitive_shading_rate ? in->max_primitives * 4 + 7 : 4)) & ~7u;
   out->shared_size = align_to(out->nv_counts_shared_base + 8, 64);
}
