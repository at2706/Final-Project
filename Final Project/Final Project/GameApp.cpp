#include "GameApp.h"

GameApp::GameApp() {
	init();
	charSheet = loadTexture("sheet.png");
	tileSheet = loadTexture("arne_sprites.png", GL_NEAREST);
	UISheet = loadTexture("greenSheet.png", GL_NEAREST);
	fontTexture = loadTexture("font1.png");
	state = STATE_MAIN_MENU;
	introMusic = Mix_LoadMUS("introMusic.mp3");
	menuMove = Mix_LoadWAV("menuMove.wav");
	jump = Mix_LoadWAV("jump.wav");
	playerCount = 1;
	
	Mix_PlayMusic(introMusic, -1);
	Mix_VolumeMusic(30);
	SDL_JoystickOpen(0);

	buildMainMenu();
	buildPauseMenu();
	buildUIstatic();
	
	drawLadder(4, 0.0f, 0.0f);
	drawPlatformHorizontal(26,0.0f,-0.5f);

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
	//makeGameLevel();
}

GLvoid GameApp::init() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
	displayWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RESOLUTION_W, RESOLUTION_H, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	glViewport(0, 0, RESOLUTION_W, RESOLUTION_H);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-ASPECT_RATIO_X, ASPECT_RATIO_X, -ASPECT_RATIO_Y, ASPECT_RATIO_Y, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
}

GameApp::~GameApp()
{
	for (GLuint i = 0; i < playerCount; i++){
		SDL_JoystickClose(players[i].controller);
	}

	Mix_FreeMusic(introMusic);
	Mix_FreeChunk(menuMove);
	Mix_FreeChunk(jump);

	SDL_Quit();
}

GLvoid tint(float alpha) {
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_BLEND);


	glLoadIdentity();
	GLfloat quad3[] = { -2.0f, 2.0f, -2.0f, -2.0f, 2.0f, -2.0f, 2.0f, 2.0f };
	glVertexPointer(2, GL_FLOAT, 0, quad3);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat tint[] = { 0.0f, 0.0f, 0.0f, alpha, 0.0f, 0.0f, 0.0f, alpha, 0.0f, 0.0f, 0.0f, alpha, 0.0f, 0.0f, 0.0f, alpha };
	glColorPointer(4, GL_FLOAT, 0, tint);
	glEnableClientState(GL_COLOR_ARRAY);

	vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

