#pragma once
#include "UIElement.h"
class UIText :
	public UIElement
{
public:
	UIText(string text, GLfloat posX = 0.0f, GLfloat posY = 0.0f, GLfloat size = 0.3f);
	~UIText();
	GLvoid attach(UIElement *e);
	GLvoid render();

	string text;
	Color color;
	GLfloat spacing;

};

