#pragma once
#include "Utilities.h"
#include "Sprite.h"

class UIElement
{
public:
	UIElement(Sprite *sheet, GLfloat posX = 0.0f, GLfloat posY = 0.0f, GLfloat scale_x = 1.0f, GLfloat scale_y = 1.0f);
	~UIElement();

	virtual GLvoid attach(UIElement *e);
	virtual GLvoid render();
	GLvoid buildMatrix();

	GLvoid setPosition(GLfloat,GLfloat);
	GLvoid setScale(GLfloat, GLfloat);

	UIElement *parent;
	vector<UIElement*> children;
	Sprite *sprite;

	Matrix matrix;
	Vector position;
	Vector scale;
	GLboolean isVisible;
	
};