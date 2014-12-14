#include "GameApp.h"

GameApp::GameApp() {
	init();
	srand(std::time(NULL));
	charSheet = loadTexture("sheet.png");
	tileSheet = loadTexture("arne_sprites.png", GL_NEAREST);
	UISheet = loadTexture("greenSheet.png", GL_NEAREST);
	fontTexture = loadTexture("font1.png");
	state = STATE_MAIN_MENU;
	introMusic = Mix_LoadMUS("introMusic.mp3");
	menuMove = Mix_LoadWAV("menuMove.wav");
	jump = Mix_LoadWAV("jump.wav");
	playerCount = 2;
	
	Mix_PlayMusic(introMusic, -1);
	Mix_VolumeMusic(30);
	SDL_JoystickOpen(0);

	buildMainMenu();
	buildPauseMenu();
	buildGameUI();
	
	drawLadder(8, 0.0f, -0.3f);
	drawPlatformHorizontal(26,0.0f,-0.5f);
	drawPlatformHorizontal(8, 0.62f, 0.45f);

	// keep in mind each tile inside the maplayout is a box of 100x100, so entire game level will be 500x500
	// initialize mapLayout
	for (unsigned int i = 0; i < LAYOUT_X; ++i) {
		for (unsigned int j = 0; j < LAYOUT_Y; ++j) {
			mapLayout[i][j] = 0;
		}
	}
	// for level generation
	mapStart.x = (std::rand() % (4));
	mapStart.y = (std::rand() % (4));
	mapGoal.x = (std::rand() % (4));
	mapGoal.y = (std::rand() % (4));
	// to ensure theres enough space between the start and end point
	while (fabs(mapStart.x - mapGoal.x) + fabs(mapStart.y - mapGoal.y) < 4) {
		mapStart.x = (std::rand() % (4));
		mapStart.y = (std::rand() % (4));
		mapGoal.x = (std::rand() % (4));
		mapGoal.y = (std::rand() % (4));
	}
	makeGameLevel();
	for (unsigned int i = 0; i < LAYOUT_X; ++i) {
		for (unsigned int j = 0; j < LAYOUT_Y; ++j) {
			std::cout << mapLayout[i][j] << " ";
		}
		std::cout << endl;
	}
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
	Entity::world = this;
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
			#pragma region Main Menu
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
		#pragma endregion

		case STATE_GAME_LEVEL:
			#pragma region Game Level
			if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
					state = STATE_GAME_PAUSE;
				}
			}

			if (event.type == SDL_JOYAXISMOTION){
				players[event.jaxis.which].axisValues[event.jaxis.axis] = event.jaxis.value;
				if (event.jaxis.axis > 3)
					cout << to_string(event.jaxis.axis) << ": " << to_string(event.jaxis.value) << endl;
			}


			if (event.type == SDL_JOYBUTTONDOWN && event.jbutton.button == 10){
				Mix_PlayChannel(-1, jump, 0);
				players[event.jaxis.which].hero->velocity.y = 1.5f;
			}
			if (event.type == SDL_JOYBUTTONDOWN && event.jbutton.button == 4){
				state = STATE_GAME_PAUSE;
			}
			break;
