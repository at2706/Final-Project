#pragma once
#include "Utilities.h"
#include "Matrix.h"
class Sprite
{
public:
	Sprite();
	~Sprite();

	//Non-uniform Constructor
	Sprite(GLuint texID, GLuint tWidth, GLuint tHeight, GLuint u, GLuint v, GLuint width, GLuint height);

	//Uniform Constructor
	Sprite(GLuint texID, GLuint index, GLuint SpriteCountX, GLint SpriteCountY);
	virtual GLvoid render();

	Vector size;

private:
	Vector UVcoords;

	GLuint textureID;
	GLboolean uniform;
	GLfloat ratio;
};

