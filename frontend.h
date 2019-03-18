//use of the engine to create the game

#include "worldGeneration.h"

#include "startScreen.h"
startScreen mainStart;

void frontendBegin() {
	int emptyButton = createButton(); //first button does not show if this is not used
	mainStart.begin();
}

void frontendMainloop() {
	mainStart.mainloop();
}