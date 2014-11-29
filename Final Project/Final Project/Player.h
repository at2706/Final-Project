#pragma once
#include "Utilities.h"
#include "Entity.h"
class Player
{
public:
	Player();
	~Player();

	Entity *hero;
	SDL_Joystick *controller;
	GLfloat axisValues[3];
};

