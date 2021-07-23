#include "Object.h"


// structs --------------------------------------------------

VkVertexInputBindingDescription Object::Vertex::getBindingDescription() {
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> Object::Vertex::getAttributeDescriptions() {
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);

	return attributeDescriptions;
}

	
// public functions -----------------------------------------

Object::Object() {
	vertices = {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};
	 
	indices = { 0, 1, 2 };
};


void Object::operator=(const Object& obj) {
	vertices = obj.vertices;
	indices = obj.indices;
}



// SHAPE CLASSES

void Shape::fillEmpty() {
	for (glm::vec2& point : points) {
		point = { 0.0f, 0.0f };
	}
}



Rectangle::Rectangle() {
	color = { 1.0f, 1.0f, 1.0f };
	points.resize(2);
	fillEmpty();
}

Rectangle::Rectangle(glm::vec2 point1, glm::vec2 point2, glm::vec3 color) {
	this->color = color;
	points.resize(2);
	points[0] = point1;
	points[1] = point2;
}


Object Rectangle::convertToObject() {
	Object obj;

	obj.vertices.resize(4);
	obj.indices.resize(6);
		
	obj.vertices[0] = { points[0], color };
	obj.vertices[1] = { {points[1].x, points[0].y}, color };
	obj.vertices[2] = { points[1], color };
	obj.vertices[3] = { {points[0].x, points[1].y}, color };

	if ((points[0].x - points[1].x) * (points[0].y - points[1].y) > 0) {
		obj.indices = { 0, 1, 2, 2, 3, 0 };
	}
	else {
		obj.indices = { 0, 3, 2, 2, 1, 0 };
	}

	return obj;
}
