#pragma once

#include <string>
#include <algorithm>

#include <glm/glm.hpp>

class CollisionBody
{
public:

	enum class shapeNames {
		Circle, Rectangle
	};

	int shape = -1;
	glm::vec3 position{};

	virtual bool isColliding(CollisionBody*) = 0;

	void* owner;
};


namespace Body2D {
	
	class Circle : public CollisionBody {
	
	public:
		double radius = 0;
		
		Circle() { shape = (int)shapeNames::Circle; }
		bool isColliding(CollisionBody*) override;
	};

	class Rectangle : public CollisionBody {
	public:
		glm::vec2 keyPoints[2]{};
	
		Rectangle() { shape = (int)shapeNames::Rectangle; }
		bool isColliding(CollisionBody*) override;

		glm::vec2 getMax();
		glm::vec2 getMin();
	};

}



