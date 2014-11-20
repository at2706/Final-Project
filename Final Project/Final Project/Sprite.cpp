#include "Sprite.h"

Sprite::Sprite(){
}


Sprite::~Sprite(){
}

//Non-uniform Constructor
Sprite::Sprite(GLuint texID, GLuint tWidth, GLuint tHeight, GLuint u, GLuint v, GLuint width, GLuint height)
	: textureID(texID){
	uniform = false;
	ratio = tWidth / tHeight;

	UVcoords.x = (float)u / tWidth;
	size.x = (float)width / tWidth;

	UVcoords.y = (float)v / tHeight;
	size.y = (float)height / tHeight;
}

//Uniform Constructor
Sprite::Sprite(GLuint texID, GLuint index, GLuint SpriteCountX, GLint SpriteCountY)
	: textureID(texID){
	uniform = true;
	ratio = SpriteCountX / SpriteCountY;

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
	vector<GLfloat> vertexData;
	vector<GLfloat> texCoordData;
	vertexData.insert(vertexData.end(), { -size.x / 2, size.y / 2,
		-size.x / 2, -size.y / 2,
		size.x / 2, -size.y / 2,
		size.x / 2, size.y / 2 });
	if (!uniform){//Non-uniformed sprite sheets
		texCoordData.insert(texCoordData.end(), { UVcoords.x, UVcoords.y,
			UVcoords.x, UVcoords.y + size.y,
			UVcoords.x + size.x, UVcoords.y + size.y,
			UVcoords.x + size.x, UVcoords.y });
	}

	else{ //Uniformed sprite sheets
		texCoordData.insert(texCoordData.end(), { UVcoords.x, UVcoords.y,
			UVcoords.x, UVcoords.y + size.y,
			UVcoords.x + (size.x / ratio), UVcoords.y + size.y,
			UVcoords.x + (size.x / ratio), UVcoords.y });
	}

	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
}