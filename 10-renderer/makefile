include .env

ENGINE := ./engine
SHADERS := ./shaders

CFLAGS = -std=c++17 -I$(VULKAN_SDK_PATH)/include -I. -I${ENGINE}
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib  -lvulkan `pkg-config --static --libs --cflags-only-I glfw3`

vertSources = $(shell find ${SHADERS} -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find ${SHADERS} -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))

TARGET = a.out
$(TARGET): $(vertObjFiles) $(fragObjFiles) *.cpp *.hpp */*.cpp */*.hpp
	g++ $(CFLAGS) -o $(TARGET) *.cpp ${ENGINE}/*.cpp $(LDFLAGS)

%.spv: %
	${GLSLC} $< -o $@

.PHONY: clean

clean:
	rm -f a.out
