#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <string>
#include "Device.h"
#include "memory/Buffers.h"
#include "Material.h"
#include "Vertex.h"

class RenderObject {
public:
  RenderObject(
    Device& device,
    Buffers& buffers,
    Material& material,
    std::string modelPath);
  ~RenderObject();

  void recordCommandBuffer(
    VkCommandBuffer commandBuffer,
    /*VkPipelineLayout pipelineLayout,*/
    uint32_t currentFrame);

  // Disallow copying
  RenderObject(const RenderObject&) = delete;
  RenderObject& operator=(const RenderObject&) = delete;

  // Allow move-construction, disallow move-assignment
  RenderObject(RenderObject&&) noexcept = default;
  RenderObject& operator=(RenderObject&&) noexcept = delete;

private:
  Device& device;
  Buffers& buffers;
  Material& material;

  // the mesh geometry to be rendered
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  // model
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  void loadObj(std::string modelPath);
  void createVertexBuffer();
  void createIndexBuffer();
};

