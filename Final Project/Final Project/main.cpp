#include "GameApp.h"

int main(int argc, char *argv[])
{
	GameApp game;

	while (game.ProcessEvents()) {

		game.Update();

		game.Render();
	}

	return 0;
}