#pragma endregion

		case STATE_GAME_PAUSE:
			#pragma region Game Pause
			if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
					state = STATE_GAME_LEVEL;
				}
			}
			
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
#pragma endregion

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

 		if (!players[0].hero->flags.deathMark){
			if (players[0].hero->collision.points[0]){
				players[0].reviveIndicator->setPosition(players[0].target->position.x - 0.08f, players[0].target->position.y + 0.08f);
				players[0].reviveIndicator->isVisible = true;
			}
			else players[0].reviveIndicator->isVisible = false;
			if (keys[SDL_SCANCODE_D]){
				players[0].hero->flags.idle = false;
				players[0].hero->setRotation(0.0f);

			}
			else if (keys[SDL_SCANCODE_A]){
				players[0].hero->flags.idle = false;
				players[0].hero->setRotation(180.0f);
			}

			else players[0].hero->flags.idle = true;

			if (keys[SDL_SCANCODE_W] && !players[0].hero->gravity.enabled){
				players[0].hero->velocity.y = 2.0f;
			}

			else if (keys[SDL_SCANCODE_S] && !players[0].hero->gravity.enabled){
				players[0].hero->velocity.y = -2.0f;
			}

			if (keys[SDL_SCANCODE_SPACE] && players[0].hero->collision.bottom)
				players[0].hero->velocity.y = 6.0f;

			if (keys[SDL_SCANCODE_F])
				players[0].hero->shoot();
		}

		if (!players[1].hero->flags.deathMark){
			if (players[1].hero->collision.points[0]){
				players[1].reviveIndicator->setPosition(players[1].target->position.x -0.08f, players[1].target->position.y + 0.08f);
				players[1].reviveIndicator->isVisible = true;
			}
			else players[1].reviveIndicator->isVisible = false;
			if (keys[SDL_SCANCODE_RIGHT]){
				players[1].hero->flags.idle = false;
				players[1].hero->setRotation(0.0f);

			}
			else if (keys[SDL_SCANCODE_LEFT]){
				players[1].hero->flags.idle = false;
				players[1].hero->setRotation(180.0f);
			}

			else players[1].hero->flags.idle = true;

			if (keys[SDL_SCANCODE_UP] && !players[1].hero->gravity.enabled){
				players[1].hero->velocity.y = 2.0f;
			}

			else if (keys[SDL_SCANCODE_DOWN] && !players[1].hero->gravity.enabled){
				players[1].hero->velocity.y = -2.0f;
			}

			if (keys[SDL_SCANCODE_M] && players[1].hero->collision.bottom)
				players[1].hero->velocity.y = 6.0f;

			if (keys[SDL_SCANCODE_N])
				players[1].hero->shoot();
		}

		/*for (GLuint i = 0; i < playerCount; i++){
			if (players[i].axisValues[0] > CONTROLER_DEAD_ZONE){
				players[i].hero->flags.idle = false;
				players[i].hero->setRotation(0.0f);
				players[i].hero->speed = 2 * players[i].axisValues[0] / AXIS_MAX;
			}

			else if (players[i].axisValues[0] < -CONTROLER_DEAD_ZONE){
				players[i].hero->flags.idle = false;
				players[i].hero->setRotation(180.0f);
				players[i].hero->speed = -2 * players[i].axisValues[0] / AXIS_MAX;
			}
			else{
				players[i].hero->flags.idle = true;
			}
		}*/

		/*if (keys[SDL_SCANCODE_RIGHT]){
			players[1].hero->flags.idle = false;
			players[1].hero->setRotation(0.0f);
		}
		else if (keys[SDL_SCANCODE_LEFT]){
			players[1].hero->flags.idle = false;
			players[1].hero->setRotation(180.0f);
		}
		else players[1].hero->flags.idle = true;

		if (keys[SDL_SCANCODE_UP] && players[1].hero->collision.bottom){
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
	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	zoom = 1.0f;
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
		glMultMatrixf(translateMatrix.ml);
		Entity::renderAll();
		UIGame->render();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-ASPECT_RATIO_X, ASPECT_RATIO_X, -ASPECT_RATIO_Y, ASPECT_RATIO_Y, -1.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		UIStatic->render();
		
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
		glMultMatrixf(translateMatrix.ml);
		Entity::renderAll();
		fadeTime += elapsed;
		fadeValue = mapValue(fadeTime, 0.0f, 2.0f, 0.0f, 0.7f);
		tint(fadeValue);

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

	s = new Sprite(tileSheet, 64, 16, 8);
	e = new Entity(s, HERO);
	e->value = i;
	for (int j = 0; j < 5; j++)
		e->animation.push_back(new Sprite(tileSheet, j+65, 16, 8));

	s = new Sprite(tileSheet, 21, 16, 8);
	Weapon *w = new Weapon(s);
	w->fireRate = 0.1f;
	w->speed = 6.0f;
	w->damage = 5.0f;
	e->weapon = w;

	e->position.x = -0.7f + i * 0.5f;
	players[i].hero = e;

	players[i].reviveIndicator = new UIText("Revive", 0.0f, 0.5f);
	players[i].reviveIndicator->fontTexture = fontTexture;
	players[i].reviveIndicator->isVisible = false;
	UIGame->attach(players[i].reviveIndicator);

	players[i].controller = SDL_JoystickOpen(i);
}

GLvoid GameApp::gameStart(){
	for (GLuint i = 0; i < playerCount; i++){
		initPlayer(i);
	}
	Mix_HaltMusic();
	state = STATE_GAME_LEVEL;
	
	/*Sprite *s;
	Entity *e;
	s = new Sprite(tileSheet, 80, 16, 8);
	e = new Entity(s, SHOOTER, 0.1f, -0.1f);
	e->animation.push_back(new Sprite(tileSheet, 81, 16, 8));
	s = new Sprite(tileSheet, 39, 16, 8);
	Weapon *w = new Weapon(s);
	w->fireRate = 0.8f;
	w->speed = 6.0f;
	w->damage = 30.0f;
	e->weapon = w;*/
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
	t = new UIText("PAUSE",-0.55f);
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
GLvoid GameApp::buildGameUI(){
	Sprite *s;
	UIElement *e;
	UIText *t;
	UIGame = new UIElement();
	UIGame->fontTexture = fontTexture;
	UIStatic = new UIElement();
	s = new Sprite(UISheet, 512, 256, 0, 192, 190, 45);
	e = new UIElement(s, -1.5f, 0.9f,0.55f, 0.5f);
	e->fontTexture = fontTexture;

	t = new UIText("Lives: ", -0.75f);
	t->color = { 1, 1, 1, 1 };
	e->attach(t);

	UIStatic->attach(e);
}

