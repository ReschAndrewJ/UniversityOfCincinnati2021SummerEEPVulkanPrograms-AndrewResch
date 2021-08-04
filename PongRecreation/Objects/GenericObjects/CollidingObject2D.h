#pragma once

#include "BaseObject.h"
#include "../CollisionShape.h"


class CollidingObject2D : public BaseObject
{

public:

	CollisionShape* collisionArea;

	virtual void resolveCollision(CollidingObject2D* collider) {};

	
	

};

