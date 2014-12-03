#include "Ladder.h"


Ladder::Ladder(Sprite *s, float x, float y)
	: Entity(s,x,y){
	isStatic = true;
	enableCollisions = true;
}


Ladder::~Ladder()
{
}

GLvoid Ladder::collisionEffectX(){
	cout << "childX" << endl;
}

GLvoid Ladder::collisionEffectY(){
	cout << "childY" << endl;
}