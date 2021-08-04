#include "Player2Paddle.h"


void Player2Paddle::preReady() {
	createGraphicsObject("Shaders/Spv/PongVert.spv", "Shaders/Spv/PongFrag.spv");

	collisionArea = new CollisionShape;
	collisionArea->owner = this;
	collisionArea->area = new Rectangle(0.2, 0.4);
	collisionArea->position = position;
	passData->collisionAreas.push_back(collisionArea);

	std::vector<SharedStructs::pushInfo> pushValues(1);
	pushValues[0].offset = 0;
	pushValues[0].size = sizeof(transformation);
	pushValues[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushValues[0].values = &transformation;
	sendPushConstants(pushValues);

}

void Player2Paddle::Ready() {

	velocity = 1;
	position = { 0.9, 0 };
	collisionArea->position = position;

}



void Player2Paddle::processFrame(double timeDiff) {

	if (getInputState("P2_UP") == inputActions::INPUT_JUST_PRESSED ||
		getInputState("P2_UP") == inputActions::INPUT_HELD) {
		position.y -= velocity * timeDiff;
	}
	if (getInputState("P2_DOWN") == inputActions::INPUT_JUST_PRESSED ||
		getInputState("P2_DOWN") == inputActions::INPUT_HELD) {
		position.y += velocity * timeDiff;
	}

	transformation = glm::mat4(1.0f);
	transformation[3] = { position.x, position.y, 0, 1 };

	std::vector<SharedStructs::pushInfo> pushValues(1);
	pushValues[0].offset = 0;
	pushValues[0].size = sizeof(transformation);
	pushValues[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushValues[0].values = &transformation;
	sendPushConstants(pushValues);

	collisionArea->position = position;
}


// ----------------------------------------


void Player2Paddle::sendVertexBindingDescription(VkVertexInputBindingDescription& bindingDescription) {
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(glm::vec2);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

void Player2Paddle::sendVertexAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions) {
	attributeDescriptions.resize(1);
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = 0;
}

void Player2Paddle::sendPushConstantsRanges(std::vector<VkPushConstantRange>& pushConstRanges) {
	VkPushConstantRange pushPositionRange{};
	pushPositionRange.offset = 0;
	pushPositionRange.size = sizeof(glm::mat4);
	pushPositionRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	pushConstRanges = std::vector{ pushPositionRange };
}


SharedStructs::verticesData Player2Paddle::sendVertexData() {
	SharedStructs::verticesData vData{};
	vData.values = vertices.data();
	vData.bufferSize = sizeof(vertices[0]) * vertices.size();
	return vData;
}

SharedStructs::indicesData Player2Paddle::sendIndicesData() {
	SharedStructs::indicesData iData;
	iData.values = indices.data();
	iData.bufferSize = sizeof(indices[0]) * indices.size();
	iData.indexCount = indices.size();
	return iData;
}
