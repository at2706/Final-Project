#include "Vector.h"

Vector::Vector() { x = 0, y = 0, z = 0; }
Vector::Vector(float x, float y, float z) : x(x), y(y), z(z) {}

float Vector::length() const {
	float len = x*x + y*y + z*z;
	return sqrt(len);
}

void Vector::normalize() {
	x /= length();
	y /= length();
	z /= length();
}

float Vector::operator* (const Vector& v2) {
	return (x * v2.x) + (y * v2.y);
}