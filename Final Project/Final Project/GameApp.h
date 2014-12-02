#pragma once
#include "Entity.h"
#include "UIElement.h"
#include "UIText.h"
#include "UIList.h"
#include "Player.h"
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

	Mix_Music *introMusic;
	Mix_Chunk *menuMove;
	Mix_Chunk *jump;

	GLfloat controllerCooldown;

	GLvoid init();
	GLvoid time();
	GLuint loadTexture(const char *image_path, GLint param = GL_LINEAR);
	GLvoid buildMainMenu();
	GLvoid buildPauseMenu();
	GLvoid buildUIstatic();
	GLvoid initPlayer(int);
	GLvoid gameStart();
	GLvoid followPlayers(Player *p);
	GLvoid drawPlatformHorizontal(GLfloat length, GLfloat x, GLfloat y);

	GameState state;
	GLfloat timeLeftOver = 0.0f;
	GLfloat elapsed;
	GLfloat ticks;
	GLfloat lastFrameTicks;
	Vector gravity;
	Matrix scaleMatrix;
	GLfloat zoom;
	GLfloat fadeTime;

	GLuint myVertexBuffer;
	GLuint charSheet;
	GLuint tileSheet;
	GLuint UISheet;
	GLuint fontTexture;
	GLuint bgTexture;

	GLuint playerCount;
	Player players[4];

	UIElement *UImain;
	UIList *mainList;
	UIElement *UIpause;
	UIList *pauseList;
	UIElement *UIstatic;

	// for collision
	/*void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);
	void doLevelCollisionY(Entity* temp);
	void doLevelCollisionX(Entity* temp);
	float checkPointForGridCollisionY(float x, float y);
	float checkPointForGridCollisionX(float x, float y);
	bool isSolid(int);*/

	// for level generation
	/*void makeGameLevel();
	bool genPath(int, int, int);*/

	int mapLayout[LAYOUT_X][LAYOUT_Y];
	Vector mapGoal;
	Vector mapStart;
};