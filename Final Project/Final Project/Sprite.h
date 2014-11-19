#pragma once
#include "Utilities.h"
#include "Matrix.h"
class Sprite
{
public:
	Sprite();
	~Sprite();

	virtual GLvoid draw(GLfloat x, GLfloat y, GLfloat facing);

private:
	Matrix matrix;
	Vector textureSize;
	Vector size;
	Vector UVcoords;

	GLuint textureID;
};

