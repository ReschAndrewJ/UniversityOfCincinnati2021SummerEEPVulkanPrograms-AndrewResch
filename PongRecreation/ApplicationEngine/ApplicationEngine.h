#pragma once

#include "../GraphicsEngine/GraphicsEngine.h"
#include "../ProcessEngine/ProcessEngine.h"
#include "../Objects/GenericObjects/GenericObjects.h"
#include "../Objects/SpecificObjects/SpecificObjects.h"
#include "PassInfo.h"

#include <thread>

class ApplicationEngine
{
	std::string appName;

	GraphicsEngine* gEngine{};
	ProcessEngine* pEngine{};

	BaseObject* root{};

	PassInfo* passData{};



public:


	static void gEngineFramebufferResizedCallback(GLFWwindow*, int width, int height);
	static void pEngineKeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);


	void startApp(std::string name, BaseObject* topObject);
	void threadApp();
	void closeApp();


private:


	void setGLFWCallbacks();

	static void GraphicsThread(ApplicationEngine* app);
	static void ProcessThread(ApplicationEngine* app);


};

