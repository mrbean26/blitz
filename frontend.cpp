#include "frontend.h"

#include "interface.h"

#include "worldGeneration.h"
worldGeneration mainWorldGeneration;
#include "startScreen.h"
startScreen mainStart;

void frontendBegin(){
	int emptyButton = createButton(); //first button does not show if this is not used
	mainStart.begin();
}

void frontendMainloop(){
	mainStart.mainloop();
	if (!mainWorldGeneration.startedBegin && mainWorldGeneration.active) {
		mainWorldGeneration.begin();
		mainWorldGeneration.startedBegin = true;
	}
	mainWorldGeneration.mainloop();
}
