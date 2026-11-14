
#include "L_gameTime.h"

float lastFrameTime = 0;

void updateLastFrameTime() {
	lastFrameTime = SDL_GetTicks();
}
float getDeltaTime() {
	return (SDL_GetTicks() - lastFrameTime)*0.001;
}