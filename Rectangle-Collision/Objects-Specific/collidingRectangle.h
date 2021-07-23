#pragma once

#include "bouncingRectangle.h"
#include "../Collision/CollisionBody.h"

class collidingRectangle : public bouncingRectangle
{

public:

	Body2D::Rectangle* collisionRect;

	void onCollision(CollisionBody*);

	void onReady() override;

	collidingRectangle() { vertexShaderFilepath = "shaders/vert.spv"; }

};

