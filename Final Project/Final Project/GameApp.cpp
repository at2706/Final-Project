#include "GameApp.h"


GameApp::GameApp(){
	init();

}

void GameApp::init() {
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
		//Entity::fixedUpdateAll(this);
	}
	timeLeftOver = fixedElapsed;

}

GLvoid GameApp::Render() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	SDL_GL_SwapWindow(displayWindow);
}