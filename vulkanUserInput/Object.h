#pragma once

class Object;

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include <vector>
#include <array>

class Object
{

	// structs
public:

	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();

	};


	// public members
public:

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;


	// public functions
public:

	Object();

	void operator=(const Object&);

};


class Shape {

public:

	virtual Object convertToObject() = 0;

	std::vector<glm::vec2> points;
	glm::vec3 color;

	static void normalizeCoordinates(Shape&, int, int);

protected:

	void fillEmpty();

};

class Rectangle : public Shape {

public:
	
	Rectangle();
	Rectangle(glm::vec2, glm::vec2, glm::vec3 color = {1.0f, 1.0f, 1.0f} );

	Object convertToObject();

};
