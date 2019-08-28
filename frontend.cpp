#include "frontend.h"

#include "interface.h"

#include "worldGeneration.h"
worldGeneration earthWorldGeneration;
#include "startScreen.h"
startScreen mainStart;

void frontendBegin(){
	int emptyButton = createButton(); //first button does not show if this is not used, texts hold this texture slot
	mainStart.begin();
}

void frontendMainloop(){
	mainStart.mainloop();
	if (!earthWorldGeneration.startedBegin && earthWorldGeneration.active) {
		earthWorldGeneration.begin();
		earthWorldGeneration.startedBegin = true;
	}
	earthWorldGeneration.mainloop();
}
