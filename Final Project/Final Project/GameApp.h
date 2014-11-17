#pragma once
#include "Utilities.h"
class GameApp
{
public:
	GameApp();
	~GameApp();

	GLboolean ProcessEvents();
	GLvoid Update();
	GLvoid Render();

private:

};

