include .env

CXX = g++
TARGET = bin/app

# ENGINE_SOURCES = $(wildcard engine/*.cpp)
# SOURCES = $(ENGINE_SOURCES) main.cpp
ROOT_FOLDER_SOURCES = $(wildcard engine/*.cpp)
SUB_FOLDERS_SOURCES = $(wildcard engine/**/*.cpp)
SOURCES = $(ROOT_FOLDER_SOURCES) $(SUB_FOLDERS_SOURCES) main.cpp

OBJECTS = $(SOURCES:.cpp=.o)
INCLUDES = -Iengine -I${VULKAN_SDK}/macOS/include `pkg-config --cflags-only-I glfw3`
LDFLAGS = -L${VULKAN_SDK}/macOS/lib -lvulkan `pkg-config --static --libs glfw3`

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
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run:
	DYLD_LIBRARY_PATH=$(VULKAN_SDK)/macOS/lib ./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)
