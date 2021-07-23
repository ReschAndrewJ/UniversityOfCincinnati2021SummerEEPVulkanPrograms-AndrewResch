#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <array>

#include "Spatial.h"
#include "FileUtil.h"

class GraphicalObject
{
	friend class GraphicsEngine;

private:

	// Engine Friended Members

	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;

	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;

	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory indexBufferMemory;

protected:

	// Protected Members

	std::string vertexShaderFilepath = "shaders/vert.spv";
	std::string fragmentShaderFilepath = "shaders/frag.spv";

public:

	// Members

	Spatial::Object object;

private:

	VkShaderModule createShaderModule(VkDevice, const std::vector<char>&);


	// Engine Friended Functions

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();

	static void createPipelines(VkDevice, VkRenderPass, VkExtent2D, std::vector<GraphicalObject*>&);
	void destroyPipeline(VkDevice);

};

