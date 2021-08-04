#include "ApplicationEngine.h"


void ApplicationEngine::closeApp() {

	root->free();
	gEngine->shutDown();
	delete gEngine;
	delete pEngine;
	delete passData;

}
