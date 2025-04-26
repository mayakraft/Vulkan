include .env

CXX = g++
SOURCES = Device.cpp SwapChain.cpp Pipeline.cpp Renderer.cpp Engine.cpp main.cpp
CXXFLAGS = -std=c++17 -I. -I${VULKAN_SDK_PATH}/include `pkg-config --cflags-only-I glfw3`
LDFLAGS = -L${VULKAN_SDK_PATH}/lib -lvulkan `pkg-config --static --libs glfw3`
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = app

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)
