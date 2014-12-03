#pragma once
#include "Entity.h"
class Weapon
{
public:
	Weapon(Sprite *s);
	~Weapon();

	GLvoid fire();
	
	Sprite *sprite;
	GLfloat speed;
	GLfloat fireRate;
	GLfloat cooldown;
	GLfloat damage;

};

