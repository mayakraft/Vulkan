#include <stdexcept>
#include "Engine.h"

Engine::Engine() {
  initWindow();

  device = new Device(window, appName, engineName);
  buffers = new Buffers(*device);
  swapChain = new SwapChain(*device);
  renderer = new Renderer(*device, *swapChain, *buffers);
}

Engine::~Engine() {
  delete renderer;
  delete swapChain;
  delete buffers;
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

  // OpenGL is enabled by default, disable it, we are using Vulkan
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // swap chain rebuilding has been implemented,
  // it's possible to resize a window now.
  // otherwise, this is how we can force disable window-resizing.
  // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, appName, nullptr, nullptr);

  if (!window) {
    throw std::runtime_error("failed to create window");
  }
}
