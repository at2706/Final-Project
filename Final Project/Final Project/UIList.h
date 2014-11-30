#pragma once
#include "UIElement.h"
class UIList :
	public UIElement
{
public:
	UIList(Sprite *s, Mix_Chunk *wav, GLfloat posX, GLfloat posY);
	~UIList();

	GLvoid attach(UIElement *e);
	GLvoid render();

	GLvoid selectionDown();
	GLvoid selectionUp();

	Vector spacing;

	GLuint selection;
private:
	UIElement *cursor;
	Mix_Chunk *sound;
};

