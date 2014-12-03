#include "UIText.h"

UIText::UIText(string text, GLfloat size, GLfloat posX, GLfloat posY)
	: UIElement(NULL, posX, posY, size, size), text(text) {
	color = {1,1,1,1};
	spacing = -0.17f;
}


UIText::~UIText(){
}

GLvoid UIText::attach(UIElement *e){
	e->parent = this;

	e->position.x = (e->position.x * (scale.x / 2) * scale.x) + position.x;
	e->position.y = (e->position.y * ((scale.y / 2) + spacing) * scale.y) + position.y;
	e->buildMatrix();
	children.push_back(e);
}

GLvoid UIText::render(){
	if (isVisible){
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		glBindTexture(GL_TEXTURE_2D, parent->fontTexture);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(matrix.ml);
		GLfloat texture_size = 1.0 / 16.0f;
		vector<GLfloat> vertexData;
		vector<GLfloat> texCoordData;
		vector<GLfloat> colorData;

		for (GLfloat i = 0; i < text.size(); i++) {
			GLfloat texture_x = (GLfloat)(((GLint)text[i]) % 16) / 16.0f;
			GLfloat texture_y = (GLfloat)(((GLint)text[i]) / 16) / 16.0f;
			colorData.insert(colorData.end(), { color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a });
			vertexData.insert(vertexData.end(), { ((scale.x + spacing) * i) + (-0.5f * scale.x), 0.5f * scale.y,
				((scale.x + spacing) * i) + (-0.5f * scale.x), -0.5f * scale.y,
				((scale.x + spacing) * i) + (0.5f * scale.x), -0.5f * scale.y,
				((scale.x + spacing) * i) + (0.5f * scale.x), 0.5f * scale.y });
			texCoordData.insert(texCoordData.end(), { texture_x, texture_y, texture_x, texture_y + texture_size, texture_x +
				texture_size, texture_y + texture_size, texture_x + texture_size, texture_y });
		}

		glColorPointer(4, GL_FLOAT, 0, colorData.data());
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
		glEnableClientState(GL_VERTEX_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glDrawArrays(GL_QUADS, 0, text.size() * 4);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glPopMatrix();

		for (vector<UIElement*>::iterator it = children.begin(); it != children.end(); ++it)
			(*it)->render();
	}
}