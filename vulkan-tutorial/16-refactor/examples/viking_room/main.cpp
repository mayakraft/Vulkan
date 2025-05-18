#include <iostream>
#include <stdexcept>
#include "../../engine/Engine.h"

int main() {
	try {
		auto engine = Engine{};
		engine.startLoop();
	} catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
