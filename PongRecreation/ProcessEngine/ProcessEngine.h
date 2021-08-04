#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "../Objects/GenericObjects/GenericObjects.h"

#include "../ApplicationEngine/PassInfo.h"

#include <chrono>


class ProcessEngine
{
	const int maxFPS = 60;

	std::mutex inputCallbackMutex;

	std::unordered_map<std::string, int> inputStates;
	std::unordered_map<std::string, inputActions::inputStates> statesFromCallback;

	std::chrono::steady_clock::time_point startOfLastFrame;

	PassInfo* passData;

public:

	void setup(PassInfo* sharedPass);

	void keyCallback(int key, int action);

	void processFrame(std::vector<BaseObject*> objectsTree);


private:

	void setupInputs();
	void advanceInputs();

	void processCollisions();


};

