#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#define PI 3.14159265359f

#define GAME_TITLE "Final Project"
#define RESOLUTION_W 1280
#define RESOLUTION_H 720

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

struct Color{
	float r;
	float g;
	float b;
	float a;
};

inline float mapValue(float value, float srcMin, float srcMax, float dstMin, float dstMax) {
	float retVal = dstMin + ((value - srcMin) / (srcMax - srcMin) * (dstMax - dstMin));
	if (retVal < dstMin) {
		retVal = dstMin;
	}
	if (retVal > dstMax) {
		retVal = dstMax;
	}
	return retVal;
}

inline float lerp(GLfloat v0, GLfloat v1, GLfloat t) {
	return (1.0f - t)*v0 + t*v1;
}

inline float easeInOut(float from, float to, float time) {
	float tVal;
	if (time > 0.5) {
		float oneMinusT = 1.0f - ((0.5f - time)*-2.0f);
		tVal = 1.0f - ((oneMinusT * oneMinusT * oneMinusT * oneMinusT *
			oneMinusT) * 0.5f);
	}
	else {
		time *= 2.0;
		tVal = (time*time*time*time*time) / 2.0;
	}
	return (1.0f - tVal)*from + tVal*to;
}

inline float easeOutElastic(float from, float to, float time) {
	float p = 0.3f;
	float s = p / 4.0f;
	float diff = (to - from);
	return from + diff + (diff*pow(2.0f, -10.0f*time) * sin((time - s)*(2 * PI) / p));
}