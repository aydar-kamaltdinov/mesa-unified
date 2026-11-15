/* SPDX-License-Identifier: MIT */
#include "tu_mesh_dispatch.h"
#include "tu_mesh_layout.h"
#include <assert.h>

static void
append_qw(uint32_t *&p, uint64_t value)
{
   *p++ = uint32_t(value);
   *p++ = uint32_t(value >> 32);
}

unsigned
tu_a8_build_mesh_dispatch_packet(uint32_t packet[32],
                                 const tu_a8_mesh_dispatch_state *state,
                                 const tu_mesh_shader_layout *m,
                                 uint32_t x, uint32_t y, uint32_t z,
                                 uint64_t indirect_iova, uint32_t ordinal)
{
   /* GameSir ELF 0x97ccb4. The 0x7a opcode is absent from our XML.
    * These are recovered type7 headers including parity, NOT CP_EXEC_CS.
    */
   uint32_t *p = packet;
   if (indirect_iova) {
      *p++ = 0x707a801e;
      *p++ = 7;
      append_qw(p, indirect_iova);
   } else {
      *p++ = 0x707a001f;
      *p++ = 5;
      *p++ = x;
      *p++ = y;
      *p++ = z;
   }

   append_qw(p, state->metadata_iova);
   /* The repeated upper word is also present in the original packet. */
   *p++ = uint32_t(state->metadata_iova >> 32);
   *p++ = state->statistics_enabled;
   *p++ = 0x100;

   bool special = m->has_layer || m->synthesize_layer || m->has_viewport ||
                  m->uses_cull_bitmap || m->uses_primitive_id ||
                  m->uses_draw_id || m->uses_num_workgroups ||
                  m->clip_mask || m->cull_mask || indirect_iova;
   uint64_t vertex_bytes = uint64_t(m->max_vertices) * m->vertex_stride * 0x2000;
   uint64_t primitive_bytes =
      uint64_t(m->max_primitives) * m->primitive_stride * 0x2000;
   special &= vertex_bytes + primitive_bytes < 0xf00001;
   uint32_t capacity = 0;
   if (!special) {
      if (!m->primitive_attributes && m->vertices_per_primitive == 1) {
         uint64_t attributes = uint64_t(m->vertex_attributes) * m->max_vertices;
         vertex_bytes = attributes > 0x77 ? 0xf00000 : attributes * 0x20000;
         primitive_bytes = 0;
      } else {
         /* Preserve the original unsigned 32-bit sizing arithmetic. */
         uint32_t vs = m->vertex_stride * m->max_vertices;
         uint32_t ps = m->primitive_stride * m->max_primitives;
         uint32_t total = vs + ps;
         if (total)
            capacity = total < 0x781 ? 0x2000 : (0xf00000 / total) & ~1u;
         vertex_bytes = uint32_t(vs * capacity);
         primitive_bytes = uint32_t(ps * capacity);
      }
   }
   if (!indirect_iova && m->primitive_attributes && uint64_t(x) * y * z >= 0x2001)
      vertex_bytes = primitive_bytes = 0x780000;
   if (!capacity)
      special = true;
   *p++ = special ? 0x1000 : capacity >> 1;

   const uint64_t resource = state->resource_iova;
   /* The ordinal offset wraps at 32 bits before addition to the IOVA. */
   append_qw(p, resource + uint32_t(ordinal * 0x10000 + 0x2053000));
   *p++ = 0x10000;
   append_qw(p, resource);
   *p++ = 0x1800000;

   const uint64_t primitive_base =
      state->gmem_base + 0x1068000 - uint32_t(primitive_bytes);
   append_qw(p, primitive_base - uint32_t(vertex_bytes));
   *p++ = uint32_t(vertex_bytes);
   append_qw(p, primitive_base);
   *p++ = uint32_t(primitive_bytes);
   append_qw(p, resource + 0x1800000);
   *p++ = 0x40000;
   append_qw(p, resource + 0x1850000);
   *p++ = 0x800000;
   append_qw(p, resource + 0x2050000);
   *p++ = 0x3000;
   assert(p - packet == (indirect_iova ? 31 : 32));
   return p - packet;
}

unsigned
tu_a8_build_mesh_view_state(uint32_t packet[8], bool multiview,
                             uint32_t view_index, uint32_t draw_index)
{
   /* GameSir ELF 0x97ca84; headers contain type4 parity. Known registers:
    * 9b0d PC_STEREO_RENDERING_VIEWMASK, 90c1 VPC_STEREO_RENDERING_VIEWMASK.
    */
   packet[0] = 0x40ab7801;
   packet[1] = draw_index;
   if (!multiview)
      return 2;
   packet[2] = 0x48ab7901;
   packet[3] = view_index;
   packet[4] = 0x489b0d01;
   packet[5] = 1u << (view_index & 31);
   packet[6] = 0x4090c101;
   packet[7] = packet[5];
   return 8;
}

