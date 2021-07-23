#include "Engine/App.h"

#include <iostream>

int main() {

	App app{};
	
	app.startApp();
	BaseObject* BO = new BaseObject;
	app.setRoot(BO);
	BO->addChildObject(new collidingRectangle);
	BO->addChildObject(new collidingRectangle);
	app.mainLoop();
	app.endApp();

	return 0;
}