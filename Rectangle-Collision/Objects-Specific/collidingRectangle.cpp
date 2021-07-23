#include "collidingRectangle.h"

// temporary solution for this program only
#include "../Engine/App.h"

#include "glm/geometric.hpp"

void collidingRectangle::onReady() {
	Spatial::Rectangle rect = Spatial::Rectangle(0.5, 0.5);
	rect.color = { static_cast<float>(rand() % 256) / 255,
		static_cast<float>(rand() % 256) / 255,
		static_cast<float>(rand() % 256) / 255 };

	object = rect.convertToObject();
	position = { 0.0, 0.0 };

	float rads = float(rand() % 360) / 180 * (4 * atan(1));
	direction = { cos(rads), sin(rads) };

	collisionRect = new Body2D::Rectangle(rect.convertToCollisionRectangle());
	collisionRect->owner = this;
	((App*)parentApp)->addCollider(collisionRect);

}


void collidingRectangle::onCollision(CollisionBody* col) {
	if ((col->position.x - collisionRect->position.x) * direction.x <= 0 &&
		(col->position.y - collisionRect->position.y) * direction.y <= 0) {
		return;
	}
	
	switch (col->shape)
	{
	case (int)CollisionBody::shapeNames::Rectangle: {
		Body2D::Rectangle* compShape = (Body2D::Rectangle*)col;
		glm::vec2 thisPoints[2] = { collisionRect->getMin(), collisionRect->getMax() };
		glm::vec2 compPoints[2] = { compShape->getMin(), compShape->getMax() };

		float combinedExtents_x = (thisPoints[1].x - thisPoints[0].x) / 2 +
			(compPoints[1].x - compPoints[0].x) / 2;
		float combinedExtents_y = (thisPoints[1].y - thisPoints[0].y) / 2 +
			(compPoints[1].y - compPoints[0].y) / 2;
		float overlap_x = (combinedExtents_x - abs(compShape->position.x - collisionRect->position.x));
		float overlap_y = (combinedExtents_y - abs(compShape->position.y - collisionRect->position.y));

		if (overlap_x > 0) {
			if (overlap_y > overlap_x) { 
				if ((col->position.x - collisionRect->position.x) * direction.x > 0) {
					direction.x *= -1;
				}
			}
			else { 
				if ((col->position.y - collisionRect->position.y) * direction.y > 0) {
					direction.y *= -1;
				}
			}
		}
		else {
			if ((col->position.y - collisionRect->position.y) * direction.y > 0) {
				direction.y *= -1;
			}
		}
		return;
	}
	case (int)CollisionBody::shapeNames::Circle: {
		return;
	}
	default:
		break;
	}
}