unsigned
tu_a8_build_mesh_draw_begin(uint32_t packet[21],
                            const tu_a8_mesh_draw_state *state,
                            const tu_mesh_shader_layout *m)
{
   /* ELF 0x97c438: hardware-state tail AFTER allocation and draw_common.
    * Not a replacement for draw_common, tracing, dirty/cache or CCU setup.
    */
   uint32_t *p = packet;
   *p++ = 0x70438006; /* CP_SET_DRAW_STATE, two groups */
   *p++ = 0x1d720000; /* disable original group29 */
   append_qw(p, 0);
   *p++ = 0x02610000 | state->program_size |
          ((!state->program_iova || !state->program_size) ? 0x20000 : 0);
   append_qw(p, state->program_iova);

   /* PC_CNTL and VPC_PC_CNTL: provoking-last or D3D ordering, per the
    * recovered dynamic provoking-vertex mode test.
    */
   const uint32_t pc_cntl = state->provoking_vertex_last ? 2 : 4;
   *p++ = 0x489b0101;
   *p++ = pc_cntl;
   *p++ = 0x48930f01;
   *p++ = pc_cntl;

   unsigned views = state->view_mask ? 32 - __builtin_clz(state->view_mask | 1) : 0;
   *p++ = 0x409b0901; /* PC_STEREO_RENDERING_CNTL */
   *p++ = views > 1 ? 3 | (views << 2) : 0;
   if (state->view_mask) {
      *p++ = 0x489b0d01;
      *p++ = state->view_mask;
      *p++ = 0x4090c101;
      *p++ = state->view_mask;
   }
   *p++ = 0x40931301; /* VPC_UNKNOWN_CNTL */
   *p++ = 1 | ((m->has_layer || m->synthesize_layer) ? 4 : 0) |
          (uint32_t(m->has_viewport) << 1) |
          (uint32_t(m->has_shading_rate) << 3);
   if (m->vertices_per_primitive == 1) {
      *p++ = 0x48a00601; /* VFD_CNTL_6 */
      *p++ = 0xfc00;
   }
   assert(p - packet <= 21);
   return p - packet;
}

unsigned
tu_a8_build_mesh_draw_end(uint32_t packet[6], const tu_a8_mesh_draw_state *state)
{
   /* ELF 0x97d0ac, excluding trace/debug strings handled by the caller. */
   packet[0] = 0x40931301;
   packet[1] = 0;
   packet[2] = 0x70438003;
   packet[3] = 0x1d700000 | (state->restore_writeable ? 0x10000 : 0) |
               state->restore_size |
               ((!state->restore_iova || !state->restore_size) ? 0x20000 : 0);
   packet[4] = uint32_t(state->restore_iova);
   packet[5] = uint32_t(state->restore_iova >> 32);
   return 6;
}

unsigned
tu_a8_build_mesh_indirect_count_packet(
   uint32_t packet[47], const tu_a8_mesh_dispatch_state *state,
   const tu_mesh_shader_layout *layout, uint32_t view_mask,
   uint32_t view_index, uint32_t draw_index, uint64_t indirect_iova,
   uint64_t count_iova, uint64_t always_true_iova, uint32_t ordinal)
{
   /* ELF 0x94a2ac: if draw_index >= GPU count, CP_COND_EXEC executes a
    * one-word CP_NOP header swallowing view setup + indirect dispatch.
    * The caller must keep this entire 41/47-word sequence in one IB.
    */
   assert(indirect_iova && always_true_iova);
   uint32_t *p = packet;
   *p++ = 0x70c48006; /* CP_COND_EXEC header including parity */
   append_qw(p, always_true_iova);
   append_qw(p, count_iova);
   *p++ = draw_index;
   *p++ = 1;
   *p++ = view_mask ? 0x70108027 : 0x70108021; /* CP_NOP, 39/33 words */
   p += tu_a8_build_mesh_view_state(p, view_mask != 0, view_index, draw_index);
   p += tu_a8_build_mesh_dispatch_packet(p, state, layout, 0, 0, 0,
                                        indirect_iova, ordinal);
   assert(p - packet == (view_mask ? 47 : 41));
   return p - packet;
}
