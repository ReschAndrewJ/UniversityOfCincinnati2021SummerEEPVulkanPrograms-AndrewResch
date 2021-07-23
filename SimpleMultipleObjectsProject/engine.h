#pragma once

class engine;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "Object.h"
#include "Debug.h"
#include "EngineWindow.h"
#include "Devices.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "Buffers.h"
#include "Sync.h"


class engine
{

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


	// public functions
public:

	void initializeEngine();
	void closeEngine();

	void addObject(Object = Object{});
	void removeObject(size_t);

	void mainLoop();

	static void frameBufferResizedCallback(GLFWwindow*, int, int);


	// private functions
private:


	void createVulkanInstance();
	void setupEnginePointer();

	std::vector<const char*> getRequiredExtensions();

	void drawFrame();
	void rebuildSwapChain();

	

};


