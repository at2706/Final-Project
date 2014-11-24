#include "GameApp.h"

GameApp::GameApp() {
	init();
	charSheet = loadTexture("sheet.png");
	tileSheet = loadTexture("arne_sprites.png", GL_NEAREST);
	UISheet = loadTexture("greenSheet.png", GL_NEAREST);
	Sprite *s;
	Entity *e;
	s = new Sprite(charSheet, 1024, 1024, 112, 866, 112, 75);
	e = new Entity(s);
	
	s = new Sprite(tileSheet, 114, 16, 8);
	e = new Entity(s, 0.5f, 0.0f);

	e->speed = 2.0;
	e->acceleration.x = 4;
	e->friction.x = 4;
	players[0] = e;

	//<SubTexture name="green_panel.png" x="190" y="94" width="100" height="100"/>
	s = new Sprite(UISheet, 512, 256, 190.0f, 94.0, 100.0f, 100.0f);
	UImain = new UIElement(s, -1.2f,0.4f,1.0f,1.0f);

	//<SubTexture name="green_boxCross.png" x="380" y="36" width="38" height="36"/>
	s = new Sprite(UISheet, 512, 256, 380.0f, 36.0f, 38.0f, 36.0f);
	UIElement *ele = new UIElement(s,0.0f,0.5f);
	UImain->attach(ele);

	// keep in mind each tile inside the maplayout is a box of 100x100, so entire game level will be 500x500
	// for level generation
	mapStart.x = 1 + (std::rand() % (4));
	mapStart.y = 1 + (std::rand() % (4));
	mapGoal.x = 1 + (std::rand() % (4));
	mapGoal.y = 1 + (std::rand() % (4));
	// to ensure theres enough space between the start and end point
	while (fabs(mapStart.x - mapGoal.x) + fabs(mapStart.y - mapGoal.y) < 6) {
		mapStart.x = 1 + (std::rand() % (4));
		mapStart.y = 1 + (std::rand() % (4));
		mapGoal.x = 1 + (std::rand() % (4));
		mapGoal.y = 1 + (std::rand() % (4));
	}
	mapLayout[(int)mapStart.x][(int)mapStart.y] = 1;
	mapLayout[(int)mapGoal.x][(int)mapGoal.y] = 1;
	makeGameLevel();
}

GLvoid GameApp::init() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	glViewport(0, 0, 1280, 720);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
}

GameApp::~GameApp()
{
	SDL_Quit();
}

GLboolean GameApp::updateAndRender() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			return true;
		}
	}

	if (keys[SDL_SCANCODE_D]){
		players[0]->isIdle = false;
		players[0]->setRotation(0.0f);
	}
	else if (keys[SDL_SCANCODE_A]){
		players[0]->isIdle = false;
		players[0]->setRotation(180.0f);
	}
	else players[0]->isIdle = true;

	Update();

	Render();

	return false;
}
GLvoid GameApp::Update() {
	time();
}

GLvoid GameApp::time(){
	ticks = (GLfloat)SDL_GetTicks() / 1000.0f;
	elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	GLfloat fixedElapsed = elapsed + timeLeftOver;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}

	while (fixedElapsed >= FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;
		Entity::fixedUpdateAll();
	}
	timeLeftOver = fixedElapsed;

}

GLuint GameApp::loadTexture(const char *image_path, GLint param) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	if (surface != NULL) glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
	SDL_FreeSurface(surface);
	return textureID;
}

GLvoid GameApp::Render() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UImain->render();
	Entity::renderAll();
	SDL_GL_SwapWindow(displayWindow);
}

// collision stuff that i used
// NOT DONE YET, levelData needs to  be created which will have to be on me since i need that for procedural generation
void GameApp::worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
	*gridX = (int)((worldX + (WORLD_OFFSET_X)) / TILE_SIZE);
	*gridY = (int)((-worldY + (WORLD_OFFSET_Y)) / TILE_SIZE);
}

float GameApp::checkPointForGridCollisionY(float x, float y) {
	int gridX, gridY;
	worldToTileCoordinates(x, y, &gridX, &gridY);
	if (gridX < 0 || gridX > 100 || gridY < 0 || gridY > 100) {
		return 0.0f;
	}
	if (isSolid(levelData[gridY][gridX])) {
		float yCoord = (gridY * TILE_SIZE);
		return -y - yCoord;
	}
	return 0.0f;
}

float GameApp::checkPointForGridCollisionX(float x, float y) {
	int gridX, gridY;
	worldToTileCoordinates(x, y, &gridX, &gridY);
	if (gridX < 0 || gridX > 100 || gridY < 0 || gridY > 100) {
		return 0.0f;
	}
	if (isSolid(levelData[gridY][gridX])) {
		float xCoord = (gridX * TILE_SIZE);
		return -x - xCoord;
	}
	return 0.0f;
}

void GameApp::doLevelCollisionY(Entity* temp) {
	float adjust = checkPointForGridCollisionY(temp->position.x, temp->position.y - temp->getHeight() * 0.5f); // do we have a variable for width and height?
	if (adjust != 0.0f) {
		temp->position.y += adjust + 0.0001f;
		temp->velocity.y = 0.0f;
		temp->collidedBottom =true;
	}
	adjust = checkPointForGridCollisionY(temp->position.x, temp->position.y + temp->getHeight() * 0.5f); // same as previous
	if (adjust != 0.0f) {
		temp->position.y -= adjust - 0.0001f;
		temp->velocity.y = 0.0f;
		temp->collidedTop = true;
	}
}

void GameApp::doLevelCollisionX(Entity* temp) {
	float adjust = checkPointForGridCollisionX(temp->position.x - temp->getWidth() * 0.5f, temp->position.y); // same as previous
	if (adjust != 0.0f) {
		temp->position.x -= adjust * TILE_SIZE *0.008f;
		temp->velocity.x = 0.0f;
		temp->collidedLeft = true;
	}
	adjust = checkPointForGridCollisionX(temp->position.x + temp->getWidth() * 0.5f, temp->position.y); // same as previous
	if (adjust != 0.0f) {
		temp->position.x += adjust * TILE_SIZE * 0.001;
		temp->velocity.x = 0.0f;
		temp->collidedRight = true;
	}
}

bool GameApp::isSolid(int tile) {
	// gotta change this to the spritesheet we are using
	// make a case for all the solid blocks
	switch (tile) {
	case 1:
		return true;
		break;
	}
}

// THIS STUFF IS FOR LEVEL GENERATION
void GameApp::makeGameLevel() {
	while (!genPath(mapStart.x, mapStart.y, 8)){} // to make sure the maplayout has a path from start to end of length 8
}

bool GameApp::genPath(int x, int y, int length) {
	// first part of procedural generation
	// I HAVE NOT TESTED THIS, THIS WAS MADE BASED OFF OF PSEUDOCODE
	if (x == mapGoal.x && y == mapGoal.y && length == 0)
		return true;
	if (mapLayout[x][y] == 1 || x < 0 || y < 0 || x > LAYOUT_X || y > LAYOUT_Y)
		return false;
	mapLayout[x][y] = 1;
	switch (1 + (std::rand() % (4))) {
	case 1:
		if (genPath(x, y - 1, length - 1)) // north of x,y
			return true;
	case 2:
		if (genPath(x + 1, y, length - 1)) // east of x,y
			return true;
	case 3:
		if (genPath(x, y + 1, length - 1)) // south of x,y
			return true;
	case 4:
		if (genPath(x - 1, y, length - 1)) // west of x,y
			return true;
	}
	mapLayout[x][y] = 0;
	return false;
}