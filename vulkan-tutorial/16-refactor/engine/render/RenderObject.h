#pragma once

#include <vulkan/vulkan.h>
#include "Model.h"
#include "Material.h"

class RenderObject {
public:
  RenderObject(Model& model, Material& material);

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t currentFrame);

  RenderObject(const RenderObject&) = delete;
  RenderObject& operator=(const RenderObject&) = delete;
  RenderObject(RenderObject&&) noexcept = default;
  RenderObject& operator=(RenderObject&&) noexcept = delete;

private:
  Model& model;
  Material& material;
};

