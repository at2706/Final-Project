#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#define PI 3.14159265359f

#define GAME_TITLE "Final Project"
#define RESOLUTION_W 960
#define RESOLUTION_H 600

#define ASPECT_RATIO_X 1.6f
#define ASPECT_RATIO_Y 1.0f
#define FIXED_TIMESTEP 0.01333333f
#define MAX_TIMESTEPS 6

#define LEVEL_HEIGHT 22
#define LEVEL_WIDTH 32
#define TILE_SIZE 0.125f
#define SPRITE_COUNT_X 16
#define SPRITE_COUNT_Y 8

#define MARGIN_TOP 0.1f
#define MARGIN_BOTTOM 0.1f
#define MARGIN_LEFT 0.1f
#define MARGIN_RIGHT 0.1f

using namespace std;