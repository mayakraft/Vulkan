#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <string>
#include "GraphicsPipeline.h"
#include "Device.h"
#include "Buffers.h"
#include "Material.h"
#include "Vertex.h"

class RenderObject {
public:
  RenderObject(Device& device, Buffers& buffers, Material& material);
  ~RenderObject();

  void loadModel();
  void createVertexBuffer();
  void createIndexBuffer();

  /*void createTextureImage();*/
  /*void createTextureImageView();*/
  /*void createTextureSampler();*/

  void recordCommandBuffer(
    VkCommandBuffer commandBuffer,
    VkPipelineLayout pipelineLayout,
    uint32_t currentFrame);
    // VkDescriptorSet descriptorSet);

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

  const std::string MODEL_PATH = "./assets/viking_room.obj";
  /*const std::string TEXTURE_PATH = "./assets/viking_room.png";*/

  // the mesh geometry to be rendered
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  // model
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  // texture
/*  uint32_t mipLevels;*/
/*  VkImage textureImage;*/
/*  VkDeviceMemory textureImageMemory;*/
/*public:*/
/*  VkImageView textureImageView;*/
/*  VkSampler textureSampler;*/
};

