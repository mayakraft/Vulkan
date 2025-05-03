#include "Engine.h"
#include <stdexcept>

Engine::Engine() {
  initWindow();

  device = new Device(window, appName, appName);
  swapChain = new SwapChain(*device);
  buffers = new Buffers(*device);
  pipeline = new Pipeline(*device, *swapChain, *buffers);
  renderer = new Renderer(*device, *swapChain, *buffers, *pipeline);
}

Engine::~Engine() {
  delete renderer;
  delete pipeline;
  delete buffers;
  delete swapChain;
  delete device;

  glfwDestroyWindow(window);
  glfwTerminate();
}

void Engine::startLoop() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    renderer->drawFrame();
  }

  vkDeviceWaitIdle(device->getDevice());
}

void Engine::initWindow() {
  if (!glfwInit()) {
    throw std::runtime_error("failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, appName, nullptr, nullptr);

  if (!window) {
    throw std::runtime_error("failed to create window");
  }
}
