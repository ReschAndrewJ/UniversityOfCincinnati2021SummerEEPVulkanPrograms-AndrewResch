
#include <time.h>
#include <random>

#include "engine.h"

engine app;

void run();
void inputLoop(Object*);

int main() {

	srand(time(NULL));

	try {
		run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


void run() {
	app.initializeEngine();

	Object obj{};
	obj.vertices = {
	{{-0.2f, -0.2f}, {0.2f, 0.2f, 0.7f}},
	{{0.2f, -0.2f}, {0.2f, 0.2f, 0.7f}},
	{{0.2f, 0.2f}, {0.2f, 0.7f, 0.2f}},
	{{-0.2f, 0.2f}, {0.02f, 0.7f, 0.2f}}
	};
	obj.indices = { 0, 1, 2, 2, 3, 0};
	app.addObject(obj);

	app.mainLoop();
	app.closeEngine();
}


