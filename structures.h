#pragma once

#include "frontend.h"

struct buildingColour {
	GLuint VAO, VBO;
	vec3 scale = vec3(1.0f);
	vec3 position = vec3(0.0f);
	vec3 rotation = vec3(0.0f);
	int size = 0;
};

void StructuresBegin();
void StructuresMainloop();

void startBuildingSelectUI();

void startBuildings();
void renderBuildings();
void buildingInteractions();

extern bool benchInUse;
extern buildingColour mainBench, mainBlueprint;
extern vec2 currentBuildingScale, currentBuildingPosition;

void startBuildBench();
void buildBenchInteraction();
