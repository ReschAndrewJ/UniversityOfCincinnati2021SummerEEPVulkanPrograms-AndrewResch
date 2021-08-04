#include "ShapesUtil.h"


Rectangle::Rectangle() {
	name = (int)Shapes::Rectangle;
	width = 0;
	height = 0;
}
Rectangle::Rectangle(double width, double height) {
	name = (int)Shapes::Rectangle;
	this->width = width;
	this->height = height;
}


Circle::Circle() {
	name = (int)Shapes::Circle;
	radius = 0;
}
Circle::Circle(double radius) {
	name = (int)Shapes::Circle;
	this->radius = radius;
}