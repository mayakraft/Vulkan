#include "Material.h"
#include "../geometry/Uniforms.h"
#include "Renderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

Material::Material(
  Device& device,
  Buffers& buffers,
  SwapChain& swapChain,
  Renderer& renderer,
  std::string texturePath)
  : device(device),
    buffers(buffers),
    swapChain(swapChain),
    renderer(renderer),
    texturePath(texturePath)
  {

  createDescriptorSetLayout();

  // viking room example
  config = PipelineConfig{};
  config.vertPath = "./examples/viking_room/shaders/simple.vert.spv";
  config.fragPath = "./examples/viking_room/shaders/simple.frag.spv";
  config.renderPass = renderer.getRenderPass();
  config.extent = swapChain.getSwapChainExtent();
  config.msaaSamples = device.getMsaaSamples();
  config.descriptorSetLayout = descriptorSetLayout;
  /*config(*/
  /*  "./shaders/simple.vert.spv",*/
  /*  "./shaders/simple.frag.spv",*/
  /*  renderer.getRenderPass(),*/
  /*  swapChain.getSwapChainExtent(),*/
  /*  device.getMsaaSamples(),*/
  /*  descriptorSetLayout),*/

  createUniformBuffers();

  createTextureImage();
  createTextureImageView();
  createTextureSampler();

  createDescriptorSets();

  graphicsPipeline = std::make_unique<GraphicsPipeline>(device.getDevice(), config);
  /*graphicsPipeline = GraphicsPipeline(device.getDevice(), config);*/
}

void Material::updateExtent(VkExtent2D newExtent) {
  config.extent = newExtent;
  /*graphicsPipeline.config.extent = newExtent;*/
}

Material::~Material() {
  vkDestroyDescriptorSetLayout(device.getDevice(), descriptorSetLayout, nullptr);

  // textures
  vkDestroySampler(device.getDevice(), textureSampler, nullptr);
  vkDestroyImageView(device.getDevice(), textureImageView, nullptr);
  vkDestroyImage(device.getDevice(), textureImage, nullptr);
  vkFreeMemory(device.getDevice(), textureImageMemory, nullptr);

  // uniforms
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroyBuffer(device.getDevice(), uniformBuffers[i], nullptr);
    vkFreeMemory(device.getDevice(), uniformBuffersMemory[i], nullptr);
  }
}

// for uniforms
// the rest of the uniform related code is in the Renderer class.
void Material::createDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  // uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
  uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
    uboLayoutBinding,
    samplerLayoutBinding,
  };

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(device.getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout");
  }
}

/*void Material::createDescriptorSets(*/
/*  VkDescriptorPool descriptorPool,*/
/*  std::vector<VkDescriptorSetLayout> layouts) {*/
void Material::createDescriptorSets() {
  /*std::vector<VkDescriptorSetLayout> layouts(*/
  /*  MAX_FRAMES_IN_FLIGHT,*/
  /*  pipeline.getDescriptorSetLayout());*/
  /*std::vector<VkDescriptorSetLayout> layouts(*/
  /*  MAX_FRAMES_IN_FLIGHT,*/
  /*  config.descriptorSetLayout);*/
  std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = renderer.getDescriptorPool();
  allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
  if (vkAllocateDescriptorSets(device.getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets");
  }

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImageView;
    imageInfo.sampler = textureSampler;

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSets[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;
    descriptorWrites[0].pImageInfo = nullptr; // Optional
    descriptorWrites[0].pTexelBufferView = nullptr; // Optional

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSets[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;
    descriptorWrites[1].pBufferInfo = nullptr; // Optional
    descriptorWrites[1].pTexelBufferView = nullptr; // Optional

    vkUpdateDescriptorSets(
      device.getDevice(),
      static_cast<uint32_t>(descriptorWrites.size()),
      descriptorWrites.data(),
      0,
      nullptr);
  }
}

void Material::createUniformBuffers() {
  VkDeviceSize bufferSize = sizeof(UniformBufferObject);

  uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
  uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
  uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    buffers.createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      uniformBuffers[i],
      uniformBuffersMemory[i]);
    vkMapMemory(
      device.getDevice(),
      uniformBuffersMemory[i],
      0,
      bufferSize,
      0,
      &uniformBuffersMapped[i]);
  }
}

void Material::updateUniformBuffer(uint32_t currentImage) {
  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  UniformBufferObject ubo{};
  ubo.model = glm::rotate(
    glm::mat4(1.0f),
    time * glm::radians(90.0f),
    glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = glm::lookAt(
    glm::vec3(2.0f, 2.0f, 2.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.projection = glm::perspective(
    glm::radians(45.0f),
    swapChain.getSwapChainExtent().width / (float) swapChain.getSwapChainExtent().height,
    0.1f,
    10.0f);
  ubo.projection[1][1] *= -1;

  memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void Material::createTextureImage() {
  int texWidth, texHeight, texChannels;

  stbi_uc* pixels = stbi_load(
    texturePath.c_str(),
    &texWidth,
    &texHeight,
    &texChannels,
    STBI_rgb_alpha);

  VkDeviceSize imageSize = texWidth * texHeight * 4;

  if (!pixels) {
    throw std::runtime_error("failed to load texture image");
  }
  mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;

  buffers.createBuffer(
    imageSize,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    stagingBuffer,
    stagingBufferMemory);

  void* data;
  vkMapMemory(device.getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
  memcpy(data, pixels, static_cast<size_t>(imageSize));
  vkUnmapMemory(device.getDevice(), stagingBufferMemory);

  stbi_image_free(pixels);

  buffers.createImage(
    static_cast<uint32_t>(texWidth),
    static_cast<uint32_t>(texHeight),
    mipLevels,
    VK_SAMPLE_COUNT_1_BIT,
    VK_FORMAT_R8G8B8A8_SRGB,
    VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    textureImage,
    textureImageMemory
  );

  buffers.transitionImageLayout(
    textureImage,
    VK_FORMAT_R8G8B8A8_SRGB,
    VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    mipLevels);

  buffers.copyBufferToImage(
    stagingBuffer,
    textureImage,
    static_cast<uint32_t>(texWidth),
    static_cast<uint32_t>(texHeight));

  // present from before we added mipmaps
  // transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

  buffers.generateMipmaps(
    textureImage,
    VK_FORMAT_R8G8B8A8_SRGB,
    texWidth,
    texHeight,
    mipLevels);

  vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
  vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
}

void Material::createTextureImageView() {
  textureImageView = buffers.createImageView(
    textureImage,
    VK_FORMAT_R8G8B8A8_SRGB,
    VK_IMAGE_ASPECT_COLOR_BIT,
    mipLevels);
}

void Material::createTextureSampler() {
  VkPhysicalDeviceProperties deviceProperties{};
  vkGetPhysicalDeviceProperties(device.getPhysicalDevice(), &deviceProperties);

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.maxLod = static_cast<float>(mipLevels);
  // samplerInfo.minLod = static_cast<float>(mipLevels / 2);
  samplerInfo.minLod = 0.0f; // Optional
  samplerInfo.mipLodBias = 0.0f; // Optional

  if (vkCreateSampler(device.getDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler");
  }
}

