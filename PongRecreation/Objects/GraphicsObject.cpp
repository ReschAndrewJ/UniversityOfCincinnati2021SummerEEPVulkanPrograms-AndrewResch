#include "GraphicsObject.h"


void GraphicsObject::setPushConstants(const std::vector<SharedStructs::pushInfo>& constants) {
	for (SharedStructs::pushInfo c : constants) {
		pushCostants[c.offset] = c;
	}
}


void GraphicsObject::createDescriptorSetLayout(VkDevice logicalDevice) {
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
	getDescriptorSetLayoutBindings(ownerObject, layoutBindings);

	VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutCreateInfo.bindingCount = (uint32_t)layoutBindings.size();
	layoutCreateInfo.pBindings = layoutBindings.data();

	if (vkCreateDescriptorSetLayout(logicalDevice, &layoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout");
	}
}


void GraphicsObject::createGraphicsPipelines(VkDevice logicalDevice, VkRenderPass renderPass,
	VkExtent2D extent, std::vector<GraphicsObject*> graphicsObjects) {
	size_t gObjectsCount = graphicsObjects.size();

	std::vector<VkGraphicsPipelineCreateInfo> pipelineCreateInfos(gObjectsCount);
	std::vector<VkPipeline> graphicsPipelines(gObjectsCount);
	std::vector<VkShaderModule> vertexShaderModules(gObjectsCount);
	std::vector<VkShaderModule> fragmentShaderModules(gObjectsCount);

	std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions(gObjectsCount);
	std::vector<std::vector<VkVertexInputAttributeDescription>> vertexAttributeDescriptions(gObjectsCount);
	std::vector<std::vector<VkPushConstantRange>> pushConstantsRanges(gObjectsCount);

	for (size_t i = 0; i < graphicsObjects.size(); i++) {
		graphicsPipelines[i] = VK_NULL_HANDLE;

		std::vector<char> vertCode = FileUtil::readFile(graphicsObjects[i]->vertexShaderFilepath);
		VkShaderModuleCreateInfo vertCreateInfo{};
		vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vertCreateInfo.codeSize = vertCode.size();
		vertCreateInfo.pCode = (uint32_t*)vertCode.data();

		if (vkCreateShaderModule(logicalDevice, &vertCreateInfo, nullptr, &vertexShaderModules[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vertex shader module");
		}

		std::vector<char> fragCode = FileUtil::readFile(graphicsObjects[i]->fragmentShaderFilepath);
		VkShaderModuleCreateInfo fragCreateInfo{};
		fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		fragCreateInfo.codeSize = fragCode.size();
		fragCreateInfo.pCode = (uint32_t*)fragCode.data();

		if (vkCreateShaderModule(logicalDevice, &fragCreateInfo, nullptr, &fragmentShaderModules[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create fragment shader module");
		}

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertexShaderModules[i];
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragmentShaderModules[i];
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		graphicsObjects[i]->getVertexBindingDescription(graphicsObjects[i]->ownerObject, vertexBindingDescriptions[i]);
		graphicsObjects[i]->getVertexAttributeDescriptions(graphicsObjects[i]->ownerObject, vertexAttributeDescriptions[i]);

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)vertexAttributeDescriptions[i].size();
		vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDescriptions[i];
		vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions[i].data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)extent.width;
		viewport.height = (float)extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0,0 };
		scissor.extent = extent;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		graphicsObjects[i]->getPushConstantsRanges(graphicsObjects[i]->ownerObject, pushConstantsRanges[i]);
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)pushConstantsRanges[i].size();
		pipelineLayoutInfo.pPushConstantRanges = pushConstantsRanges[i].data();

		if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &graphicsObjects[i]->graphicsPipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline layout");
		}

		pipelineCreateInfos[i] = VkGraphicsPipelineCreateInfo{};
		pipelineCreateInfos[i].sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfos[i].stageCount = 2;
		pipelineCreateInfos[i].pStages = shaderStages;
		pipelineCreateInfos[i].pVertexInputState = &vertexInputInfo;
		pipelineCreateInfos[i].pInputAssemblyState = &inputAssembly;
		pipelineCreateInfos[i].pViewportState = &viewportState;
		pipelineCreateInfos[i].pRasterizationState = &rasterizer;
		pipelineCreateInfos[i].pMultisampleState = &multisampling;
		pipelineCreateInfos[i].pColorBlendState = &colorBlending;
		pipelineCreateInfos[i].layout = graphicsObjects[i]->graphicsPipelineLayout;
		pipelineCreateInfos[i].renderPass = renderPass;
		pipelineCreateInfos[i].subpass = 0;
		pipelineCreateInfos[i].basePipelineHandle = VK_NULL_HANDLE;
	}

	if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, (uint32_t)pipelineCreateInfos.size(), pipelineCreateInfos.data(), nullptr, graphicsPipelines.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipelines");
	}

	for (size_t i = 0; i < graphicsObjects.size(); i++) {
		graphicsObjects[i]->graphicsPipeline = graphicsPipelines[i];
		vkDestroyShaderModule(logicalDevice, vertexShaderModules[i], nullptr);
		vkDestroyShaderModule(logicalDevice, fragmentShaderModules[i], nullptr);
	}

}