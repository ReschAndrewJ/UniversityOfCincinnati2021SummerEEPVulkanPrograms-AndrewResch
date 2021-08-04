#pragma once


enum class Shapes {
	Rectangle, Circle,
};



class Shape2D {
public:
	int name = -1;
};


class Rectangle : public Shape2D {

public:
	Rectangle();
	Rectangle(double width, double height);

	double width, height;

};


class Circle : public Shape2D {

public:
	Circle();
	Circle(double radius);

	double radius;
};