#include "GameApp.h"

GameApp::GameApp(){
	init();
	charSheet = loadTexture("sheet.png");
	tileSheet = loadTexture("arne_sprites.png", GL_NEAREST);
	Sprite *s;
	Entity *e;
	s = new Sprite(charSheet, 1024, 1024, 112, 866, 112, 75);
	e = new Entity(s);

	s = new Sprite(tileSheet, 114, 16, 8);
	e = new Entity(s, 0.5f, 0.0f);
}

GLvoid GameApp::init() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	glViewport(0, 0, 1280, 720);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-2.66, 2.66, -2.0, 2.0, -2.0, 2.0);
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
	Entity::renderAll();
	SDL_GL_SwapWindow(displayWindow);
}