GLvoid GameApp::followPlayers(Player *p){
	GLfloat posX = 0, posY = 0;
	GLuint alive = 0;
	for (GLuint i = 0; i < playerCount; i++){
		if (!p[i].hero->flags.deathMark){
			posX += p[i].hero->position.x;
			posY += p[i].hero->position.y;
			alive++;
		}
	}

	if (alive == 0){
		fadeTime = 0.0f;
		state = STATE_GAME_OVER;
	}

	if (posX != 0 && posY != 0){
		posX /= alive;
		posY /= alive;
		translateMatrix.m[3][0] = lerp(translateMatrix.m[3][0], -posX, elapsed * 3);
		translateMatrix.m[3][1] = lerp(translateMatrix.m[3][1], -posY, elapsed * 3);
	}
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
	Entity *ladder;
	for (GLuint i = 0; i < length; i++){
		sprite = new Sprite(tileSheet, 20, 16, 8);
		ladder = new Entity(sprite, LADDER, x, y + (sprite->size.y * i));
	}
}

// collision stuff that i used
// NOT DONE YET, levelData needs to  be created which will have to be on me since i need that for procedural generation
/*
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
temp->collision.bottom =true;
}
adjust = checkPointForGridCollisionY(temp->position.x, temp->position.y + temp->getHeight() * 0.5f); // same as previous
if (adjust != 0.0f) {
temp->position.y -= adjust - 0.0001f;
temp->velocity.y = 0.0f;
temp->collision.top = true;
}
}

void GameApp::doLevelCollisionX(Entity* temp) {
float adjust = checkPointForGridCollisionX(temp->position.x - temp->getWidth() * 0.5f, temp->position.y); // same as previous
if (adjust != 0.0f) {
temp->position.x -= adjust * TILE_SIZE *0.008f;
temp->velocity.x = 0.0f;
temp->collision.left = true;
}
adjust = checkPointForGridCollisionX(temp->position.x + temp->getWidth() * 0.5f, temp->position.y); // same as previous
if (adjust != 0.0f) {
temp->position.x += adjust * TILE_SIZE * 0.001;
temp->velocity.x = 0.0f;
temp->collision.right = true;
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
*/

