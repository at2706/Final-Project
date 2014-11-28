#include "Sprite.h"

Sprite::Sprite(){
}


Sprite::~Sprite(){
}

//Non-uniform Constructor
Sprite::Sprite(GLuint texID, GLuint tWidth, GLuint tHeight, GLuint u, GLuint v, GLuint width, GLuint height)
	: textureID(texID){
	ratio = tWidth / tHeight;

	UVcoords.x = (float)u / tWidth;
	size.x = (float)width / tHeight;

	UVcoords.y = (float)v / tHeight;
	size.y = (float)height / tHeight;
}

//Uniform Constructor
Sprite::Sprite(GLuint texID, GLuint index, GLuint spriteCountX, GLint spriteCountY)
	: textureID(texID){
	ratio = spriteCountX / spriteCountY;

	UVcoords.x = (GLfloat)(((GLint)index) % spriteCountX) / (GLfloat)spriteCountX;
	size.x = ratio / (GLfloat)spriteCountX;

	UVcoords.y = (GLfloat)(((GLint)index) / spriteCountY) / (GLfloat)spriteCountY;
	size.y = 1.0 / (GLfloat)spriteCountY;
}


GLvoid Sprite::render(){
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, textureID);

	GLfloat vertexData[] = { -size.x / 2, size.y / 2,
		-size.x / 2, -size.y / 2,
		size.x / 2, -size.y / 2,
		size.x / 2, size.y / 2 };

	GLfloat texCoordData[] = { UVcoords.x, UVcoords.y,
		UVcoords.x, UVcoords.y + size.y,
		UVcoords.x + (size.x / ratio), UVcoords.y + size.y,
		UVcoords.x + (size.x / ratio), UVcoords.y };

	glVertexPointer(2, GL_FLOAT, 0, vertexData);
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices.data());
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
}