#pragma once
#include <math.h>

class Vector {
public:
	Vector();
	Vector(float, float, float z = 0);

	float length() const;
	void normalize();
	float operator* (const Vector&);
	float x;
	float y;
	float z;
};