// THIS STUFF IS FOR LEVEL GENERATION
void GameApp::makeGameLevel() {
	bool done = false;
	while (!done) { // to make sure the maplayout has a path from start to end of length 8
		for (unsigned int i = 0; i < LAYOUT_X; ++i) { // to clear the maplayout if generating the path failed
			for (unsigned int j = 0; j < LAYOUT_Y; ++j) {
				mapLayout[i][j] = 0;
			}
		}
		// regenerate start and end point
		// this is to create new points
		mapStart.x = (std::rand() % (4));
		mapStart.y = (std::rand() % (4));
		std::cout << "Failed, retrying..." << endl;
		// retry to generate
		if (genPath(mapStart.x, mapStart.y, 9))
			done = true;
	}
	cout << "start x: " << mapStart.x << " start y: " << mapStart.y << endl;
	cout << "start : " << mapLayout[(int)mapStart.x][(int)mapStart.y] << endl;
}

bool GameApp::genPath(int x, int y, int length) {
	// first part of procedural generation
	// I HAVE NOT TESTED THIS, THIS WAS MADE BASED OFF OF PSEUDOCODE
	cout << "x = " << x << " y = " << y << endl;
	if (length < 0)
		return false;
	if (mapLayout[x][y] == 0 && length == 0) {
		if (y + 1 < LAYOUT_Y - 1) {
			if (mapLayout[x][y + 1] == 6 || mapLayout[x][y + 1] == 9 || mapLayout[x][y + 1] == 10) {
				mapLayout[x][y] = 1;
				return true;
			}
		}
		else if (y - 1 > 0) {
			if (mapLayout[x][y - 1] == 6 || mapLayout[x][y - 1] == 7 || mapLayout[x][y - 1] == 8) {
				mapLayout[x][y] = 2;
				return true;
			}
		}
		else if (x + 1 < LAYOUT_X - 1) {
			if (mapLayout[x + 1][y] == 5 || mapLayout[x + 1][y] == 8 || mapLayout[x + 1][y] == 9) {
				mapLayout[x][y] = 3;
				return true;
			}
		}
		else if (x - 1 > 0) {
			if (mapLayout[x - 1][y] == 5 || mapLayout[x - 1][y] == 7 || mapLayout[x - 1][y] == 10) {
				mapLayout[x][y] = 4;
				return true;
			}
		}
		return false; // something went terribly wrong if this returns false!!
	}
	if (mapLayout[x][y] != 0 || x < 0 || y < 0 || x > LAYOUT_X - 1 || y > LAYOUT_Y - 1)
		return false;

	//assigning random layout to current tile
	bool done = false;
	while (!done) {
		switch (1 + (std::rand() % 10)) {
			// 1-4, must check the if the tile for the end point is compatible or not
		case 1:
			if (y + 1 < LAYOUT_Y - 1) {
				if ((x == mapStart.x && y == mapStart.y) || (x == mapGoal.x && y == mapGoal.y && (mapLayout[x][y + 1] == 6 || mapLayout[x][y + 1] == 9 || mapLayout[x][y + 1] == 10)))
					mapLayout[x][y] = 1;
				done = true;
				break;
			}
		case 2:
			if (y - 1 > 0) {
				if ((x == mapStart.x && y == mapStart.y) || (x == mapGoal.x && y == mapGoal.y && (mapLayout[x][y - 1] == 6 || mapLayout[x][y - 1] == 7 || mapLayout[x][y - 1] == 8)))
					mapLayout[x][y] = 2;
				done = true;
				break;
			}
		case 3:
			if (x + 1 < LAYOUT_X - 1) {
				if ((x == mapStart.x && y == mapStart.y) || (x == mapGoal.x && y == mapGoal.y && (mapLayout[x + 1][y] == 5 || mapLayout[x + 1][y] == 8 || mapLayout[x + 1][y] == 9)))
					mapLayout[x][y] = 3;
				done = true;
				break;
			}
		case 4:
			if (x - 1 > 0) {
				if ((x == mapStart.x && y == mapStart.y) || (x == mapGoal.x && y == mapGoal.y && (mapLayout[x - 1][y] == 5 || mapLayout[x - 1][y] == 7 || mapLayout[x - 1][y] == 10)))
					mapLayout[x][y] = 4;
				done = true;
				break;
			}
			// case 5 and 6 are for 2 parallel walls
		case 5:
			if (x - 1 > -1) { // make sure we don't access something outside the array
				if (mapLayout[x - 1][y] == 3 || mapLayout[x - 1][y] == 5 || mapLayout[x - 1][y] == 7 || mapLayout[x - 1][y] == 10) { // make sure left side has an opening
					if (y - 1 > -1) { // make sure we don't access something outside the array
						if (mapLayout[x][y - 1] == 0 || mapLayout[x][y - 1] == 5 || mapLayout[x][y - 1] == 7 || mapLayout[x][y - 1] == 10) { // make sure top is not a door connecting to current tile
							mapLayout[x][y] = 5;
							done = true;
							break;
						}
					}
					else if (y + 1 < LAYOUT_Y) { // make sure we dont access something outside the array
						if (mapLayout[x][y + 1] == 0 || mapLayout[x][y + 1] == 5 || mapLayout[x][y + 1] == 7 || mapLayout[x][y + 1] == 8) { // make sure bottom is not a door connecting to current tile
							mapLayout[x][y] = 5;
							done = true;
							break;
						}
					}
				}
			}
			else if (x + 1 < LAYOUT_X) { // make sure we don't access something outside the array
				if (mapLayout[x + 1][y] == 4 || mapLayout[x + 1][y] == 5 || mapLayout[x + 1][y] == 8 || mapLayout[x + 1][y] == 9) { // make sure right side has an opening
					if (y - 1 > -1) { // make sure we don't access something outside the array
						if (mapLayout[x][y - 1] == 0 || mapLayout[x][y - 1] == 5 || mapLayout[x][y - 1] == 7 || mapLayout[x][y - 1] == 10) { // make sure top is not a door connecting to current tile
							mapLayout[x][y] = 5;
							done = true;
							break;
						}
					}
					else if (y + 1 < LAYOUT_Y) { // make sure we dont access something outside the array
						if (mapLayout[x][y + 1] == 0 || mapLayout[x][y + 1] == 5 || mapLayout[x][y + 1] == 7 || mapLayout[x][y + 1] == 8) { // make sure bottom is not a door connecting to current tile
							mapLayout[x][y] = 5;
							done = true;
							break;
						}
					}
				}
			}
		case 6:
			if (y - 1 > -1) { // make sure we don't access something outside the array
				if (mapLayout[x][y - 1] == 1 || mapLayout[x][y - 1] == 6 || mapLayout[x][y - 1] == 7 || mapLayout[x][y - 1] == 8) { // make sure top side has an opening
					if (x - 1 > -1) { // make sure we don't access something outside the array
						if (mapLayout[x - 1][y] == 0 || mapLayout[x - 1][y] == 6 || mapLayout[x - 1][y] == 8 || mapLayout[x - 1][y] == 9) { // make sure left is not a door connecting to current tile
							mapLayout[x][y] = 6;
							done = true;
							break;
						}
					}
					else if (x + 1 < LAYOUT_X) { // make sure we dont access something outside the array
						if (mapLayout[x + 1][y] == 0 || mapLayout[x + 1][y] == 6 || mapLayout[x + 1][y] == 7 || mapLayout[x + 1][y] == 10) { // make sure right is not a door connecting to current tile
							mapLayout[x][y] = 6;
							done = true;
							break;
						}
					}
				}
			}
			else if (y + 1 < LAYOUT_Y) {
				if (mapLayout[x][y - 1] == 1 || mapLayout[x][y - 1] == 6 || mapLayout[x][y - 1] == 7 || mapLayout[x][y - 1] == 8) { // make sure top side has an opening
					if (x - 1 > -1) { // make sure we don't access something outside the array
						if (mapLayout[x - 1][y] == 0 || mapLayout[x - 1][y] == 6 || mapLayout[x - 1][y] == 8 || mapLayout[x - 1][y] == 9) { // make sure left is not a door connecting to current tile
							mapLayout[x][y] = 6;
							done = true;
							break;
						}
					}
					else if (x + 1 < LAYOUT_X) { // make sure we dont access something outside the array
						if (mapLayout[x + 1][y] == 0 || mapLayout[x + 1][y] == 6 || mapLayout[x + 1][y] == 7 || mapLayout[x + 1][y] == 10) { // make sure right is not a door connecting to current tile
							mapLayout[x][y] = 6;
							done = true;
							break;
						}
					}
				}
			}
			// case 7-10 are for corners
		case 7:
			if (y + 1 < LAYOUT_Y) { // make sure south of tile is still in range
				if (mapLayout[x][y + 1] == 2 || mapLayout[x][y + 1] == 6 || mapLayout[x][y + 1] == 9 || mapLayout[x][y + 1] == 10) { // make sure south has opening connecting to current tile
					mapLayout[x][y] = 7;
					done = true;
					break;
				}
			}
			else if (x + 1 < LAYOUT_X) { // make sure east of tile is still in range
				if (mapLayout[x + 1][y] == 4 || mapLayout[x + 1][y] == 5 || mapLayout[x + 1][y] == 8 || mapLayout[x + 1][y] == 9) {
					mapLayout[x][y] = 7;
					done = true;
					break;
				}
			}
		case 8:
			if (x - 1 > -1) { // make sure west of tile is still in range
				if (mapLayout[x - 1][y] == 3 || mapLayout[x - 1][y] == 5 || mapLayout[x - 1][y] == 7 || mapLayout[x - 1][y] == 10) {
					mapLayout[x][y] = 8;
					done = true;
					break;
				}
			}
			else if (y + 1 < LAYOUT_Y) { // make sure south of tile is still in range
				if (mapLayout[x][y + 1] == 2 || mapLayout[x][y + 1] == 6 || mapLayout[x][y + 1] == 9 || mapLayout[x][y + 1] == 10) { // make sure south has opening connecting to current tile
					mapLayout[x][y] = 8;
					done = true;
					break;
				}
			}
		case 9:
			if (x - 1 > -1) { // make sure west of tile is still in range
				if (mapLayout[x - 1][y] == 3 || mapLayout[x - 1][y] == 5 || mapLayout[x - 1][y] == 7 || mapLayout[x - 1][y] == 10) {
					mapLayout[x][y] = 9;
					done = true;
					break;
				}
			}
			else if (y - 1 > -1) { // make sure north of tile is still in range
				if (mapLayout[x][y - 1] == 1 || mapLayout[x][y - 1] == 6 || mapLayout[x][y - 1] == 7 || mapLayout[x][y - 1] == 8) {
					mapLayout[x][y] = 9;
					done = true;
					break;
				}
			}
		case 10:
			if (y - 1 > -1) { // make sure north of tile is still in range
				if (mapLayout[x][y - 1] == 1 || mapLayout[x][y - 1] == 6 || mapLayout[x][y - 1] == 7 || mapLayout[x][y - 1] == 8) {
					mapLayout[x][y] = 10;
					done = true;
					break;
				}
			}
			else if (x + 1 < LAYOUT_X) { // make sure east of tile is still in range
				if (mapLayout[x + 1][y] == 4 || mapLayout[x + 1][y] == 5 || mapLayout[x + 1][y] == 8 || mapLayout[x + 1][y] == 9) {
					mapLayout[x][y] = 10;
					done = true;
					break;
				}
			}
		}
	}

	// moving onwards to the next part of the path
	switch (mapLayout[x][y]) {
	case 1:
		if (y + 1 < LAYOUT_Y) {
			if (genPath(x, y + 1, length - 1)) // south of x,y
				return true;
		}
		else
			return false;
	case 2:
		if (y - 1 > -1) {
			if (genPath(x, y - 1, length - 1)) // north of x,y
				return true;
		}
		else
			return false;
	case 3:
		if (x + 1 < LAYOUT_X) {
			if (genPath(x + 1, y, length - 1)) // east of x,y
				return true;
		}
		else
			return false;
	case 4:
		if (x - 1 > -1) {
			if (genPath(x - 1, y, length - 1)) // west of x,y
				return true;
		}
		else
			return false;
	case 5:
		if (genPath(x + 1, y, length - 1)) // east of x,y
			return true;
		else if (genPath(x - 1, y, length - 1)) // west of x,y
			return true;
	case 6:
		if (genPath(x, y - 1, length - 1)) // north of x,y
			return true;
		else if (genPath(x, y + 1, length - 1)) // south of x,y
			return true;
	case 7:
		switch (1 + std::rand() % 2) {
		case 1:
			if (genPath(x + 1, y, length - 1)) // east of x,y
				return true;
		case 2:
			if (genPath(x, y + 1, length - 1)) // south of x,y
				return true;
		}
	case 8:
		switch (1 + std::rand() % 2) {
		case 1:
			if (genPath(x - 1, y, length - 1)) // west of x,y
				return true;
		case 2:
			if (genPath(x, y + 1, length - 1)) // south of x,y
				return true;
		}
	case 9:
		switch (1 + std::rand() % 2) {
		case 1:
			if (genPath(x - 1, y, length - 1)) // west of x,y
				return true;
		case 2:
			if (genPath(x, y - 1, length - 1)) // north of x,y
				return true;
		}
	case 10:
		switch (1 + std::rand() % 2) {
		case 1:
			if (genPath(x + 1, y, length - 1)) // east of x,y
				return true;
		case 2:
			if (genPath(x, y - 1, length - 1)) // north of x,y
				return true;
		}
	}
	mapLayout[x][y] = 0;
	return false;
}

