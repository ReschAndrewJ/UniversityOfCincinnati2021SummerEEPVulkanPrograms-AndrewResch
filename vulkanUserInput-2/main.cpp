
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

	app.mainLoop();
	app.closeEngine();
}


