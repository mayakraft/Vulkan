cmake_minimum_required(VERSION 3.10)

project(VulkanEngine)

set(CMAKE_CXX_STANDARD 17)

# Find Vulkan SDK
find_package(Vulkan REQUIRED)

# Find GLFW via pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

include_directories(
  ${Vulkan_INCLUDE_DIR}
  ${GLFW_INCLUDE_DIRS}
  .
)

link_directories(
  ${Vulkan_LIBRARY_DIRS}
)

add_executable(VulkanEngine
  Device.cpp
  SwapChain.cpp
  Pipeline.cpp
  Renderer.cpp
  Engine.cpp
  main.cpp
)

target_link_libraries(VulkanEngine
  ${Vulkan_LIBRARIES}
  ${GLFW_STATIC_LIBRARIES}
)
