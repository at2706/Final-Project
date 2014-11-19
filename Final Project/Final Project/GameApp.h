#pragma once
#include "Utilities.h"
#include "Matrix.h"
#include "Vector.h"

class GameApp
{
public:
	GameApp();
	~GameApp();

	GLboolean updateAndRender();

	GLvoid Update();
	GLvoid Render();

private:
	SDL_Event event;
	SDL_Window* displayWindow;
	const Uint8 *keys;

	void init();
};

