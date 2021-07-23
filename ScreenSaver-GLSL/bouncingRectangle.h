#pragma once

#include "BaseObject.h"

class bouncingRectangle : public BaseObject
{

	glm::vec2 position = { 0,0 };
	glm::vec2 direction = {0,0};

	void process(double) override;
	void onReady() override;

	Shaders::transformationCompact pushTransform;

	std::vector<VkPushConstantRange> setPushConstantRanges() override;
	void sendPushConstants(VkCommandBuffer) override;

	void updateTransform();

};

