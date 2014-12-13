#pragma once
#include "Utilities.h"
#include "Entity.h"
class Player
{
public:
	Player();
	~Player();

	Entity *hero;
	Entity *target;
	SDL_Joystick *controller;
	GLfloat axisValues[6];
	UIText *reviveIndicator;
};

