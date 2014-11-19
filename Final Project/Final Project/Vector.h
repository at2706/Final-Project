#pragma once
#include <math.h>

class Vector {
public:
	Vector();
	Vector(float, float, float);

	float length();
	void normalize();
	float operator* (const Vector&);
	float x;
	float y;
	float z;
};