void GameApp::readMap(string map, unsigned char **tmpMap) {
	ifstream infile(map);
	string line;
	while (getline(infile, line)) {
		if (line == "[header]") {
			if (!readHeader(infile, tmpMap)) {
				return;
			}
		}
		else if (line == "[layer]") {
			readLayerData(infile, tmpMap);
		}
		else if (line == "[Objects]") {
			readEntityData(infile, tmpMap);
		}
	}
}

bool GameApp::readHeader(ifstream& stream, unsigned char **tmpMap) {
	string line;
	mapWidth = -1;
	mapHeight = -1;
	while (getline(stream, line)) {
		if (line == "") { break; }

		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);

		if (key == "width") {
			mapWidth = atoi(value.c_str());
		}
		else if (key == "height"){
			mapHeight = atoi(value.c_str());
		}
	}

	if (mapWidth == -1 || mapHeight == -1) {
		return false;
	}
	else { // allocate our map data
		tmpMap = new unsigned char*[mapHeight];
		for (int i = 0; i < mapHeight; ++i) {
			tmpMap[i] = new unsigned char[mapWidth];
		}
		return true;
	}
}

bool GameApp::readLayerData(ifstream& stream, unsigned char **tmpMap) {
	string line;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < mapHeight; y++) {
				getline(stream, line);
				istringstream lineStream(line);
				string tile;
				for (int x = 0; x < mapWidth; x++) {
					getline(lineStream, tile, ',');
					unsigned char val = (unsigned char)atoi(tile.c_str());
					if (val > 0) {
						// be careful, the tiles in this format are indexed from 1 not 0
						tmpMap[y][x] = val - 1;
					}
					else {
						tmpMap[y][x] = 12;
					}
				}
			}
		}
	}
	return true;
}

