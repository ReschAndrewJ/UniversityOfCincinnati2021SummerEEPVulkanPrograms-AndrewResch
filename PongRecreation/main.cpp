#include "ApplicationEngine/ApplicationEngine.h"



int main() {
	srand(time(0));

	ApplicationEngine app{};
	std::cout << "Starting App" << std::endl;
	app.startApp("Pong", new Game);
	return 1;
}