#include "CollisionBody.h"




// Body2D -------------------------


// Circle

bool Body2D::Circle::isColliding(CollisionBody* compBody) {
	switch (compBody->shape)
	{
	case (int)shapeNames::Circle: {
		Circle* body = (Circle*)compBody;
		double r2 = pow(radius + body->radius, 2);
		double distance2 = pow(position.x + body->position.x, 2) + pow(position.y + body->position.y, 2);
		return r2 > distance2;
	}
	case (int)shapeNames::Rectangle: {
		Rectangle* body = (Rectangle*)compBody;
		glm::vec2 compPoints[2] = { body->getMin(), body->getMax() };
		glm::vec2 closest{};
		closest.x = std::clamp(position.x, compPoints[0].x, compPoints[1].x);
		closest.y = std::clamp(position.y, compPoints[0].y, compPoints[1].y);
		double r2 = pow(radius, 2);
		double distance2 = pow(position.x + closest.x, 2) + pow(position.y + closest.y, 2);
		return r2 > distance2;
	}
	default:
		return false;
	}
}


// Rectangle

bool Body2D::Rectangle::isColliding(CollisionBody* compBody) {
	switch (compBody->shape)
	{
	case (int)shapeNames::Circle: {
		Circle* body = (Circle*)compBody;
		glm::vec2 thisPoints[2] = { getMin(), getMax() };
		glm::vec2 closest{};
		closest.x = std::clamp(position.x, thisPoints[0].x, thisPoints[1].x);
		closest.y = std::clamp(position.y, thisPoints[0].y, thisPoints[1].y);
		double r2 = pow(body->radius, 2);
		double distance2 = pow(body->position.x + closest.x, 2) + pow(body->position.y + closest.y, 2);
		return r2 > distance2;
	}
	case (int)shapeNames::Rectangle: {
		Rectangle* body = (Rectangle*)compBody;
		glm::vec2 thisPoints[2] = { getMin(), getMax() };
		glm::vec2 compPoints[2] = { body->getMin(), body->getMax() };
		if (thisPoints[0].x > compPoints[1].x || thisPoints[1].x < compPoints[0].x) { return false; }
		if (thisPoints[0].y > compPoints[1].y || thisPoints[1].y < compPoints[0].y) { return false; }
		return true;
	}
	default:
		return false;
	}
}


glm::vec2 Body2D::Rectangle::getMin() {
	glm::vec2 min{};
	if (keyPoints[0].x < keyPoints[1].x) { min.x = keyPoints[0].x + position.x; }
	else { min.x = keyPoints[1].x + position.x; }
	if (keyPoints[0].y < keyPoints[1].y) { min.y = keyPoints[0].y + position.y; }
	else { min.y = keyPoints[1].y + position.y; }
	return min;
}

glm::vec2 Body2D::Rectangle::getMax() {
	glm::vec2 max{};
	if (keyPoints[0].x > keyPoints[1].x) { max.x = keyPoints[0].x + position.x; }
	else { max.x = keyPoints[1].x + position.x; }
	if (keyPoints[0].y > keyPoints[1].y) { max.y = keyPoints[0].y + position.y; }
	else { max.y = keyPoints[1].y + position.y; }
	return max;
}

