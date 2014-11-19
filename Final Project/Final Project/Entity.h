#pragma once

#include "Utilities.h"
#include "Matrix.h"

class Entity {
public:
	Entity(float x, float y);

	void Update(float elapsed);
	void Render();
	void buildMatrix();
	void FixedUpdate();

	Matrix matrix;
	Vector position;
	Vector rotation;
	Vector scale;
	Vector velocity;
	Vector acceleration;

	unsigned int texture;

	std::vector<float> vertices;
	std::vector<float> uvs;
	bool visible;
	float friction;
};