#pragma once

class engine;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include "Object.h"
#include "Debug.h"
#include "EngineWindow.h"
#include "Devices.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "Buffers.h"
#include "Sync.h"

#include "InputHandling.h"




class engine
{

	friend class frames;

public:
	// public members

	std::vector<Object> objects{};

	bool framebufferResized = false;

	// private members
private:

	bool objectsChanged = false;

	VkInstance vulkanInstance;

	EngineWindow window;
	Debug debugMessenger;

	Devices devices;
	SwapChain swapChain;

	GraphicsPipeline pipeline;
	Buffers buffers;

	Sync sync;
	InputHandling inputHandle;

	Rectangle rect;


	// public functions
public:

	void initializeEngine();
	void closeEngine();

	void addObject(Object = Object{});
	void removeObject(size_t);
	void updateObject(Object, int);

	void mainLoop();

	static void frameBufferResizedCallback(GLFWwindow*, int, int);

	std::vector<int> objectRemoveQueue{};
	std::map<int, Object> objectUpdateQueue{};

	// private functions
private:


	void createVulkanInstance();
	void setupEnginePointer();
	void setupInput();

	std::vector<const char*> getRequiredExtensions();

	void rebuildSwapChain();
	void drawFrame();
	void processFrame();
	


	// callback functions
public:

	static void mouseButtonCallback(GLFWwindow*, int, int, int);
	static void keyCallback(GLFWwindow*, int, int, int, int);


};


