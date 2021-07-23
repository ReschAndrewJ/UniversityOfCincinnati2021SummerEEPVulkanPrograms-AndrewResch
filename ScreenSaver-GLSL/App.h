#pragma once

#include <chrono>
#include <algorithm>

#include "GraphicsEngine.h"
#include "ObjectClasses-General.h"
#include "ObjectClasses-Specific.h"
#include "Spatial.h"

class App
{

	BaseObject* rootObject;

	GraphicsEngine engine;

	std::chrono::steady_clock::time_point previousFrameStart;

	std::vector<BaseObject*> objectsToAdd;
	std::vector<BaseObject*> objectsToRemove;
	std::vector<BaseObject*> objectsToUpdate;

public:


	void setRoot(BaseObject*);
	void startApp();
	void mainLoop();
	void endApp();

	static void pushToAddVector(BaseObject*, void*);
	static void pushToRemoveVector(std::vector<BaseObject*>, void*);
	static void pushToUpdateVector(BaseObject*, void*);

	
private:
	
	
	void processFrame();
	void addObjects();
	void removeObjects();

};

