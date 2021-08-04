#include "ApplicationEngine.h"



void ApplicationEngine::startApp(std::string name, BaseObject* topObject) {
	appName = name;

	passData = new PassInfo();
	gEngine = new GraphicsEngine();
	gEngine->startUp(appName, 600, 600, this, passData);
	pEngine = new ProcessEngine();
	pEngine->setup(passData);
	setGLFWCallbacks();
	root = new Root(this, passData);
	root->addChildObject(topObject);
	threadApp();

}


