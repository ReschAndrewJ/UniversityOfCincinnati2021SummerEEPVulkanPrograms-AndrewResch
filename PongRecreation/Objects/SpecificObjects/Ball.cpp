#include "Ball.h"


void Ball::preReady() {
	createGraphicsObject("Shaders/Spv/PongVert.spv", "Shaders/Spv/PongFrag.spv");

	collisionArea = new CollisionShape;
	collisionArea->owner = this;
	collisionArea->area = new Circle(0.05);
	passData->collisionAreas.push_back(collisionArea);

	std::vector<SharedStructs::pushInfo> pushValues(1);
	pushValues[0].offset = 0;
	pushValues[0].size = sizeof(transformation);
	pushValues[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushValues[0].values = &transformation;
	sendPushConstants(pushValues);

}


void Ball::Ready() {

	velocity = 0.7;
	position = { 0,0 };
	collisionArea->position = position;

	direction.x = pow(-1, rand() % 2);
	direction.y = ((double)rand()) / RAND_MAX * 2 - 1;
	double magnitude = sqrt(pow(direction.x, 2) + pow(direction.y, 2));
	direction.x /= magnitude;
	direction.y /= magnitude;

}

void Ball::processFrame(double timeDiff) {
	
	if (position.y + (*(Rectangle*)collisionArea->area).height/2 > 1) { direction.y = -abs(direction.y); }
	if (position.y - (*(Rectangle*)collisionArea->area).height / 2 < -1) { direction.y = abs(direction.y); }
	if (abs(position.x) - (*(Rectangle*)collisionArea->area).width / 2 > 1) { position.x = 0; direction.x *= -1; }

	position.x += ((double)direction.x * velocity) * timeDiff;
	position.y += ((double)direction.y * velocity) * timeDiff;

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


void Ball::resolveCollision(CollidingObject2D* collider) {
	if (collider->collisionArea->area->name != (int)Shapes::Rectangle) { return; }
	Rectangle colRect = *(Rectangle*)collider->collisionArea->area;
	Circle colCircle = *(Circle*)collisionArea->area;


	double overlapX = colRect.width + colCircle.radius - abs(position.x - collider->collisionArea->position.x);
	double overlapY = colRect.height + colCircle.radius - abs(position.y - collider->collisionArea->position.y);

	if (overlapX > 0) {
		if (overlapY > overlapX) {
			if ((collider->collisionArea->position.x - position.x) * direction.x > 0) {
				direction.x *= -1;
			}
		}
		else {
			if ((collider->collisionArea->position.y - position.y) * direction.y > 0) {
				direction.y *= -1;
			}
		}
	}
	else {
		if ((collider->collisionArea->position.y - position.y) * direction.y > 0) {
			direction.y *= -1;
		}
	}

}


// ----------------------------------------


void Ball::sendVertexBindingDescription(VkVertexInputBindingDescription& bindingDescription) {
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(glm::vec2);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

void Ball::sendVertexAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions) {
	attributeDescriptions.resize(1);
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = 0;
}

void Ball::sendPushConstantsRanges(std::vector<VkPushConstantRange>& pushConstRanges) {
	VkPushConstantRange pushPositionRange{};
	pushPositionRange.offset = 0;
	pushPositionRange.size = sizeof(glm::mat4);
	pushPositionRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	pushConstRanges = std::vector{ pushPositionRange };
}


SharedStructs::verticesData Ball::sendVertexData() {
	SharedStructs::verticesData vData{};
	vData.values = vertices.data();
	vData.bufferSize = sizeof(vertices[0]) * vertices.size();
	return vData;
}

SharedStructs::indicesData Ball::sendIndicesData() {
	SharedStructs::indicesData iData;
	iData.values = indices.data();
	iData.bufferSize = sizeof(indices[0]) * indices.size();
	iData.indexCount = indices.size();
	return iData;
}

