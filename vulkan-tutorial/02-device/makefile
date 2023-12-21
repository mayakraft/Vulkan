include .env

CFLAGS = -std=c++17 -I. -I${VULKAN_SDK_PATH}/include `pkg-config --cflags-only-I glfw3`
LDFLAGS = -L${VULKAN_SDK_PATH}/lib -lvulkan `pkg-config --static --libs glfw3`

a.out: *.hpp *.cpp
	g++ ${CFLAGS} -o a.out *.cpp ${LDFLAGS}

clean:
	rm -f a.out
