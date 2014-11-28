#include "UIList.h"


UIList::UIList(Sprite *s, GLfloat posX, GLfloat posY)
	: UIElement(NULL, posX, posY, 1, 1){
	cursor = new UIElement(s, posX, posY,0.5f,0.5f);
}


UIList::~UIList() {
}

GLvoid UIList::attach(UIElement *e){
	e->parent = this;
	e->position.x = (children.size() * spacing.x) + position.x;
	e->position.y = -(children.size() * spacing.y) + position.y;
	e->buildMatrix();
	children.push_back(e);
	selectionDown();
	selectionUp();
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
		selection++;
		cursor->position.x = -1.6f;
		cursor->position.y = 0.0f;
		children[selection]->attach(cursor);
	}
}

GLvoid UIList::selectionUp(){
	if (selection > 0) {
		selection--;
		cursor->position.x = -1.6f;
		cursor->position.y = 0.0f;
		children[selection]->attach(cursor);
	}
}