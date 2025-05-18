include .env

CXX = g++
TARGET_DIR = bin
BUILD_DIR = build
TARGET = $(TARGET_DIR)/app

# which example
EXAMPLE ?= viking_room
EXAMPLE_SRC = examples/$(EXAMPLE)/main.cpp

ENGINE_SOURCES := $(shell find engine -name '*.cpp')
SOURCES := $(ENGINE_SOURCES) $(EXAMPLE_SRC)

# corresponding .o paths in build/ dir
OBJECTS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

# INCLUDES = -Iengine -I. -Ilib -I${VULKAN_SDK}/macOS/include `pkg-config --cflags-only-I glfw3`
INCLUDES = -Iengine -I${VULKAN_SDK}/macOS/include `pkg-config --cflags-only-I glfw3`
LDFLAGS = -L${VULKAN_SDK}/macOS/lib -lvulkan `pkg-config --static --libs glfw3`

# Debug/release
# default to debug build
BUILD ?= debug

# run either by writing `make`, `make BUILD=debug` or `make BUILD=release`
ifeq ($(BUILD),debug)
  CXXFLAGS = -std=c++17 -g -O0 -DDEBUG $(INCLUDES)
else ifeq ($(BUILD),release)
  CXXFLAGS = -std=c++17 -O3 -DNDEBUG $(INCLUDES)
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(TARGET_DIR)
	# $(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run:
	DYLD_LIBRARY_PATH=$(VULKAN_SDK)/macOS/lib ./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET_DIR)/app

