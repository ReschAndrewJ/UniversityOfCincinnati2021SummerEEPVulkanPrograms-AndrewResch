#include "CollisionShape.h"

#include <iostream>

bool CollisionShape::areOverlapping(CollisionShape* object1, CollisionShape* object2) {

	switch (object1->area->name)
	{
	case (int)Shapes::Circle: {

		switch (object2->area->name)
		{
		case (int)Shapes::Circle: {
			return pow((*(Circle*)object1->area).radius + (*(Circle*)object2->area).radius, 2) >
				pow(object1->position.x - object2->position.x, 2)
				+ pow(object1->position.y - object2->position.y, 2);
			break;
		}
		case(int)Shapes::Rectangle: {
			double closestX = std::clamp((double)object1->position.x,
				object2->position.x - (*(Rectangle*)object2->area).width / 2,
				object2->position.x + (*(Rectangle*)object2->area).width / 2);
			double closestY = std::clamp((double)object1->position.y,
				object2->position.y - (*(Rectangle*)object2->area).height / 2,
				object2->position.y + (*(Rectangle*)object2->area).height / 2);
			double distance2 = pow(object1->position.x - closestX, 2) + pow(object1->position.y - closestY, 2);
			return pow((*(Circle*)object1->area).radius, 2) > distance2;
			break;
		}
		default:
			break;
		}

		break;
	}
	case (int)Shapes::Rectangle: {
	
		switch (object2->area->name)
		{
		case (int)Shapes::Circle: {
			double closestX = std::clamp((double)object2->position.x,
				object1->position.x - (*(Rectangle*)object1->area).width / 2,
				object1->position.x + (*(Rectangle*)object1->area).width / 2);
			double closestY = std::clamp((double)object2->position.y,
				object1->position.y - (*(Rectangle*)object1->area).height / 2,
				object1->position.y + (*(Rectangle*)object1->area).height / 2);
			double distance2 = pow(object2->position.x - closestX, 2) + pow(object2->position.y - closestY, 2);
			return pow((*(Circle*)object2->area).radius, 2) > distance2;
			break;
		}
		case (int)Shapes::Rectangle: {
			if (object1->position.x - (*(Rectangle*)object1->area).height >
				object2->position.x + (*(Rectangle*)object2->area).height
				||
				object1->position.x + (*(Rectangle*)object1->area).height <
				object2->position.x - (*(Rectangle*)object2->area).height) 
			{ return false; }
			if (object1->position.y - (*(Rectangle*)object1->area).height >
				object2->position.y + (*(Rectangle*)object2->area).height
				||
				object1->position.y + (*(Rectangle*)object1->area).height <
				object2->position.y - (*(Rectangle*)object2->area).height)
			{ return false; }
			return true;
			break;
		}
		default:
			break;
		}


		break;
	}

	default:
		break;
	}

	return false;
}