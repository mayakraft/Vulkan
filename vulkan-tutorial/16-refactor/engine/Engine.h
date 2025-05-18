#pragma once

#include <GLFW/glfw3.h>
#include "Debug.h"
#include "core/Device.h"
#include "memory/Buffers.h"
#include "core/SwapChain.h"
#include "render/Renderer.h"

class Engine {
public:
  Engine();
  ~Engine();

  void startLoop();

private:
  void initWindow();

  GLFWwindow* window;

  Device* device;
  Buffers* buffers;
  SwapChain* swapChain;
  Renderer* renderer;

  bool framebufferResized = false;

 	const char *appName = "Vulkan App";
  const char *engineName = "Vulkan Engine";
  const uint32_t WIDTH = 512;
  const uint32_t HEIGHT = 512;
};

