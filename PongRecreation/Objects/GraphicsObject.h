#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <vector>
#include <unordered_map>

#include "../Util/FileUtil.h"
#include "../Util/SharedStructs.h"

class GraphicsObject
{
	friend class GraphicsEngine;
	
	VkPipeline graphicsPipeline;
	VkPipelineLayout graphicsPipelineLayout;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorSetPool;
	std::vector<VkDescriptorSet> descriptorSets;

	std::string vertexShaderFilepath;
	std::string fragmentShaderFilepath;

	std::unordered_map<int, SharedStructs::pushInfo> pushCostants;

	SharedStructs::bufferSpace vertexMemoryInfo;
	SharedStructs::bufferSpace indexMemoryInfo;

public:

	void* ownerObject;

	SharedStructs::verticesData(*getVertexData)(void* owner);
	SharedStructs::indicesData(*getIndicesData)(void* owner);

	void(*getVertexBindingDescription)(void* owner, VkVertexInputBindingDescription& bindingDescs);
	void(*getVertexAttributeDescriptions)(void* owner, std::vector<VkVertexInputAttributeDescription>& attrDescs);
	void(*getDescriptorSetLayoutBindings)(void* owner, std::vector<VkDescriptorSetLayoutBinding>& descSetLayoutBindings);
	void(*getPushConstantsRanges)(void* owner, std::vector<VkPushConstantRange>& pushConstRanges);

	void setVertexShaderFilepath(std::string filepath) { vertexShaderFilepath = filepath; }
	void setFragmentShaderFilePath(std::string filepath) { fragmentShaderFilepath = filepath; }

	void setPushConstants(const std::vector<SharedStructs::pushInfo>& constants);


private:

	void createDescriptorSetLayout(VkDevice logicalDevice);

	static void createGraphicsPipelines(VkDevice logicalDevice, VkRenderPass renderPass,
		VkExtent2D extent, std::vector<GraphicsObject*> graphicsObjects);

};

