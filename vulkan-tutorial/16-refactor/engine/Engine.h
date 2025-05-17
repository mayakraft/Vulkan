#pragma once

#include <GLFW/glfw3.h>
#include "Debug.h"
#include "Device.h"
#include "memory/Buffers.h"
#include "SwapChain.h"
/*#include "Pipeline.h"*/
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
  Buffers* buffers;
  SwapChain* swapChain;
  /*Pipeline* pipeline;*/
  Renderer* renderer;

  bool framebufferResized = false;

 	const char *appName = "Vulkan App";
  const char *engineName = "Vulkan Engine";
  const uint32_t WIDTH = 512;
  const uint32_t HEIGHT = 512;
};

