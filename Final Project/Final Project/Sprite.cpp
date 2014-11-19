#include "Sprite.h"

Sprite::Sprite(){
}


Sprite::~Sprite(){
}

GLvoid Sprite::draw(GLfloat x, GLfloat y, GLfloat facing){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	GLfloat quad[] = { -size.x / 2, size.y / 2,
		-size.x / 2, -size.y / 2,
		size.x / 2, -size.y / 2,
		size.x / 2, size.y / 2 };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);
	GLfloat quadUVs[] = { UVcoords.x, UVcoords.y, UVcoords.x, UVcoords.y + size.y, UVcoords.x + size.x,
			UVcoords.y + size.y, UVcoords.x + size.x, UVcoords.y };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
}