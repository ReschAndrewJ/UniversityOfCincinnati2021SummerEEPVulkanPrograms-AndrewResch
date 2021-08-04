#pragma once

#include "../GenericObjects/CollidingObject2D.h"

class Player1Paddle : public CollidingObject2D
{

	glm::vec2 position{};
	float velocity{};


	std::vector<glm::vec2> vertices = {
		{-0.1, -0.2}, {0.1, -0.2},
		{-0.1, 0.2}, {0.1, 0.2},
	};
	std::vector<int> indices = {
		0, 1, 3, 3, 2, 0,
	};
	glm::mat4 transformation = glm::mat4(1);


public:

	void preReady() override;
	void Ready() override;

	void processFrame(double timeDiff) override;


private:

	void sendVertexBindingDescription(VkVertexInputBindingDescription& bindingDesc) override;
	void sendVertexAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attrDescs) override;

	SharedStructs::verticesData sendVertexData() override;
	SharedStructs::indicesData sendIndicesData() override;

	void sendPushConstantsRanges(std::vector<VkPushConstantRange>& pushConstRanges) override;



};

