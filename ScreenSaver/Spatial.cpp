#include "Spatial.h"


// Vertex Struct

bool Spatial::Vertex::operator==(const Vertex& rItem) {
	return (rItem.position == position && rItem.color == color);
}



// Object Class 

Spatial::Object::Object() {
	vertices = {};
	indices = {};
	position = { 0,0,0 };
}

void Spatial::Object::operator=(const Object& rItem) {
	vertices = rItem.vertices;
	indices = rItem.indices;
}

Spatial::Object Spatial::Object::combine(const Object& object1, const Object& object2) {
	Object newObj = object1;
	for (size_t index : object2.indices) {
		bool shared = false;
		for (size_t i = 0; i < newObj.vertices.size(); i++) {
			if (newObj.vertices[i] == object2.vertices[index]) {
				shared = true;
				newObj.indices.push_back(i);
				break;
			}
		}
		if (!shared) {
			newObj.vertices.push_back(object2.vertices[index]);
			newObj.indices.push_back(newObj.vertices.size() - 1);
		}
	}
	return newObj;
}

std::vector<Spatial::Vertex> Spatial::Object::getTransformedVertices() {
	std::vector<Vertex> transformedVerts = vertices;
	for (size_t i = 0; i < transformedVerts.size(); i++) {
		transformedVerts[i].position += position;
	}
	return transformedVerts;
}


// Shape2D Class

std::string Spatial::Shape2D::getTexture() {
	return texture;
}

void Spatial::Shape2D::setTexture(std::string filePath) {
	std::fstream textureFile;
	textureFile.open(filePath, std::fstream::in);
	if (!textureFile.is_open()) {
		throw std::runtime_error("Failed to Find Texture From File Path: " + filePath);
	}
	textureFile.close();
	texture = filePath;
}



// Rectangle Class

Spatial::Rectangle::Rectangle() {
	color = { 1,1,1 };
	keyPoints.resize(2);
	for (glm::vec2& p : keyPoints) {
		p = { 0,0 };
	}
}

Spatial::Rectangle::Rectangle(float width, float height) {
	color = { 1,1,1 };
	keyPoints.resize(2);
	keyPoints[0] = { -width / 2, -height / 2 };
	keyPoints[1] = { width / 2, height / 2 };
}

Spatial::Object Spatial::Rectangle::convertToObject() {
	Object obj;

	obj.vertices.resize(4);
	obj.indices.resize(4);

	obj.vertices[0] = { {keyPoints[0], 0 }, color };

	obj.vertices[1] = { {keyPoints[1].x, keyPoints[0].y, 0}, color };

	obj.vertices[2] = { {keyPoints[1], 0}, color };

	obj.vertices[3] = { {keyPoints[0].x, keyPoints[1].y, 0}, color };
	
	if ((keyPoints[0].x - keyPoints[1].x) * (keyPoints[0].y - keyPoints[1].y) > 0) {
		obj.indices = { 0,1,2,2,3,0 };
	}
	else {
		obj.indices = { 0,3,2,2,1,0 };
	}

	if (!texture.empty()) {
		obj.texture = texture;
	}

	return obj;
}