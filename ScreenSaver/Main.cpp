#include "App.h"

#include <iostream>

int main() {

	App app{};
	
	app.startApp();
	app.setRoot(new bouncingRectangle());
	app.mainLoop();
	app.endApp();

	return 0;
}