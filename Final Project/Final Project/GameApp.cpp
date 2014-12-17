#include "GameApp.h"

GameApp::GameApp() {
	init();
	srand(std::time(NULL));
	charSheet = loadTexture("Samus.png");
	tileSheet = loadTexture("arne_sprites.png", GL_NEAREST);
	UISheet = loadTexture("greenSheet.png", GL_NEAREST);
	gameTile = loadTexture("dirt-tiles.png", GL_NEAREST);
	fontTexture = loadTexture("font1.png");
	state = STATE_MAIN_MENU;
	introMusic = Mix_LoadMUS("introMusic.mp3");
	menuMove = Mix_LoadWAV("menuMove.wav");
	jump = Mix_LoadWAV("jump.wav");
	hurt = Mix_LoadWAV("hurt.wav");
	shootSound = Mix_LoadWAV("shoot.wav");
	healSound = Mix_LoadWAV("heal.ogg");
	reviveSound = Mix_LoadWAV("revive.ogg");
	pickupSound = Mix_LoadWAV("pickup.wav");
	playerCount = 2;
	
	Mix_PlayMusic(introMusic, -1);
	Mix_VolumeMusic(30);
	SDL_JoystickOpen(0);

	buildMainMenu();
	buildPauseMenu();
	buildGameUI();

	// keep in mind each tile inside the maplayout is a box of 50x50, so entire game level will be 250x250
	// initialize mapLayout
	for (unsigned int i = 0; i < LAYOUT_X; ++i) {
		for (unsigned int j = 0; j < LAYOUT_Y; ++j) {
			mapLayout[j][i] = 0;
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
			std::cout << mapLayout[j][i] << " ";
		}
		std::cout << endl;
	}

	createMap();

	drawLadder(16, startPoint.x, startPoint.y);
	drawPlatformHorizontal(42, startPoint.x, startPoint.y - 0.5f);
	drawPlatformHorizontal(8, startPoint.x + 0.65f, startPoint.y + 0.45f);

	animHeroRun = new vector < Sprite* >;
	animHeroRun->insert(animHeroRun->end(),
	{	new Sprite(charSheet, 2048.0f, 1024.0f, 97, 770, 95, 110),	//1		114
		new Sprite(charSheet, 2048.0f, 1024.0f, 719, 344, 77, 110),	//2		120
		new Sprite(charSheet, 2048.0f, 1024.0f, 555, 348, 82, 112),	//3		172
		new Sprite(charSheet, 2048.0f, 1024.0f, 195, 454, 92, 110),	//4		121
		new Sprite(charSheet, 2048.0f, 1024.0f, 0, 104, 100, 110),	//5		123
		new Sprite(charSheet, 2048.0f, 1024.0f, 0, 658, 97, 110),	//6		124
		new Sprite(charSheet, 2048.0f, 1024.0f, 720, 567, 75, 110),	//7		126
		new Sprite(charSheet, 2048.0f, 1024.0f, 469, 694, 85, 110),	//8		127
		new Sprite(charSheet, 2048.0f, 1024.0f, 195, 228, 92, 110),	//9		125
		new Sprite(charSheet, 2048.0f, 1024.0f, 0, 437, 98, 110)	//10	109
	});

	animHeroIdle = new vector < Sprite* >;
	animHeroIdle->insert(animHeroIdle->end(),
	{	new Sprite(charSheet, 2048.0f, 1024.0f, 1019, 206, 67, 110),	//2		117
		new Sprite(charSheet, 2048.0f, 1024.0f, 1017, 876, 67, 110),	//3		141
		new Sprite(charSheet, 2048.0f, 1024.0f, 1087, 542, 67, 110),	//4		155
		new Sprite(charSheet, 2048.0f, 1024.0f, 876, 0, 67, 110)		//5		156
	});

	animHeroJump = new vector < Sprite* >;
	animHeroJump->push_back(new Sprite(charSheet, 2048.0f, 1024.0f, 195, 454, 92, 110));

	animHeroDead = new vector < Sprite* >;
	animHeroDead->insert(animHeroDead->end(),
	{ new Sprite(charSheet, 2048.0f, 1024.0f, 1509, 469, 42, 42),		//0		17
		new Sprite(charSheet, 2048.0f, 1024.0f, 1509, 425, 42, 42),		//1		06
		new Sprite(charSheet, 2048.0f, 1024.0f, 1609, 925, 42, 42),		//2		24
		new Sprite(charSheet, 2048.0f, 1024.0f, 1610, 166, 42, 42),		//3		25
		new Sprite(charSheet, 2048.0f, 1024.0f, 1609, 969, 42, 42),		//4		23
		new Sprite(charSheet, 2048.0f, 1024.0f, 1609, 881, 42, 42),		//5		19
		new Sprite(charSheet, 2048.0f, 1024.0f, 948, 428, 42, 42),		//6		21
		new Sprite(charSheet, 2048.0f, 1024.0f, 1560, 777, 42, 42)		//7		18
	});

	animHeroCrouch = new vector < Sprite* >;
	animHeroCrouch->insert(animHeroCrouch->end(),
	{ new Sprite(charSheet, 2048.0f, 1024.0f, 1342, 233, 55, 83),		//0		47
		new Sprite(charSheet, 2048.0f, 1024.0f, 1341, 488, 55, 83),		//1		51
		new Sprite(charSheet, 2048.0f, 1024.0f, 1341, 750, 55, 83),		//2		53
	});

	pickup = new vector < Sprite* >;
	pickup->insert(pickup->end(),
	{ new Sprite(charSheet, 2048.0f, 1024.0f, 1654, 127, 34, 34),
	new Sprite(charSheet, 2048.0f, 1024.0f, 1689, 127, 34, 34),
	new Sprite(charSheet, 2048.0f, 1024.0f, 1724, 127, 34, 34),
	new Sprite(charSheet, 2048.0f, 1024.0f, 1759, 127, 34, 34),
	new Sprite(charSheet, 2048.0f, 1024.0f, 1798, 127, 34, 34),
	new Sprite(charSheet, 2048.0f, 1024.0f, 1838, 127, 34, 34),
	});

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
	Mix_FreeChunk(hurt);
	Mix_FreeChunk(shootSound);
	Mix_FreeChunk(healSound);
	Mix_FreeChunk(reviveSound);
	Mix_FreeChunk(pickupSound);

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
			if (event.type == SDL_JOYAXISMOTION && event.jaxis.axis == 1 && cooldown > 0.1f) {

				if (event.jaxis.value > CONTROLER_DEAD_ZONE) {
					mainList->selectionDown();
					cooldown = 0;
				}

				else if (event.jaxis.value < -CONTROLER_DEAD_ZONE) {
					mainList->selectionUp();
					cooldown = 0;
				}
			}
			//LEFT and RIGHT for picking the amount of players
			if (event.type == SDL_JOYAXISMOTION && event.jaxis.axis == 0 && cooldown > 0.3f) {
				if (mainList->selection == 1 && event.jaxis.value < -CONTROLER_DEAD_ZONE) {
					if (playerCount > 1){
						playerCount--;
						UIText *t = static_cast<UIText*>(mainList->children[1]);
						t->text = "Players: " + to_string(playerCount);
						cooldown = 0;
					}
				}
				else if (mainList->selection == 1 && event.jaxis.value > CONTROLER_DEAD_ZONE) {
					if (playerCount < SDL_NumJoysticks()){
						playerCount++;
						UIText *t = static_cast<UIText*>(mainList->children[1]);
						t->text = "Players:" + to_string(playerCount);
						cooldown = 0;
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
			
			if (event.type == SDL_JOYAXISMOTION && event.jaxis.axis == 1 && cooldown > 0.1f) {

				if (event.jaxis.value > CONTROLER_DEAD_ZONE) {
					pauseList->selectionDown();
					cooldown = 0;
				}

				else if (event.jaxis.value < -CONTROLER_DEAD_ZONE) {
					pauseList->selectionUp();
					cooldown = 0;
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
		cooldown += elapsed;
		break;
	case STATE_GAME_LEVEL:
		reviveCooldown += elapsed;
		if (players[0].hero != nullptr && !players[0].hero->flags.deathMark){
			if (players[0].hero->collision.points[0]){
				players[0].reviveIndicator->setPosition(players[0].target->position.x - 0.08f, players[0].target->position.y + 0.08f);
				players[0].reviveIndicator->isVisible = true;
			}
			else players[0].reviveIndicator->isVisible = false;
			if (keys[SDL_SCANCODE_D]){
				players[0].hero->flags.idle = false;
				players[0].hero->setRotation(0.0f);
				players[0].hero->animation = animHeroRun;
			}
			else if (keys[SDL_SCANCODE_A]){
				players[0].hero->flags.idle = false;
				players[0].hero->setRotation(180.0f);
				players[0].hero->animation = animHeroRun;
			}

			else{
				players[0].hero->flags.idle = true;
				players[0].hero->animation = animHeroIdle;
			}

			if (keys[SDL_SCANCODE_W] && !players[0].hero->gravity.enabled){
				players[0].hero->velocity.y = 1.0f;
			}

			else if (keys[SDL_SCANCODE_S] && !players[0].hero->gravity.enabled){
				players[0].hero->velocity.y = -1.0f;
			}

			if (keys[SDL_SCANCODE_SPACE] && players[0].hero->collision.bottom){
				players[0].hero->velocity.y = 4.0f;
				Mix_PlayChannel(-1, jump, 0);
			}

			if (players[0].hero->collision.points[0] && keys[SDL_SCANCODE_R] && reviveCooldown > 0.5f){
				players[0].target->modHealth(50);
				Mix_PlayChannel(-1, healSound, 0);
				if (players[0].target->health >= players[0].target->healthMax){
					players[0].target->flags.deathMark = false;
					players[0].target->animation = animHeroIdle;
					lives--;
					lifeIndicator->text = "x" + to_string(lives);
					Mix_PlayChannel(-1, reviveSound, 0);
				}

				reviveCooldown = 0.0f;
			}

			else if (keys[SDL_SCANCODE_F])
				players[0].hero->shoot();

			if (keys[SDL_SCANCODE_C]){
				players[0].hero->animation = animHeroCrouch;
				players[0].hero->flags.idle = true;
			}

			/*if (!players[0].hero->collision.bottom)
				players[0].hero->animation = animHeroJump;*/
		}

		if (players[1].hero != nullptr && !players[1].hero->flags.deathMark){
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

		for (GLuint i = 0; i < playerCount; i++){
			if (players[i].hero != nullptr && !players[i].hero->flags.deathMark){
				players[i].label->setPosition(players[i].hero->position.x - 0.01f, players[i].hero->position.y +0.175f);
				players[i].label->isVisible = true;
			}
			else players[i].label->isVisible = false;
		}
		fillSmallArray();
		break;
	case STATE_GAME_PAUSE:
		cooldown += elapsed;
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
	switch (state){
	case STATE_MAIN_MENU:
		UImain->render();
		break;

	case STATE_GAME_LEVEL: {
		followPlayers(players);
		glMultMatrixf(translateMatrix.ml);
		renderGameLevel();
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
	}
	case STATE_GAME_PAUSE:
		followPlayers(players);
		glMultMatrixf(translateMatrix.ml);
		renderGameLevel();
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
	e = new Entity(animHeroIdle, HERO);
	e->value = i;
	e->scale.x = 1.75f;
	e->scale.y = 1.75f;

	s = new Sprite(tileSheet, 21, 16, 8);
	Weapon *w = new Weapon(s);
	w->fireRate = 0.1f;
	w->speed = 4.0f;
	w->damage = 5.0f;
	e->weapon = w;

	e->position.x = startPoint.x -0.7f + i * 0.5f;
	e->position.y = startPoint.y;
	players[i].hero = e;

	players[i].reviveIndicator = new UIText("Revive", 0.0f, 0.5f);
	players[i].reviveIndicator->fontTexture = fontTexture;
	players[i].reviveIndicator->isVisible = false;

	players[i].label = new UIText("P" + to_string(i + 1), 0.0f, 0.5f, 0.25f);
	players[i].label->fontTexture = fontTexture;
	players[i].label->spacing = -0.1f;

	s = new Sprite(charSheet, 2048.0f, 1024.0f, 1656, 0, 128, 128);
	players[i].positionLabel = new UIElement(s);
	players[i].positionLabel->setScale(0.75f, 0.75f);

	UIGame->attach(players[i].reviveIndicator);
	UIGame->attach(players[i].label);
	UIStatic->attach(players[i].positionLabel);

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
	UIElement *e2;
	UIGame = new UIElement();
	UIGame->fontTexture = fontTexture;

	UIStatic = new UIElement();
	UIStatic->fontTexture = fontTexture;
	s = new Sprite(UISheet, 512, 256, 0, 192, 190, 45);
	e = new UIElement(s, -1.60f, 0.78f, 0.31f, 0.5f);
	UIStatic->attach(e);
	s = new Sprite(charSheet, 2048.0f, 1024.0f, 1656, 0, 128, 128);
	e2 = new UIElement(s, -0.75f);
	lifeIndicator = new UIText("x" + to_string(lives));
	lifeIndicator->color = { 1, 1, 1, 1 };
	e->attach(e2);
	e->attach(lifeIndicator);

	s = new Sprite(UISheet, 512, 256, 0, 192, 190, 45);
	e = new UIElement(s, -1.50f, 0.9f, 0.7f, 0.5f);
	UIStatic->attach(e);
	scoreIndicator = new UIText("Score: 0", -0.82f);
	e->attach(scoreIndicator);
}

bool sortPositionX(Entity *e1, Entity *e2){
	return e1->position.x < e2->position.x;
}
bool sortPositionY(Entity *e1, Entity *e2){
	return e1->position.y < e2->position.y;
}
GLvoid GameApp::followPlayers(Player *p){
	GLfloat avgX = 0, avgY = 0;
	GLuint alive = 0;
	deque<Entity*> heroes;
	vector<Entity*> outOfBounds;
	for (GLuint i = 0; i < playerCount; i++){
		p[i].positionLabel->isVisible = false;
		heroes.push_back(p[i].hero);
		if (p[i].hero != nullptr && !p[i].hero->flags.deathMark){
			alive++;
		}
	}

	if (alive == 0){
		fadeTime = 0.0f;
		state = STATE_GAME_OVER;
	}
	if (alive > 1) {
		sort(heroes.begin(), heroes.end(), sortPositionX);
		GLfloat distanceX = fabs(heroes.back()->position.x - heroes.front()->position.x);
		cooldown += elapsed;
		if (distanceX > 3.5f){
			GLfloat distance1 = fabs(heroes.begin()[1]->position.x - heroes.front()->position.x);
			GLfloat distance2 = fabs(heroes.back()->position.x - heroes.rbegin()[1]->position.x);
			if (distance1 > distance2){
				if (cooldown > 2){
					heroes.front()->modHealth(-15);
					Mix_PlayChannel(-1, hurt, 0);
					cooldown = 0.0f;
				}
				outOfBounds.push_back(heroes.front());
				heroes.pop_front();
			}
			else{
				if (cooldown > 2){
					heroes.back()->modHealth(-15);
					Mix_PlayChannel(-1, hurt, 0);
					cooldown = 0.0f;
				}
				outOfBounds.push_back(heroes.back());
				heroes.pop_back();
			}
		}

		sort(heroes.begin(), heroes.end(), sortPositionY);
		GLfloat distanceY = fabs(heroes.back()->position.y - heroes.front()->position.y);
		if (distanceY > 1.8f){
			GLfloat distance1 = fabs(heroes.begin()[1]->position.y - heroes.front()->position.y);
			GLfloat distance2 = fabs(heroes.back()->position.y - heroes.rbegin()[1]->position.y);
			if (distance1 > distance2){
				if (cooldown > 2){
					heroes.front()->modHealth(-15);
					Mix_PlayChannel(-1, hurt, 0);
					cooldown = 0.0f;
				}
				outOfBounds.push_back(heroes.front());
				heroes.pop_front();
			}
			else{
				if (cooldown > 2){
					heroes.back()->modHealth(-15);
					Mix_PlayChannel(-1, hurt, 0);
					cooldown = 0.0f;
				}
				outOfBounds.push_back(heroes.back());
				heroes.pop_back();
			}
		}
	}
	for (GLuint i = 0; i < playerCount; i++){
		if (p[i].hero != nullptr &&
			(p[i].hero->position.x > -translateMatrix.m[3][0] + ASPECT_RATIO_X || p[i].hero->position.x < -translateMatrix.m[3][0] - ASPECT_RATIO_X
			|| p[i].hero->position.y > -translateMatrix.m[3][1] + ASPECT_RATIO_Y || p[i].hero->position.y < -translateMatrix.m[3][1] - ASPECT_RATIO_Y)){
			outOfBounds.push_back(p[i].hero);
		}
	}

	for (vector<Entity*>::iterator it = outOfBounds.begin(); it != outOfBounds.end(); ++it){
		p[(*it)->value].positionLabel->isVisible = true;
		GLfloat LabelX = (*it)->position.x + translateMatrix.m[3][0];
		if (LabelX > ASPECT_RATIO_X - 0.08f) LabelX = ASPECT_RATIO_X - 0.08f;
		else if (LabelX < -ASPECT_RATIO_X + 0.08f) LabelX = -ASPECT_RATIO_X + 0.08f;
		GLfloat LabelY = (*it)->position.y + translateMatrix.m[3][1];
		if (LabelY > ASPECT_RATIO_Y - 0.08f) LabelY = ASPECT_RATIO_Y - 0.08f;
		else if (LabelY < -ASPECT_RATIO_Y + 0.08f) LabelY = -ASPECT_RATIO_Y + 0.08f;
		p[(*it)->value].positionLabel->setPosition(LabelX, LabelY);
	}

	for (GLuint i = 0; i < heroes.size(); i++){
		if (p[i].hero != nullptr && !heroes[i]->flags.deathMark){
			avgX += heroes[i]->position.x;
			avgY += heroes[i]->position.y;
		}
	}

	if (avgX != 0 && avgY != 0){
		avgX /= alive;
		avgY /= alive;
		translateMatrix.m[3][0] = lerp(translateMatrix.m[3][0], -avgX, elapsed * 4);
		translateMatrix.m[3][1] = lerp(translateMatrix.m[3][1], -avgY, elapsed * 4);
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
	cout << "x = " << x << " y = " << y << endl;
	if (length < 0 || x == LAYOUT_X || y == LAYOUT_Y)
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

void GameApp::readMap(string map, unsigned int **&tmpMap, int xOffset, int yOffset) {
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
		else if (line == "[Object Layer]") {
			readEntityData(infile, tmpMap, xOffset, yOffset);
		}
	}
}

bool GameApp::readHeader(ifstream& stream, unsigned int **&tmpMap) {
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
		tmpMap = new unsigned int*[mapHeight];
		for (int i = 0; i < mapHeight; ++i) {
			tmpMap[i] = new unsigned int[mapWidth];
		}
		return true;
	}
}

bool GameApp::readLayerData(ifstream& stream, unsigned int **&tmpMap) {
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
					unsigned int val = atoi(tile.c_str());
					if (val > 0) {
						// be careful, the tiles in this format are indexed from 1 not 0
						tmpMap[y][x] = val - 1;
					}
					else {
						tmpMap[y][x] = 404;
					}
				}
			}
		}
	}
	return true;
}

bool GameApp::readEntityData(ifstream& stream, unsigned int **&tmpMap, int xOffset, int yOffset) {
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
			placeEntity(type, placeX, placeY, xOffset, yOffset);
		}
	}
	return true;
}

void GameApp::placeEntity(string type, float placeX, float placeY, int xOffset, int yOffset) {
	if (type == "Start")  {
		startPoint.x = (xOffset * TILE_SIZE) + placeX;
		startPoint.y = (yOffset * -TILE_SIZE) + placeY;
	}
}

void GameApp::createMap() {
	int xOffset = 0;
	int yOffset = 0;

	// filling in the case 0 room
	readMap("Map Files/0.txt", unwalkable, 0, 0);

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

	for (int i = 1; i < 5; ++i) {
		firstRoomVariation.push_back("Map Files/1_var_" + to_string(i) + ".txt");
	}
	for (int i = 1; i < 5; ++i) {
		secondRoomVariation.push_back("Map Files/2_var_" + to_string(i) + ".txt");
	}
	for (int i = 1; i < 5; ++i) {
		thirdRoomVariation.push_back("Map Files/3_var_" + to_string(i) + ".txt");
	}
	for (int i = 1; i < 5; ++i) {
		fourthRoomVariation.push_back("Map Files/4_var_" + to_string(i) + ".txt");
	}
	for (int i = 1; i < 5; ++i) {
		fifthRoomVariation.push_back("Map Files/5_var_" + to_string(i) + ".txt");
	}
	for (int i = 1; i < 5; ++i) {
		sixthRoomVariation.push_back("Map Files/6_var_" + to_string(i) + ".txt");
	}
	for (int i = 1; i < 5; ++i) {
		seventhRoomVariation.push_back("Map Files/7_var_" + to_string(i) + ".txt");
	}
	for (int i = 1; i < 5; ++i) {
		eigthRoomVariation.push_back("Map Files/8_var_" + to_string(i) + ".txt");
	}
	for (int i = 1; i < 5; ++i) {
		ninthRoomVariation.push_back("Map Files/9_var_" + to_string(i) + ".txt");
	}
	for (int i = 1; i < 5; ++i) {
		tenthRoomVariation.push_back("Map Files/10_var_" + to_string(i) + ".txt");
	}

	for (int i = 0; i < LAYOUT_X; ++i) {
		for (int j = 0; j < LAYOUT_Y; ++j) {
			if (mapLayout[j][i] != 0) {
				switch (mapLayout[j][i]) {
				case 1:
					readMap(firstRoomVariation[rand() % 4], tmp, xOffset, yOffset);
					fillLargeArray(tmp, xOffset, yOffset);
					break;
				case 2:
					readMap(secondRoomVariation[rand() % 4], tmp, xOffset, yOffset);
					fillLargeArray(tmp, xOffset, yOffset);
					break;
				case 3:
					readMap(thirdRoomVariation[rand() % 4], tmp, xOffset, yOffset);
					fillLargeArray(tmp, xOffset, yOffset);
					break;
				case 4:
					readMap(fourthRoomVariation[rand() % 4], tmp, xOffset, yOffset);
					fillLargeArray(tmp, xOffset, yOffset);
					break;
				case 5:
					readMap(fifthRoomVariation[rand() % 4], tmp, xOffset, yOffset);
					fillLargeArray(tmp, xOffset, yOffset);
					break;
				case 6:
					readMap(sixthRoomVariation[rand() % 4], tmp, xOffset, yOffset);
					fillLargeArray(tmp, xOffset, yOffset);
					break;
				case 7:
					readMap(seventhRoomVariation[rand() % 4], tmp, xOffset, yOffset);
					fillLargeArray(tmp, xOffset, yOffset);
					break;
				case 8:
					readMap(eigthRoomVariation[rand() % 4], tmp, xOffset, yOffset);
					fillLargeArray(tmp, xOffset, yOffset);
					break;
				case 9:
					readMap(ninthRoomVariation[rand() % 4], tmp, xOffset, yOffset);
					fillLargeArray(tmp, xOffset, yOffset);
					break;
				case 10:
					readMap(tenthRoomVariation[rand() % 4], tmp, xOffset, yOffset);
					fillLargeArray(tmp, xOffset, yOffset);
					break;
				}
				xOffset += 50;
			}
			else {
				fillLargeArray(unwalkable, xOffset, yOffset);
				xOffset += 50;
			}
		}
		yOffset += 50;
		xOffset = 0;
	}
}

void GameApp::fillLargeArray(unsigned int** &small, int xOffset, int yOffset) {
	for (int i = 0; i < 50; ++i) {
		for (int j = 0; j < 50; ++j) {
			trueMap[xOffset + j][yOffset + i] = small[j][i];
		}
	}
}

void GameApp::renderGameLevel() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gameTile);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTranslatef((-TILE_SIZE / 2), (-TILE_SIZE / 2), 0.0f);

	vector<float> vertexData;
	vector<float> texCoordData;
	for (int y = 0; y < 50; y++) {
		for (int x = 0; x < 50; x++) {
			if (trueMap[y][x] != 404) {
				float u = (float)(((int)smallMap[y][x]) % spriteCountX) / (float)spriteCountX;
				float v = (float)(((int)smallMap[y][x]) / spriteCountX) / (float)spriteCountY;
				float spriteWidth = 1.0f / (float)spriteCountX;
				float spriteHeight = 1.0f / (float)spriteCountY;
				vertexData.insert(vertexData.end(), {
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
				});
				texCoordData.insert(texCoordData.end(), { u, v,
					u, v + (spriteHeight),
					u + spriteWidth, v + (spriteHeight),
					u + spriteWidth, v
				});
			}
		}
	}
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, mapWidth * mapHeight * 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void GameApp::worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
	*gridX = (int)((worldX + (WORLD_OFFSET_X)) / TILE_SIZE);
	*gridY = (int)((-worldY + (WORLD_OFFSET_Y)) / TILE_SIZE);
}

void GameApp::fillSmallArray() {
	int playerCoordX, playerCoordY;
	worldToTileCoordinates(players[0].hero->position.x, players[0].hero->position.y, &playerCoordX, &playerCoordY);
	for (int i = 0; i < 50; ++i) {
		for (int j = 0; j < 50; ++j) {
			if (playerCoordX + (i - 25) < 250 && playerCoordY + (j - 25) < 250 && playerCoordX + (i - 25) > -1 && playerCoordY + (j - 25) > -1)
				smallMap[j][i] = trueMap[playerCoordY + (j - 25)][playerCoordX + (i - 25)];
			else
				smallMap[j][i] = 404;
		}
	}
}