GLboolean GameApp::updateAndRender() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			return true;
		}
		/*
		=============					=============
			AXIS							VALUE
		=============					=============
		0: left stick x-axis			Negative: Up, Left
		1: left stick y-axis			Positive: Down, Right
		2: right stick x-axis			
		3: right stick y-axis			*/
		

		switch (state){
		case STATE_MAIN_MENU:
			if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.scancode == SDL_SCANCODE_S){
					mainList->selectionDown();
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_W){
					mainList->selectionUp();
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_RETURN){
					switch (mainList->selection){
					case 0:
						gameStart();
						break;
					case 2:
						return true;
					}
				}
			}
			
			//UP and DOWN the main menu
			if (event.type == SDL_JOYAXISMOTION && event.jaxis.axis == 1 && controllerCooldown > 0.1f) {

				if (event.jaxis.value > CONTROLER_DEAD_ZONE) {
					mainList->selectionDown();
					controllerCooldown = 0;
				}

				else if (event.jaxis.value < -CONTROLER_DEAD_ZONE) {
					mainList->selectionUp();
					controllerCooldown = 0;
				}
			}
			//LEFT and RIGHT for picking the amount of players
			if (event.type == SDL_JOYAXISMOTION && event.jaxis.axis == 0 && controllerCooldown > 0.3f) {
				if (mainList->selection == 1 && event.jaxis.value < -CONTROLER_DEAD_ZONE) {
					if (playerCount > 1){
						playerCount--;
						UIText *t = static_cast<UIText*>(mainList->children[1]);
						t->text = "Players: " + to_string(playerCount);
						controllerCooldown = 0;
					}
				}
				else if (mainList->selection == 1 && event.jaxis.value > CONTROLER_DEAD_ZONE) {
					if (playerCount < SDL_NumJoysticks()){
						playerCount++;
						UIText *t = static_cast<UIText*>(mainList->children[1]);
						t->text = "Players:" + to_string(playerCount);
						controllerCooldown = 0;
					}
				}
			}
			//Pressing A does menu action
			if (event.type == SDL_JOYBUTTONDOWN && event.jbutton.button == 10){
				switch (mainList->selection){
				case 0:
					gameStart();
					break;
				case 2:
					return true;
				}
			}
			break;
		case STATE_GAME_LEVEL:

			if (event.type == SDL_JOYAXISMOTION){
				players[event.jaxis.which].axisValues[event.jaxis.axis] = event.jaxis.value;
				if (event.jaxis.axis > 3)
					cout <<to_string(event.jaxis.axis) << ": " << to_string(event.jaxis.value) << endl;
			}
			

			if (event.type == SDL_JOYBUTTONDOWN && event.jbutton.button == 10){
				Mix_PlayChannel(-1, jump, 0);
				players[event.jaxis.which].hero->velocity.y = 1.5f;
			}
			if (event.type == SDL_JOYBUTTONDOWN && event.jbutton.button == 4){
				state = STATE_GAME_PAUSE;
			}
			break;
		case STATE_GAME_PAUSE:
			if (event.type == SDL_JOYAXISMOTION && event.jaxis.axis == 1 && controllerCooldown > 0.1f) {

				if (event.jaxis.value > CONTROLER_DEAD_ZONE) {
					pauseList->selectionDown();
					controllerCooldown = 0;
				}

				else if (event.jaxis.value < -CONTROLER_DEAD_ZONE) {
					pauseList->selectionUp();
					controllerCooldown = 0;
				}
			}

			if (event.type == SDL_JOYBUTTONDOWN){
				switch (event.jbutton.button){
				//A button
				case 10:
					switch (pauseList->selection){
					case 0:
						state = STATE_GAME_LEVEL;
						break;
					case 1:
						return true;
					}
					break;
					//Start Button
				case 4:
					state = STATE_GAME_LEVEL;
					break;
				}
			}
			break;
		case STATE_GAME_OVER:

			break;
		}

		if (event.type == SDL_JOYBUTTONDOWN){
			cout << to_string(event.jbutton.button) << endl;
		}
	}

	switch (state){
	case STATE_MAIN_MENU:
		controllerCooldown += elapsed;
		break;
	case STATE_GAME_LEVEL:

		if (keys[SDL_SCANCODE_D]){
			players[0].hero->isIdle = false;
			players[0].hero->setRotation(0.0f);

		}
		else if (keys[SDL_SCANCODE_A]){
			players[0].hero->isIdle = false;
			players[0].hero->setRotation(180.0f);

		}
		else players[0].hero->isIdle = true;

		if (keys[SDL_SCANCODE_SPACE])
			players[0].hero->velocity.y = 2.0f;

		/*for (GLuint i = 0; i < playerCount; i++){
			if (players[i].axisValues[0] > CONTROLER_DEAD_ZONE){
				players[i].hero->isIdle = false;
				players[i].hero->setRotation(0.0f);
				players[i].hero->speed = 2 * players[i].axisValues[0] / AXIS_MAX;
			}

			else if (players[i].axisValues[0] < -CONTROLER_DEAD_ZONE){
				players[i].hero->isIdle = false;
				players[i].hero->setRotation(180.0f);
				players[i].hero->speed = -2 * players[i].axisValues[0] / AXIS_MAX;
			}
			else{
				players[i].hero->isIdle = true;
			}
		}*/

		/*if (keys[SDL_SCANCODE_RIGHT]){
			players[1].hero->isIdle = false;
			players[1].hero->setRotation(0.0f);
		}
		else if (keys[SDL_SCANCODE_LEFT]){
			players[1].hero->isIdle = false;
			players[1].hero->setRotation(180.0f);
		}
		else players[1].hero->isIdle = true;

		if (keys[SDL_SCANCODE_UP] && players[1].hero->collidedBottom){
			players[1].hero->velocity.y = 2.0f;
		}*/

		break;
	case STATE_GAME_PAUSE:
		controllerCooldown += elapsed;
		break;
	case STATE_GAME_OVER:

		break;
	}



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

	if (state != STATE_GAME_PAUSE) {
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

}