bool GameApp::readEntityData(ifstream& stream, unsigned char **tmpMap) {
	string line;
	string type;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "type") {
			type = value;
		}
		else if (key == "location") {
			istringstream lineStream(value);
			string xPosition, yPosition;
			getline(lineStream, xPosition, ',');
			getline(lineStream, yPosition, ',');
			float placeX = atoi(xPosition.c_str()) / 16 * TILE_SIZE;
			float placeY = atoi(yPosition.c_str()) / 16 * -TILE_SIZE;
			placeEntity(type, placeX, placeY);
		}
	}
	return true;
}

void GameApp::placeEntity(string type, float placeX, float placeY) {
	if (type == "Start")  {
		startPoint.x = placeX;
		startPoint.y = placeY;
	}
}

void GameApp::createMap() {
	unsigned char** unwalkable;
	unsigned char** firstRoom;
	unsigned char** secondRoom;
	unsigned char** thirdRoom;
	unsigned char** fourthRoom;
	unsigned char** fifthRoom;
	unsigned char** sixthRoom;
	unsigned char** seventhRoom;
	unsigned char** eigthRoom;
	unsigned char** ninthRoom;
	unsigned char** tenthRoom;

	vector<string> firstRoomVariation;
	vector<string> secondRoomVariation;
	vector<string> thirdRoomVariation;
	vector<string> fourthRoomVariation;
	vector<string> fifthRoomVariation;
	vector<string> sixthRoomVariation;
	vector<string> seventhRoomVariation;
	vector<string> eigthRoomVariation;
	vector<string> ninthRoomVariation;
	vector<string> tenthRoomVariation;
	vector<vector<string>> chooseFrom;
	vector<string> tenRooms;

	for (int i = 1; i < 5; ++i) {
		firstRoomVariation.push_back("1_var_" + to_string(i));
	}
	for (int i = 1; i < 5; ++i) {
		secondRoomVariation.push_back("2_var_" + to_string(i));
	}
	for (int i = 1; i < 5; ++i) {
		thirdRoomVariation.push_back("3_var_" + to_string(i));
	}
	for (int i = 1; i < 5; ++i) {
		fourthRoomVariation.push_back("4_var_" + to_string(i));
	}
	for (int i = 1; i < 5; ++i) {
		fifthRoomVariation.push_back("5_var_" + to_string(i));
	}
	for (int i = 1; i < 5; ++i) {
		sixthRoomVariation.push_back("6_var_" + to_string(i));
	}
	for (int i = 1; i < 5; ++i) {
		seventhRoomVariation.push_back("7_var_" + to_string(i));
	}
	for (int i = 1; i < 5; ++i) {
		eigthRoomVariation.push_back("8_var_" + to_string(i));
	}
	for (int i = 1; i < 5; ++i) {
		ninthRoomVariation.push_back("9_var_" + to_string(i));
	}
	for (int i = 1; i < 5; ++i) {
		tenthRoomVariation.push_back("10_var_" + to_string(i));
	}
	// to fill the 0th index
	vector<string> filler;
	chooseFrom.push_back(firstRoomVariation);
	chooseFrom.push_back(secondRoomVariation);
	chooseFrom.push_back(thirdRoomVariation);
	chooseFrom.push_back(fourthRoomVariation);
	chooseFrom.push_back(fifthRoomVariation);
	chooseFrom.push_back(sixthRoomVariation);
	chooseFrom.push_back(seventhRoomVariation);
	chooseFrom.push_back(eigthRoomVariation);
	chooseFrom.push_back(ninthRoomVariation);
	chooseFrom.push_back(tenthRoomVariation);

	// rooms will be assigned from left to right, top to bottom of mapLayout, NOT BASED ON START TO END
	// choose the room
	string realFirstRoom = firstRoomVariation[rand() % 5 + 1];
	string realSecondRoom = secondRoomVariation[rand() % 5 + 1];
	string realThirdRoom = thirdRoomVariation[rand() % 5 + 1];
	string realFourthRoom = fourthRoomVariation[rand() % 5 + 1];
	string realFifthRoom = fifthRoomVariation[rand() % 5 + 1];
	string realSixthRoom = sixthRoomVariation[rand() % 5 + 1];
	string realSeventhRoom = seventhRoomVariation[rand() % 5 + 1];
	string realEigthRoom = eigthRoomVariation[rand() % 5 + 1];
	string realNinthRoom = ninthRoomVariation[rand() % 5 + 1];
	string realTenthRoom = tenthRoomVariation[rand() % 5 + 1];
	for (int i = 0; i < LAYOUT_X; ++i) {
		for (int j = 0; j < LAYOUT_Y; ++j) {
			if (mapLayout[i][j] != 0) {
				switch (mapLayout[i][j]) {
				case 1:
					tenRooms.push_back(firstRoomVariation[rand() % 5 + 1]);
					break;
				}
			}
		}
	}

	/*
	readMap(realFirstRoom, firstRoom);
	readMap(realSecondRoom, secondRoom);
	readMap(realThirdRoom, thirdRoom);
	readMap(realFourthRoom, fourthRoom);
	readMap(realFifthRoom, fifthRoom);
	readMap(realSixthRoom, sixthRoom);
	readMap(realSeventhRoom, seventhRoom);
	readMap(realEigthRoom, eigthRoom);
	readMap(realNinthRoom, ninthRoom);
	readMap(realTenthRoom, tenthRoom);
	*/
	// at this point firstRoom - tenthRoom should have the map info stored
}