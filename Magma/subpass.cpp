#include "subpass.h"
#include "gfx_structs.h"
#include "gfx_wrap.h"

void Subpass::init() {
  description = MVkBaseSubpass;
}

void Subpass::bind(VkCommandBuffer cmd) {
  // Advance subpass if not first subpass
  if (index > 0) {
    vkCmdNextSubpass(cmd, VK_SUBPASS_CONTENTS_INLINE);
  }
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet, 0, NULL);
}
