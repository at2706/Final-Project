#include "Ladder.h"


Ladder::Ladder(Sprite *s, float x, float y)
	: Entity(s,x,y){
	isStatic = true;
	enableCollisions = true;
}


Ladder::~Ladder()
{
}

GLvoid Ladder::collisionEffectX(Entity *e){

}

GLvoid Ladder::collisionEffectY(Entity *e){
	e->acceleration.y = -GRAVITY;
}