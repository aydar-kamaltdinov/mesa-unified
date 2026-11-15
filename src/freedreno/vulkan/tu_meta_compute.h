/*
 * Copyright © 2021 Bas Nieuwenhuizen
 * Copyright © 2024 Valve Corporation
 * SPDX-License-Identifier: MIT
 */
#ifndef TU_META_COMPUTE_H
#define TU_META_COMPUTE_H

#include "tu_cmd_buffer.h"
#include "tu_shader.h"

/* Meta dispatches must preserve both pieces of the bound compute pipeline:
 * the program and the descriptor-load commands.  Descriptor registers may
 * also have been overwritten by the meta dispatch.
 */
struct tu_saved_compute_state {
   uint32_t push_constants[MAX_PUSH_CONSTANTS_SIZE / 4];
   struct tu_shader *compute_shader;
   struct tu_draw_state compute_load_state;
};

static inline void
tu_save_compute_state(struct tu_cmd_buffer *cmd,
                      struct tu_saved_compute_state *state)
{
   memcpy(state->push_constants, cmd->push_constants, sizeof(cmd->push_constants));
   state->compute_shader = cmd->state.shaders[MESA_SHADER_COMPUTE];
   state->compute_load_state = cmd->state.compute_load_state;
}

static inline void
tu_restore_compute_state(struct tu_cmd_buffer *cmd,
                         struct tu_saved_compute_state *state)
{
   cmd->state.shaders[MESA_SHADER_COMPUTE] = state->compute_shader;
   cmd->state.compute_load_state = state->compute_load_state;
   if (state->compute_shader) {
      tu_cs_emit_state_ib(&cmd->cs, state->compute_shader->state);
   }
   memcpy(cmd->push_constants, state->push_constants, sizeof(cmd->push_constants));
   cmd->state.dirty |= TU_CMD_DIRTY_SHADER_CONSTS | TU_CMD_DIRTY_COMPUTE_DESC_SETS;
}

#endif
