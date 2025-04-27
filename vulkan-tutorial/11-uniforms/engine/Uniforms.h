#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

struct UniformBufferObject {
  alignas(16) glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
};
