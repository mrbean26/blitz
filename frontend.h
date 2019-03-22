//use of the engine to create the game

#include "worldGeneration.h"
worldGeneration mainWorldGeneration;

#include "startScreen.h"
startScreen mainStart;

void frontendBegin() { //called before the first frame
	int emptyButton = createButton(); //first button does not show if this is not used
	mainStart.begin();
	mainWorldGeneration.begin();
}

void frontendMainloop() { //called every frame
	mainStart.mainloop();
	mainWorldGeneration.mainloop();
}