#pragma once

#include "glm/glm.hpp"

#include "../Util/ShapesUtil.h"

#include <algorithm>

class CollisionShape
{

public:

	Shape2D* area;
	glm::vec2 position{};
	void* owner;

	static bool areOverlapping(CollisionShape* object1, CollisionShape* object2);




};

