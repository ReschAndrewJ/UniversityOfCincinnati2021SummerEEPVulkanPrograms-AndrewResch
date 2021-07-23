#include "bouncingRectangle.h"



void bouncingRectangle::process(double timeDiff) {
	bool flipX = false;
	bool flipY = false;
	for (const Spatial::Vertex& vert : object.vertices) {
		if (vert.position.x + position.x >= 1) { direction.x = -abs(direction.x); flipX = true;	}
		if (vert.position.x + position.x <= -1) { direction.x = abs(direction.x); flipX = true; }
		if (vert.position.y + position.y >= 1) { direction.y = -abs(direction.y); flipY = true; }
		if (vert.position.y + position.y <= -1) { direction.y = abs(direction.y); flipY = true; }
		if (flipX && flipY) { break; }
	}
	
	position += glm::vec2{direction.x * timeDiff * 0.75, direction.y * timeDiff * 0.75};
	updateTransform();
}


void bouncingRectangle::onReady() {
	Spatial::Rectangle rect = Spatial::Rectangle(0.5, 0.5);
	rect.color = { static_cast<float>(rand() % 256) / 255, 
		static_cast<float>(rand() % 256) / 255, 
		static_cast<float>(rand() % 256) / 255 };

	object = rect.convertToObject();
	position = { 0.0, 0.0 };

	float rads = float(rand() % 360) / 180 * (4 * atan(1));
	direction = { cos(rads), sin(rads) };
	
}


void bouncingRectangle::updateTransform() {
	Shaders::transformationExpand expanded{};
	expanded.scale = glm::mat4(1.0f);
	expanded.rotation = glm::mat4(1.0f);
	expanded.translation = glm::mat3(1.0f);
	expanded.translation[3] = { position.x, position.y, 0, 1 };

	pushTransform = expanded.compact();
}



std::vector<VkPushConstantRange> bouncingRectangle::setPushConstantRanges() {
	VkPushConstantRange pushTransformRange{};
	pushTransformRange.offset = 0;
	pushTransformRange.size = sizeof(Shaders::transformationCompact);
	pushTransformRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	
	std::vector<VkPushConstantRange> pushConstantRanges{ pushTransformRange };
	return pushConstantRanges;
}

void bouncingRectangle::sendPushConstants(VkCommandBuffer cmdBuffer) {
	vkCmdPushConstants(cmdBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT,
		0, sizeof(Shaders::transformationCompact), &pushTransform);
}

