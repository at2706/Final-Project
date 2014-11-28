#pragma once
#include "UIElement.h"
class UIList :
	public UIElement
{
public:
	UIList(Sprite *s, GLfloat posX, GLfloat posY);
	~UIList();

	GLvoid attach(UIElement *e);
	GLvoid render();

	GLvoid selectionDown();
	GLvoid selectionUp();

	Vector spacing;
private:
	UIElement *cursor;
	GLuint selection;
};

