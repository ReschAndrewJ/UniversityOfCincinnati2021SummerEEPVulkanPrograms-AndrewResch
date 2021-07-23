#include "bouncingRectangle.h"



void bouncingRectangle::process(double timeDiff) {
	bool flipX = false;
	bool flipY = false;
	for (const Spatial::Vertex& vert : object.getTransformedVertices()) {
		if (abs(vert.position.x) >= 1) { flipX = true; }
		if (abs(vert.position.y) >= 1) { flipY = true; }
		if (flipX && flipY) { break; }
	}
	if (flipX) { direction.x *= -1; }
	if (flipY) { direction.y *= -1; }
	
	object.position += glm::vec3{direction.x * timeDiff * 0.75, direction.y * timeDiff * 0.75, 0};
	
	parentApp_shouldUpdateFunc(parentApp);
}


void bouncingRectangle::onReady() {
	Spatial::Rectangle rect = Spatial::Rectangle(0.5, 0.5);
	rect.color = { static_cast<float>(rand() % 256) / 255, 
		static_cast<float>(rand() % 256) / 255, 
		static_cast<float>(rand() % 256) / 255 };

	object = rect.convertToObject();
	object.position = { 0.0, 0.0, 0.0 };

	float rads = float(rand() % 360) / 180 * (4 * atan(1));
	direction = { cos(rads), sin(rads) };
	
}