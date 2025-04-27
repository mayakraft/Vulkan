include .env

CXX = g++
CXXFLAGS = -std=c++17 -Iengine -I${VULKAN_SDK}/macOS/include `pkg-config --cflags-only-I glfw3`
LDFLAGS = -L${VULKAN_SDK}/macOS/lib -lvulkan `pkg-config --static --libs glfw3`
ENGINE_SOURCES = $(wildcard engine/*.cpp)
SOURCES = $(ENGINE_SOURCES) main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = bin/app

all: $(TARGET)

# for debugging, add -g

$(TARGET): $(OBJECTS)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run:
	DYLD_LIBRARY_PATH=$(VULKAN_SDK)/macOS/lib ./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)
