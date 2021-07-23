#pragma once

#include "../Objects-Generic/BaseObject.h"

class bouncingRectangle : public BaseObject
{
public:

	glm::vec2 position = { 0,0 };
	glm::vec2 direction = {0,0};

	void process(double) override;
	void onReady() override;

	Shaders::transformationCompact pushTransform;

	std::vector<VkPushConstantRange> setPushConstantRanges() override;
	void sendPushConstants(VkCommandBuffer) override;

	void updateTransform();

	bouncingRectangle() { vertexShaderFilepath = "shaders/vert.spv"; }

};

