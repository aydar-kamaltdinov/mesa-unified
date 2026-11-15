/*
 * Copyright © 2021 Bas Nieuwenhuizen
 * Copyright © 2024 Valve Corporation
 * SPDX-License-Identifier: MIT
 */

#include "tu_cmd_buffer.h"
#include "tu_device.h"
#include "tu_meta_compute.h"
#include "common/freedreno_gpu_event.h"

static const uint32_t copy_memory_indirect_spv[] = {
#include "tu_copy_memory_indirect.spv.h"
};

static VkResult
get_copy_memory_indirect_pipeline(struct tu_device *device,
                                  VkPipeline *pipeline,
                                  VkPipelineLayout *layout)
{
   static const char key[] = "tu-copy-memory-indirect-v1";
   const VkPushConstantRange range = {
      .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
      .size = sizeof(VkDeviceAddress),
   };
   VkResult result = vk_meta_get_pipeline_layout(
      &device->vk, &device->meta, NULL, &range, key, sizeof(key), layout);
   if (result != VK_SUCCESS)
      return result;

   *pipeline = vk_meta_lookup_pipeline(&device->meta, key, sizeof(key));
   if (*pipeline != VK_NULL_HANDLE)
      return VK_SUCCESS;

   const VkShaderModuleCreateInfo module = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = sizeof(copy_memory_indirect_spv),
      .pCode = copy_memory_indirect_spv,
   };
   const VkComputePipelineCreateInfo info = {
      .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
      .stage = {
         .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
         .pNext = &module,
         .stage = VK_SHADER_STAGE_COMPUTE_BIT,
         .pName = "main",
      },
      .layout = *layout,
   };
   return vk_meta_create_compute_pipeline(&device->vk, &device->meta, &info,
                                          key, sizeof(key), pipeline);
}

template <chip CHIP>
VKAPI_ATTR void VKAPI_CALL
tu_CmdCopyMemoryIndirectKHR(VkCommandBuffer commandBuffer,
                            const VkCopyMemoryIndirectInfoKHR *info)
{
   VK_FROM_HANDLE(tu_cmd_buffer, cmd, commandBuffer);
   if (!info->copyCount)
      return;

   VkPipeline pipeline;
   VkPipelineLayout layout;
   VkResult result = get_copy_memory_indirect_pipeline(cmd->device,
                                                       &pipeline, &layout);
   if (result != VK_SUCCESS) {
      vk_command_buffer_set_error(&cmd->vk, result);
      return;
   }

   struct tu_saved_compute_state saved;
   tu_save_compute_state(cmd, &saved);

   /* The API exposes transfer accesses; this implementation uses UCHE.
    * Resolve pending CCU writes before the first shader reads its command
    * or data.  Normal dispatch emission consumes these cache transitions.
    */
   tu_flush_for_access<CHIP>(&cmd->state.cache, TU_ACCESS_NONE,
                             (tu_cmd_access_mask)(TU_ACCESS_UCHE_READ | TU_ACCESS_UCHE_WRITE));
   tu_CmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
   cmd->state.dirty |= TU_CMD_DIRTY_SHADER_CONSTS | TU_CMD_DIRTY_COMPUTE_DESC_SETS;

   for (uint32_t i = 0; i < info->copyCount; i++) {
      VkDeviceAddress address = info->copyAddressRange.address +
                                uint64_t(i) * info->copyAddressRange.stride;
      vk_common_CmdPushConstants(commandBuffer, layout,
                                  VK_SHADER_STAGE_COMPUTE_BIT, 0,
                                  sizeof(address), &address);
      tu_CmdDispatchBase<CHIP>(commandBuffer, 0, 0, 0, 1, 1, 1);
   }

   /* Match GameSir's post-copy UCHE clean/invalidate and completion wait.
    * Later transfer barriers must not need to know that a compute shader
    * implemented the copy.
    */
   tu_emit_event_write<CHIP>(cmd, &cmd->cs, FD_CACHE_CLEAN);
   tu_emit_event_write<CHIP>(cmd, &cmd->cs, FD_CACHE_INVALIDATE);
   tu_cs_emit_pkt7(&cmd->cs, CP_WAIT_FOR_IDLE, 0);
   tu_restore_compute_state(cmd, &saved);
}
TU_GENX(tu_CmdCopyMemoryIndirectKHR);
