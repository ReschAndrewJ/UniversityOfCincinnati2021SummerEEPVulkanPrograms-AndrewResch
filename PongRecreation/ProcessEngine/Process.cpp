#include "ProcessEngine.h"


void ProcessEngine::processFrame(std::vector<BaseObject*> objectsTree) {
	std::chrono::steady_clock::time_point startOfCurrentFrame = std::chrono::steady_clock::now();
	double timeDiff = std::chrono::duration_cast<std::chrono::duration<double>>(startOfCurrentFrame - startOfLastFrame).count();
	if (timeDiff < 1.0 / maxFPS) { return; }
	startOfLastFrame = startOfCurrentFrame;
	advanceInputs();
	for (BaseObject* obj : objectsTree) {
		obj->processFrame(timeDiff);
	}
	processCollisions();
}






void ProcessEngine::processCollisions() {
	std::vector<CollisionShape*> colliders{};
	for (void* shape : passData->collisionAreas) {
		colliders.push_back((CollisionShape*)shape);
	}

	for (auto forwardIt = colliders.begin(); forwardIt != colliders.end(); ++forwardIt) {
		for (auto backwardIt = colliders.end()-1; backwardIt > forwardIt; --backwardIt) {
			if (CollisionShape::areOverlapping(*forwardIt, *backwardIt)) {
				((CollidingObject2D*)(*forwardIt)->owner)->
					resolveCollision((CollidingObject2D*)(*backwardIt)->owner);
				((CollidingObject2D*)(*backwardIt)->owner)->
					resolveCollision((CollidingObject2D*)(*forwardIt)->owner);
	}}}

}