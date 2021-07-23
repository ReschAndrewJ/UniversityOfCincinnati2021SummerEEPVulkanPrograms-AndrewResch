#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <array>

#include "ShaderStructs.h"
#include "Spatial.h"
#include "FileUtil.h"

class GraphicalObject
{
	friend class GraphicsEngine;

private:

	// Private/Engine-Friended Members

	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;

	VkPipeline pipeline;

	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory indexBufferMemory;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

protected:

	// Protected Members

	std::string vertexShaderFilepath = "shaders/vert.spv";
	std::string fragmentShaderFilepath = "shaders/frag.spv";

	VkPipelineLayout pipelineLayout;

public:

	// Members

	Spatial::Object object;

private:

	VkShaderModule createShaderModule(VkDevice, const std::vector<char>&);
	void createDescriptorSetLayout(VkDevice);


	// Engine Friended Functions

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();

	static void createPipelines(VkDevice, VkRenderPass, VkExtent2D, std::vector<GraphicalObject*>&);
	void destroyPipeline(VkDevice);


protected:

	virtual std::vector<VkDescriptorSetLayoutBinding> setDescriptorLayoutBindings() { return std::vector<VkDescriptorSetLayoutBinding>(); };
	virtual std::vector<VkPushConstantRange> setPushConstantRanges() { return std::vector<VkPushConstantRange>(); }
	virtual void sendPushConstants(VkCommandBuffer) {}

};

