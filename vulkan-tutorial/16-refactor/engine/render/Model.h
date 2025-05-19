#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <string>
#include "../core/Device.h"
#include "../memory/Buffers.h"
#include "../geometry/Vertex.h"

class Model {
public:
  Model(
    Device& device,
    Buffers& buffers,
    std::string modelPath);
  ~Model();

  // the mesh geometry to be rendered
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  // model
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  // Disallow copying
  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;

  // Allow move-construction, disallow move-assignment
  Model(Model&&) noexcept = default;
  Model& operator=(Model&&) noexcept = delete;

private:
  Device& device;
  Buffers& buffers;

  void loadObj(std::string modelPath);
  void createVertexBuffer();
  void createIndexBuffer();
};

