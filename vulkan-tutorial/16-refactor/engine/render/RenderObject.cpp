#include "RenderObject.h"

RenderObject::RenderObject(Model& model, Material& material)
  : model(model), material(material) { }

void RenderObject::recordCommandBuffer(
  VkCommandBuffer commandBuffer,
  uint32_t currentFrame
) {
  // bind the graphics pipeline
  // the second parameter specifies if the pipeline is graphics or compute
  vkCmdBindPipeline(
    commandBuffer,
    VK_PIPELINE_BIND_POINT_GRAPHICS,
    material.getPipeline());

  VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(material.config.extent.width);
	viewport.height = static_cast<float>(material.config.extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = material.config.extent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	VkBuffer vertexBuffers[] = {model.vertexBuffer};
	VkDeviceSize offsets[] = {0};

  // this used to be after vertex/index binding but before the draw call,
  // but now that we abstracted drawing into each object,
  // this has now been moved to be before vertex/index binding.
  vkCmdBindDescriptorSets(
    commandBuffer,
    VK_PIPELINE_BIND_POINT_GRAPHICS,
    material.getPipelineLayout(),
    0,
    1,
    &(material.getDescriptorSets()[currentFrame]),
    0,
    nullptr);

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, model.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	// used previously before adding index buffers
	// vkCmdDraw(commandBuffer, static_cast<uint32_t>(model.vertices.size()), 1, 0, 0);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(model.indices.size()), 1, 0, 0, 0);
}

