#pragma once

class GraphicsPipeline;

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>
#include <vector>

#include "Object.h"
#include "FileUtil.h"

class GraphicsPipeline
{

	// public members
public:

	VkRenderPass renderPass;
	std::vector<VkPipelineLayout> pipelineLayouts{};
	std::vector<VkPipeline> pipelines{};
	


	// public funtions
public:

	void createRenderPass(VkDevice, VkFormat);
	void createGraphicsPipeline(VkDevice, VkExtent2D, int = -1);

	void destroyPipeline(VkDevice, int);
	void destroyRenderPass(VkDevice);


	// private functions
private:

	VkShaderModule createShaderModule(VkDevice, const std::vector<char>&);

};

