#pragma once
#include "Entity.h"
#include "UIElement.h"
#define LAYOUT_X 5
#define LAYOUT_Y 5
#define WORLD_OFFSET_X 0
#define WORLD_OFFSET_Y 0

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
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_GAME_PAUSE, STATE_GAME_OVER };

	GLvoid init();
	GLvoid time();
	GLuint loadTexture(const char *image_path, GLint param = GL_LINEAR);

	GLfloat timeLeftOver = 0.0f;
	GLfloat elapsed;
	GLfloat ticks;
	GLfloat lastFrameTicks;
	Vector gravity;
	GameState state;

	GLuint charSheet;
	GLuint tileSheet;
	GLuint UISheet;
	GLuint fontSheet;
	GLuint bgTexture;

	UIElement *UImain;
	Entity *players[4];

	// for collision
	void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);
	void doLevelCollisionY(Entity* temp);
	void doLevelCollisionX(Entity* temp);
	float checkPointForGridCollisionY(float x, float y);
	float checkPointForGridCollisionX(float x, float y);
	bool isSolid(int);

	// for level generation
	void makeGameLevel();
	bool genPath(int, int, int);

	int mapLayout[LAYOUT_X][LAYOUT_Y];
	Vector mapGoal;
	Vector mapStart;
};

