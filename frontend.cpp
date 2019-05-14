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
	int emptyButton = createButton(); //first button does not show if this is not used, also allows texts to show with skybox
	mainStart.begin();
	startPlayerShader();
	startSkybox();
}

void frontendMainloop(){
	if (!earthWorldGeneration.startedBegin && earthWorldGeneration.active) {
		earthWorldGeneration.begin();
		mainPlayer.begin();
		earthWorldGeneration.startedBegin = true;
	}
	renderSkybox(!earthWorldGeneration.startedBegin);
	earthWorldGeneration.mainloop();
	mainPlayer.mainloop();
	mainStart.mainloop();
}
