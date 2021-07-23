#pragma once

#include "BaseObject.h"

class bouncingRectangle : public BaseObject
{

	glm::vec2 direction = {0,0};

	void process(double) override;
	void onReady() override;

};

