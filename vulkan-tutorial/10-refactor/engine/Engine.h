#pragma once

#include <GLFW/glfw3.h>
#include "Device.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "Renderer.h"

class Engine {
public:
  Engine();
  ~Engine();

  void startLoop();

private:
  void initWindow();

  GLFWwindow* window;

  Device* device;
  SwapChain* swapChain;
  Pipeline* pipeline;
  Renderer* renderer;

  bool framebufferResized = false;

 	const char *appName = "Vulkan Graphics";
  const uint32_t WIDTH = 512;
  const uint32_t HEIGHT = 512;
};
