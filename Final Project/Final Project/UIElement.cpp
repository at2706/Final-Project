#include "UIElement.h"

UIElement::UIElement(Sprite *sheet, GLfloat posX, GLfloat posY, GLfloat scale_x, GLfloat scale_y)
	: isVisible(true), position(posX,posY,0.0f), scale(scale_x,scale_y, 0.0f){
	parent = nullptr;
	sprite = sheet;
}

UIElement::~UIElement(){
}

GLvoid UIElement::attach(UIElement *e){
	e->parent = this;

	e->position.x = (position.x * sprite->size.x *scale.x) + position.x;
	e->position.y = (position.y * sprite->size.y * scale.y) + position.y;

	children.push_back(e);
}

GLvoid UIElement::render(){
	if (isVisible){
			buildMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glMultMatrixf(matrix.ml);
			sprite->render();

		glPopMatrix();

		for (vector<UIElement*>::iterator it = children.begin(); it != children.end(); ++it)
			(*it)->render();
	}
}

GLvoid UIElement::buildMatrix(){
	Matrix scaleMatrix;
	scaleMatrix.m[0][0] = scale.x;
	scaleMatrix.m[1][1] = scale.y;

	Matrix translateMatrix;
	translateMatrix.m[3][0] = position.x;
	translateMatrix.m[3][1] = position.y;

	matrix = scaleMatrix * translateMatrix;
}