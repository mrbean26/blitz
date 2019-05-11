#include "frontend.h"

#include "interface.h"

#include "worldGeneration.h"
worldGeneration earthWorldGeneration;
#include "startScreen.h"
startScreen mainStart;
#include "player.h"
player mainPlayer;
#include "display.h"

void frontendBegin(){
	int emptyButton = createButton(); //first button does not show if this is not used
	mainStart.begin();
	startPlayerShader();
	startSkybox();
}

void frontendMainloop(){
	mainStart.mainloop();
	if (!earthWorldGeneration.startedBegin && earthWorldGeneration.active) {
		earthWorldGeneration.begin();
		mainPlayer.begin();
		earthWorldGeneration.startedBegin = true;
	}
	earthWorldGeneration.mainloop();
	mainPlayer.mainloop();
	renderSkybox(); // last
}
