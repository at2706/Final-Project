#pragma once
#include "UIElement.h"
#include "UIText.h"
#include "UIList.h"
#include "Player.h"
#define LAYOUT_X 5
#define LAYOUT_Y 5
#define TRUE_X 250
#define TRUE_Y 250
#define WORLD_OFFSET_X 0
#define WORLD_OFFSET_Y 0
#define spriteCountX 24
#define spriteCountY 16


class GameApp
{
public:
	friend class Entity;
	GameApp();
	~GameApp();

	GLboolean updateAndRender();

	GLvoid Update();
	GLvoid Render();

protected:
	Player players[4];
	GLint score = 0;
	GLint lives = 2;

private:
	SDL_Event event;
	SDL_Window* displayWindow;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_GAME_PAUSE, STATE_GAME_OVER };

	Mix_Music *introMusic;
	Mix_Chunk *menuMove;
	Mix_Chunk *jump;
	Mix_Chunk *hurt;
	Mix_Chunk *shootSound;
	Mix_Chunk *healSound;
	Mix_Chunk *reviveSound;
	Mix_Chunk *pickupSound;

	GLfloat cooldown = 0;
	GLfloat reviveCooldown = 0;

	GLvoid init();
	GLvoid time();
	GLuint loadTexture(const char *image_path, GLint param = GL_LINEAR);
	GLvoid buildMainMenu();
	GLvoid buildPauseMenu();
	GLvoid buildGameUI();
	GLvoid initPlayer(int);
	GLvoid gameStart();
	GLvoid followPlayers(Player *p);
	GLvoid drawPlatformHorizontal(GLfloat length, GLfloat x, GLfloat y);
	GLvoid drawLadder(GLfloat length, GLfloat x, GLfloat y);

	GameState state;
	GLfloat timeLeftOver = 0.0f;
	GLfloat elapsed = 0;
	GLfloat ticks = 0;
	GLfloat lastFrameTicks = 0;

	Matrix translateMatrix;
	GLfloat fadeTime;

	GLuint myVertexBuffer;
	GLuint charSheet;
	GLuint buttonSheet;
	GLuint tileSheet;
	GLuint UISheet;
	GLuint fontTexture;
	GLuint bgTexture;
	GLuint gameTile;

	GLuint playerCount;

	UIElement *UImain;
	UIList *mainList;
	UIElement *UIpause;
	UIList *pauseList;
	UIElement *UIStatic;
	UIElement *UIGame;
	UIText *lifeIndicator;
	UIText *scoreIndicator;

	vector<Sprite*> *animHeroIdle;
	vector<Sprite*> *animHeroRun;
	vector<Sprite*> *animHeroJump;
	vector<Sprite*> *animHeroCrouch;
	vector<Sprite*> *animHeroDead;
	vector<Sprite*> *pickup;

	// for collision
	/*void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);
	void doLevelCollisionY(Entity* temp);
	void doLevelCollisionX(Entity* temp);
	float checkPointForGridCollisionY(float x, float y);
	float checkPointForGridCollisionX(float x, float y);
	bool isSolid(int);*/

	// for level generation
	void makeGameLevel();
	bool genPath(int, int, int);

	void readMap(string map, unsigned int **&tmpMap, int xOffset, int yOffset);
	bool readHeader(ifstream&, unsigned int **&tmpMap);
	bool readLayerData(ifstream&, unsigned int **&tmpMap);
	bool readEntityData(ifstream&, unsigned int **&tmpMap, int xOffset, int yOffset);
	void placeEntity(string, float, float, int xOffset, int yOffset);

	void createMap();
	void fillLargeArray(unsigned int** &small, int xOffset, int yOffset);
	void fillSmallArray();
	void renderGameLevel();
	void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);

	int mapWidth;
	int mapHeight;
	int mapLayout[LAYOUT_X][LAYOUT_Y];
	unsigned int trueMap[TRUE_Y][TRUE_X];
	unsigned int **unwalkable;
	unsigned int **tmp;
	unsigned int smallMap[50][50];
	Vector startPoint;

	Vector mapGoal;
	Vector mapStart;
};