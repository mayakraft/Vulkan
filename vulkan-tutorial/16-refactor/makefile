include .env

# Usage:
# run with `make`
# options:
# make BUILD=debug
# make BUILD=release
# make EXAMPLE=viking_room
# make EXAMPLE=triangle

CXX = g++
AR = ar
BUILD_DIR = build
TARGET = $(BUILD_DIR)/app
LIB_DIR = lib
STATIC_LIB = $(LIB_DIR)/libvulkan_engine.a

# Which example to build
EXAMPLE ?= viking_room
EXAMPLE_SOURCES = examples/$(EXAMPLE)/main.cpp
EXAMPLE_OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(EXAMPLE_SOURCES))

# Engine source files
ENGINE_SOURCES := $(shell find engine -name '*.cpp')
ENGINE_OBJECTS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(ENGINE_SOURCES))

OBJECTS = $(ENGINE_OBJECTS) $(EXAMPLE_OBJECTS)
INCLUDES = -Iengine -I${VULKAN_SDK}/macOS/include `pkg-config --cflags-only-I glfw3`
LDFLAGS = -L${VULKAN_SDK}/macOS/lib -lvulkan `pkg-config --static --libs glfw3`

# build or release
BUILD ?= debug
ifeq ($(BUILD),debug)
  CXXFLAGS = -std=c++17 -g -O0 -DDEBUG $(INCLUDES)
else ifeq ($(BUILD),release)
  CXXFLAGS = -std=c++17 -O3 -DNDEBUG $(INCLUDES)
endif

all: $(STATIC_LIB) $(TARGET)

# Build static library from engine sources
$(STATIC_LIB): $(ENGINE_OBJECTS)
	@mkdir -p $(LIB_DIR)
	$(AR) rcs $@ $^

# Link final app with static lib
$(TARGET): $(EXAMPLE_OBJECTS) $(STATIC_LIB)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile cpp files to build/ path
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run:
	DYLD_LIBRARY_PATH=$(VULKAN_SDK)/macOS/lib ./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(LIB_DIR) $(TARGET)

print:
	@echo ENGINE_SOURCES=$(ENGINE_SOURCES)
	@echo EXAMPLE_SOURCES=$(EXAMPLE_SOURCES)
	@echo OBJECTS=$(OBJECTS)

