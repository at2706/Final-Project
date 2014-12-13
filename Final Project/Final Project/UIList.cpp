#include "UIList.h"


UIList::UIList(Sprite *s, Mix_Chunk *wav, GLfloat posX, GLfloat posY)
	: UIElement(NULL, posX, posY, 1, 1), sound(wav){
	cursor = new UIElement(s, posX, posY,0.5f,0.5f);
}


UIList::~UIList() {
}

GLvoid UIList::attach(UIElement *e){
	e->parent = this;
	e->fontTexture = fontTexture;
	e->position.x = (children.size() * spacing.x) + position.x;
	e->position.y = -(children.size() * spacing.y) + position.y;
	e->buildMatrix();
	children.push_back(e);
	
	cursor->position.x = -1.6f;
	cursor->position.y = 0.0f;
	children[0]->attach(cursor);
}

GLvoid UIList::render(){
	if (isVisible){
		cursor->render();
		for (vector<UIElement*>::iterator it = children.begin(); it != children.end(); ++it)
			(*it)->render();
	}
}

GLvoid UIList::selectionDown(){
	if (selection < children.size() - 1) {
		Mix_PlayChannel(-1, sound, 0);
		selection++;
		cursor->position.x = -1.6f;
		cursor->position.y = 0.0f;
		children[selection]->attach(cursor);
	}
}

GLvoid UIList::selectionUp(){
	if (selection > 0) {
		Mix_PlayChannel(-1, sound, 0);
		selection--;
		cursor->position.x = -1.6f;
		cursor->position.y = 0.0f;
		children[selection]->attach(cursor);
	}
}