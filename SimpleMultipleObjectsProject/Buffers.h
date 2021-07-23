#pragma once

class Buffers;

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>
#include<vector>

#include "Object.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"


class Buffers
{

	// public members
public:

	std::vector<VkFramebuffer> swapChainFrameBuffers{};
	std::vector<VkCommandBuffer> commandBuffers{};
	std::vector<VkBuffer> vertexBuffers {};
	std::vector<VkDeviceMemory> vertexBuffersMemory{};
	std::vector<VkBuffer> indexBuffers{};
	std::vector<VkDeviceMemory> indexBuffersMemory{};

	VkCommandPool commandPool;


	// public functions
public:

	void createFrameBuffers(VkDevice, SwapChain&, GraphicsPipeline&);
	void createCommandPool(Devices&);
	void createVertexBuffer(Devices&, SwapChain&, const Object&);
	void createIndexBuffer(Devices&, const Object&);
	void createCommandBuffers(Devices&, SwapChain&, GraphicsPipeline&, const std::vector<Object>&);
	
	void recordCommandBuffers(SwapChain&, GraphicsPipeline&, const std::vector<Object>&);

	
	void destroyCommandPool(VkDevice);
	void destroyFrameBuffers(VkDevice);
	void destroyCommandBuffers(VkDevice);
	void destroyObjectBuffers(VkDevice, size_t);


	// private functions
private:
	uint32_t findMemoryType(VkPhysicalDevice, uint32_t, VkMemoryPropertyFlags);

	void createBuffer(Devices, VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, 
		VkBuffer&, VkDeviceMemory&);
	void copyBuffer(Devices, VkBuffer, VkBuffer, VkDeviceSize);

};


