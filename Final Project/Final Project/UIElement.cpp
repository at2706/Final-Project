#include "UIElement.h"

UIElement::UIElement()
	: position(0,0,0),scale(1,1,0){
}

UIElement::UIElement(Sprite *sheet, GLfloat posX, GLfloat posY, GLfloat scale_x, GLfloat scale_y)
	: isVisible(true), position(posX,posY,0.0f), scale(scale_x,scale_y, 0.0f){
	parent = nullptr;
	sprite = sheet;
	buildMatrix();
}

UIElement::~UIElement(){
}

GLvoid UIElement::attach(UIElement *e){
	e->parent = this;
	e->fontTexture = fontTexture;
	if (sprite != nullptr)
	{
		e->position.x = (e->position.x * (sprite->size.x / 2) * scale.x) + position.x;
		e->position.y = (e->position.y * (sprite->size.y / 2) * scale.y) + position.y;
		e->buildMatrix();
	}
	
	children.push_back(e);
}

GLvoid UIElement::render(){
	if (isVisible){
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(matrix.ml);
		sprite->render();
		
		glPopMatrix();
	}
	
	for (vector<UIElement*>::iterator it = children.begin(); it != children.end(); ++it)
		(*it)->render();
}

GLvoid UIElement::buildMatrix(){
	Matrix scaleMatrix;
	scaleMatrix.m[0][0] = scale.x;
	scaleMatrix.m[1][1] = scale.y;

	Matrix translateMatrix;
	translateMatrix.m[3][0] = position.x;
	translateMatrix.m[3][1] = position.y;
	translateMatrix.m[3][2] = position.z;

	matrix = scaleMatrix * translateMatrix;
}

GLvoid UIElement::setPosition(GLfloat x, GLfloat y){
	position = Vector(x, y);
	buildMatrix();
}

GLvoid UIElement::setScale(GLfloat x, GLfloat y){
	scale = Vector(x, y);
	buildMatrix();
}