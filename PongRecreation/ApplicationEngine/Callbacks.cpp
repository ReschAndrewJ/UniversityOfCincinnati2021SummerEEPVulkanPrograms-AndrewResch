#include "ApplicationEngine.h"


void ApplicationEngine::gEngineFramebufferResizedCallback(GLFWwindow* window,
	int width, int height) {
	GraphicsEngine* gEngine = ((ApplicationEngine*)glfwGetWindowUserPointer(window))->gEngine;
	GraphicsEngine::framebufferResizedCallback(gEngine);
}

// ------------------------------------------


void ApplicationEngine::setGLFWCallbacks() {
	glfwSetFramebufferSizeCallback(gEngine->getWindowPointer(), gEngineFramebufferResizedCallback);
	glfwSetKeyCallback(gEngine->getWindowPointer(), pEngineKeyCallback);
}


void ApplicationEngine::pEngineKeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
	ProcessEngine* pEngine = ((ApplicationEngine*)glfwGetWindowUserPointer(window))->pEngine;
	pEngine->keyCallback(key, action);
}


