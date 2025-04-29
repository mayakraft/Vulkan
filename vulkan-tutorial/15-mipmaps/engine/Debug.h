#pragma once

#include <iostream>

#ifndef NDEBUG
  #define DEBUG_LOG(x) std::cout << "[DEBUG] " << x << std::endl
#else
  #define DEBUG_LOG(x)
#endif
