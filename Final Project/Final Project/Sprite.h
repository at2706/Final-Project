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
	Sprite::Sprite(GLuint texID, GLuint index, GLuint SpriteCountX, GLint SpriteCountY);
	virtual GLvoid draw();

private:
	Vector size;
	Vector UVcoords;

	GLint index;
	GLint spriteCountX;
	GLint spriteCountY;

	GLuint textureID;
	GLboolean uniform;
	GLfloat ratio;
};

