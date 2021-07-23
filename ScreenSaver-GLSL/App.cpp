#include "App.h"


void App::setRoot(BaseObject* obj) {
	if (rootObject != nullptr) {
		App::pushToRemoveVector(rootObject->getObjectTree(), this);
	}
	rootObject = obj;
	rootObject->parentApp_addObjectFunc = &pushToAddVector;
	rootObject->parentApp_removeObjectFunc = &pushToRemoveVector;
	rootObject->parentApp_updateObjectFunc = &pushToUpdateVector;
	rootObject->parentApp = this;
	App::pushToAddVector(rootObject, this);
}

void App::startApp() {
	srand(time(0));

	engine.initializeEngine("ScreenSaver");
	previousFrameStart = std::chrono::steady_clock::now();

}

void App::endApp() {
	if (rootObject != nullptr) {
		objectsToRemove = rootObject->getObjectTree();
	}
	removeObjects();
	engine.closeEngine();
}


void App::mainLoop() {
	while (!engine.windowShouldClose()) { 
		glfwPollEvents();

		processFrame();

		std::vector<BaseObject*> tree{};
		std::vector<GraphicalObject*> graphicsObjects{};
		if (rootObject != nullptr) {
			tree = rootObject->getObjectTree();
			graphicsObjects = std::vector<GraphicalObject*>(tree.begin(), tree.end());
		}

		if (objectsToRemove.size() > 0) { removeObjects(); }
		if (objectsToAdd.size() > 0) { addObjects(); }

		if (objectsToUpdate.size() > 0) {
			std::vector<GraphicalObject*> objs = std::vector<GraphicalObject*>(objectsToUpdate.begin(), objectsToUpdate.end());
			engine.updateObjects(objs);
			objectsToUpdate.clear();
		}

		engine.drawFrame(graphicsObjects);

		engine.waitIdle();
	}
}


void App::processFrame() {
	std::chrono::time_point currentFrameStart = std::chrono::steady_clock::now();
	double timeDiff = std::chrono::duration_cast<std::chrono::duration<double>>(currentFrameStart - previousFrameStart).count();
	previousFrameStart = currentFrameStart;

	if (rootObject != nullptr) {
		for (BaseObject* node : rootObject->getObjectTree()) {
			node->process(timeDiff);
		}
	}
}



void App::addObjects() {
	std::vector<GraphicalObject*> graphicalObjects(objectsToAdd.begin(), objectsToAdd.end());
	engine.createObjects(graphicalObjects);
	objectsToAdd.clear();
}


void App::removeObjects() {
	std::sort(objectsToRemove.begin(), objectsToRemove.end());
	std::vector<BaseObject*>::iterator it =	std::unique(objectsToRemove.begin(), objectsToRemove.end());
	objectsToRemove.resize(std::distance(objectsToRemove.begin(), it));
	std::vector<GraphicalObject*> graphicsObjects(objectsToRemove.begin(), objectsToRemove.end());
	engine.destroyObjects(graphicsObjects);
	for (BaseObject* obj : objectsToRemove) {
		delete obj;
	}
	objectsToRemove.clear();
}



// Add And Remove Handles

void App::pushToAddVector(BaseObject* obj, void* app) {
	reinterpret_cast<App*>(app)->objectsToAdd.push_back(obj);
	obj->onAdd();
}


void App::pushToRemoveVector(std::vector<BaseObject*> objects, void* app) {
	reinterpret_cast<App*>(app)->objectsToRemove.insert(
		reinterpret_cast<App*>(app)->objectsToRemove.end(), objects.begin(), objects.end());
}


void App::pushToUpdateVector(BaseObject* obj, void* app) {
	reinterpret_cast<App*>(app)->objectsToUpdate.push_back(obj);
}
