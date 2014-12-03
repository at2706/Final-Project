#pragma once
#include "Entity.h"
class Ladder :
	public Entity
{
public:
	Ladder(Sprite *s, float x = 0, float y = 0);
	~Ladder();

	GLvoid collisionEffectX();
	GLvoid collisionEffectY();
};

