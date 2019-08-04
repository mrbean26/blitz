#include "frontend.h"

#include "interface.h"

#include "worldGeneration.h"
worldGeneration WorldGeneration;

#include "startScreen.h"
startScreen StartScreen;

#include "player.h"
player mainPlayer;

#include "structures.h"
#include "weapons.h"
#include "monsters.h"
#include "display.h"
#include "inventory.h"

void frontendBegin(){
	int emptyButton = createButton(); //first button does not show if this is not used, also allows texts to show with skybox
	StartScreen.begin();
	startPlayerShader();
	startSkybox();
}

void frontendMainloop(){
	if (!WorldGeneration.startedBegin && WorldGeneration.active) {
		WorldGeneration.begin();
		mainPlayer.begin();
		StructuresBegin();
		weaponsBegin();
		monstersBegin();
		inventoryBegin();
		WorldGeneration.startedBegin = true;
	}
	renderSkybox(!WorldGeneration.startedBegin);
	StartScreen.mainloop();
	WorldGeneration.mainloop();
	inventoryMainloop();
	StructuresMainloop();
	weaponsMainloop();
	mainPlayer.mainloop();
	monstersMainloop();
	interfaceLastcall();
	updateKeys(); // this has to be called last every frame / after last call of getkeydown
}