GLvoid GameApp::Render() {
	
	GLfloat fadeValue;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	zoom = 1.5f;
	switch (state){
	case STATE_MAIN_MENU:
		UImain->render();

		break;
	case STATE_GAME_LEVEL:
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-ASPECT_RATIO_X * zoom, ASPECT_RATIO_X * zoom, -ASPECT_RATIO_Y * zoom, ASPECT_RATIO_Y * zoom, -1.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
		
		followPlayers(players);
		Entity::renderAll();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-ASPECT_RATIO_X, ASPECT_RATIO_X, -ASPECT_RATIO_Y, ASPECT_RATIO_Y, -1.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		UIstatic->render();
		

		/*fadeTime += elapsed;
		fadeValue = mapValue(fadeTime, 0.0f, 2.0f, 0.0f, 1.0f);
		tint(1 - fadeValue);*/
		
		break;
	case STATE_GAME_PAUSE:
		followPlayers(players);
		Entity::renderAll();
		tint(0.5f);
		UIpause->render();
		break;
	case STATE_GAME_OVER:

		break;
	default:
		break;
	}


	SDL_GL_SwapWindow(displayWindow);
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

GLvoid GameApp::initPlayer(int i){
	Sprite *s;
	Entity *e;

	s = new Sprite(tileSheet, 99, 16, 8);
	e = new Entity(s, HERO);

	e->position.x = -0.7f + i * 0.5f;
	players[i].hero = e;
	players[i].controller = SDL_JoystickOpen(i);
}

GLvoid GameApp::gameStart(){
	for (GLuint i = 0; i < playerCount; i++){
		initPlayer(i);
	}
	Mix_HaltMusic();
	state = STATE_GAME_LEVEL;
}
GLvoid GameApp::buildMainMenu(){
	Sprite *s;
	//<SubTexture name="green_panel.png" x="190" y="94" width="100" height="100"/>
	s = new Sprite(UISheet, 512, 256, 190, 94, 100, 100);
	UImain = new UIElement(s, 0.0f, 0.0f, 1.22f, 0.8f);
	UImain->fontTexture = fontTexture;

	//<SubTexture name="green_sliderRight.png" x="339" y="143" width="39" height="31"/>
	s = new Sprite(UISheet, 512, 256, 339, 143, 39, 31);
	mainList = new UIList(s, menuMove, -0.7f, 0.65f);
	mainList->spacing.y = 0.08f;
	UImain->attach(mainList);
	UIText *b;
	b = new UIText("Start");
	b->color = { 1, 1, 1, 1 };
	mainList->attach(b);

	b = new UIText("Players:1");
	b->color = { 1, 1, 1, 1 };
	mainList->attach(b);

	b = new UIText("Quit");
	b->spacing = -0.13f;
	b->color = { 1, 1, 1, 1 };
	mainList->attach(b);

}
GLvoid GameApp::buildPauseMenu(){
	Sprite *s;
	UIText *t;
	//<SubTexture name="green_panel.png" x="190" y="94" width="100" height="100"/>
	s = new Sprite(UISheet, 512, 256, 190, 94, 100, 100);
	UIpause = new UIElement(s, 0.0f, 0.0f, 1.22f, 0.8f);
	UIpause->fontTexture = fontTexture;

	//<SubTexture name="green_button03.png" x="0" y="192" width="190" height="45"/>
	s = new Sprite(UISheet, 512, 256, 0, 192, 190, 45);
	UIElement *pauseTitle = new UIElement(s, 0.0f, 1.1f, 0.4f, 0.6f);
	UIpause->attach(pauseTitle);
	t = new UIText("PAUSE",0.3f,-0.55f);
	t->color = { 1, 1, 1, 1 };
	pauseTitle->attach(t);

	//<SubTexture name="green_sliderRight.png" x="339" y="143" width="39" height="31"/>
	s = new Sprite(UISheet, 512, 256, 339, 143, 39, 31);
	pauseList = new UIList(s, menuMove, -0.7f, 0.4f);
	pauseList->spacing.y = 0.08f;
	UIpause->attach(pauseList);

	t = new UIText("Resume");
	t->spacing = -0.12f;
	t->color = { 1, 1, 1, 1 };
	pauseList->attach(t);

	t = new UIText("Quit");
	t->color = { 1, 1, 1, 1 };
	pauseList->attach(t);
}
GLvoid GameApp::buildUIstatic(){
	Sprite *s;
	UIText *b;

	s = new Sprite(UISheet, 512, 256, 0, 192, 190, 45);
	UIstatic = new UIElement(s, -1.5f, 0.9f,0.55f, 0.5f);
	UIstatic->fontTexture = fontTexture;

	b = new UIText("Lives: ", 0.3f, -0.75f);
	b->color = { 1, 1, 1, 1 };
	UIstatic->attach(b);
}

GLvoid GameApp::followPlayers(Player *p){
	GLfloat posX = 0, posY = 0;
	for (GLuint i = 0; i < playerCount; i++){
		if (i!= 1){
			posX += p[i].hero->position.x;
			posY += p[i].hero->position.y;
		}
	}

	posX /= playerCount;
	posY /= playerCount;
	glTranslatef(-posX, -posY, 0.0f);
}
GLvoid GameApp::drawPlatformHorizontal(GLfloat length, GLfloat x, GLfloat y){
	Sprite *sprite;
	Entity *platform;
	for (GLfloat i = -(length / 2); i < (length / 2); i++){
		sprite = new Sprite(tileSheet, 3, 16, 8);
		platform = new Entity(sprite, PLATFORM);
		platform->setPosition((i * sprite->size.x) + x, y);
	}
}

GLvoid GameApp::drawLadder(GLfloat length, GLfloat x, GLfloat y){
	Sprite *sprite;
	Ladder *ladder;
	for (GLuint i = 0; i < length; i++){
		sprite = new Sprite(tileSheet, 20, 16, 8);
		ladder = new Ladder(sprite, x, y + (sprite->size.y * i));
	}
}

// collision stuff that i used
// NOT DONE YET, levelData needs to  be created which will have to be on me since i need that for procedural generation
/*void GameApp::worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
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
}*/