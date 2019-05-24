#pragma once

#include "frontend.h"

struct buildingColour {
	GLuint VAO, VBO;
	vec3 scale = vec3(1.0f);
	vec3 position = vec3(0.0f);
	vec3 rotation = vec3(0.0f);
	int size = 0;
};

struct placedMiniBuilding {
	vec2 scale = vec2(0.5f, 0.5f);
	vec2 position = vec3(0.0f);
	vec3 rotation = vec3(0.0f);
};

void StructuresBegin();
void StructuresMainloop();

extern vector<placedMiniBuilding> allMiniBuildings;
extern vector<string> newBuildingLines;
extern vector<vec2> mountainLimits;

void startBuildingSelectUI();
void getMountainLimits(vector<float> vertices);

bool insideAreaScale();
bool insideMountain();
bool okToBuild();

void startBuildings();
void renderBuildings();
void buildingInteractions();

extern bool benchInUse;
extern buildingColour mainBench, mainBlueprint;
extern vec2 currentBuildingScale, currentBuildingPosition;

void startBuildBench();
void buildBenchInteraction();
