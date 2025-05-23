#include <stdio.h>
#include "Model.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "../third_party/tiny_obj_loader.h"

Model::Model(Device& device, Buffers& buffers, std::string modelPath)
  : device(device), buffers(buffers) {
  loadObj(modelPath);
  createVertexBuffer();
  createIndexBuffer();
}

Model::~Model() {
  vkDestroyBuffer(device.getDevice(), indexBuffer, nullptr);
  vkFreeMemory(device.getDevice(), indexBufferMemory, nullptr);
  vkDestroyBuffer(device.getDevice(), vertexBuffer, nullptr);
  vkFreeMemory(device.getDevice(), vertexBufferMemory, nullptr);
}

void Model::loadObj(std::string modelPath) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, modelPath.c_str())) {
    throw std::runtime_error(err);
  }

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};

  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      Vertex vertex{};

      vertex.position = {
        attrib.vertices[3 * index.vertex_index + 0],
        attrib.vertices[3 * index.vertex_index + 1],
        attrib.vertices[3 * index.vertex_index + 2],
      };

      vertex.texCoord = {
        attrib.texcoords[2 * index.texcoord_index + 0],
        1.0 - attrib.texcoords[2 * index.texcoord_index + 1],
      };

      vertex.color = {1.0f, 1.0f, 1.0f};

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }

      indices.push_back(uniqueVertices[vertex]);
    }
  }
}

void Model::createVertexBuffer() {
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  buffers.createBuffer(
    bufferSize,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    stagingBuffer,
    stagingBufferMemory);

  void* data;
  vkMapMemory(device.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertices.data(), (size_t)bufferSize);
  vkUnmapMemory(device.getDevice(), stagingBufferMemory);

  buffers.createBuffer(
    bufferSize,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    vertexBuffer,
    vertexBufferMemory);

  buffers.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

  vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
  vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
}

void Model::createIndexBuffer() {
  VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();

  VkBuffer indexStagingBuffer;
  VkDeviceMemory indexStagingBufferMemory;
  buffers.createBuffer(
    indexBufferSize,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    indexStagingBuffer,
    indexStagingBufferMemory);

  void* indexData;
  vkMapMemory(device.getDevice(), indexStagingBufferMemory, 0, indexBufferSize, 0, &indexData);
  memcpy(indexData, indices.data(), (size_t)indexBufferSize);
  vkUnmapMemory(device.getDevice(), indexStagingBufferMemory);

  buffers.createBuffer(
    indexBufferSize,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    indexBuffer,
    indexBufferMemory);
  buffers.copyBuffer(indexStagingBuffer, indexBuffer, indexBufferSize);

  vkDestroyBuffer(device.getDevice(), indexStagingBuffer, nullptr);
  vkFreeMemory(device.getDevice(), indexStagingBufferMemory, nullptr);
}

