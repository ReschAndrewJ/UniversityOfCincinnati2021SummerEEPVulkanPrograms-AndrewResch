#pragma once

#include "glm/glm.hpp"

#include <array>
#include <vector>
#include <string>
#include <fstream>

namespace Spatial {


	struct Vertex {
		glm::vec3 position;
		glm::vec3 color;

		bool operator==(const Vertex&);
	};


	class Object {

	public:

		// Public Members

		std::vector<Vertex> vertices;
		std::vector<int> indices;


		// Public Functions

		Object();

		static Object combine(const Object&, const Object&);
		void operator=(const Object&);

	};


	class Shape2D {

	public:
		

		// Public Members

		std::vector<glm::vec2> keyPoints;
		glm::vec3 color{};


		// Public Virtual Funtions

		virtual Object convertToObject() = 0;
		

	};


	class Rectangle : public Shape2D {

	public:

		// Public Functions

		Rectangle();
		Rectangle(float, float);


		// Public Virtual Implementation

		Object convertToObject() override;

	};





};