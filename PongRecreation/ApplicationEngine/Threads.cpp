#include "ApplicationEngine.h"


void ApplicationEngine::threadApp() {
	
	std::thread gThread(GraphicsThread, this);
	std::thread pThread(ProcessThread, this);

	bool eop = false;
	while (!eop) {
		glfwPollEvents();
		eop = glfwWindowShouldClose(gEngine->getWindowPointer());
		std::lock_guard<std::mutex> eopGuard(passData->eopMutex);
		passData->closeProgram = eop;
	}
	gThread.join();
	pThread.join();
	
	closeApp();
}


void ApplicationEngine::GraphicsThread(ApplicationEngine* app) {
	bool eop = false;
	while (!eop) {
		app->gEngine->createDestroyGraphicsObjects();
		app->gEngine->graphicsFrame();
		
		app->gEngine->waitIdle();
		std::lock_guard<std::mutex> eopGuard(app->passData->eopMutex);
		eop = app->passData->closeProgram;
	}
	
}



void ApplicationEngine::ProcessThread(ApplicationEngine* app) {
	bool eop = false;
	while (!eop) {
		if (app->root != nullptr) {
			app->pEngine->processFrame(app->root->getDescendants());
		}
		std::lock_guard<std::mutex> eopGuard(app->passData->eopMutex);
		eop = app->passData->closeProgram;
	}
}