#pragma once
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "frontend.h"

struct buildingColour {
	int buildingType = -1;
	GLuint VAO = 0, VBO = 0;
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

extern int interactKey, shootButton, aimButton;
void StructuresBegin();
void StructuresMainloop();

extern vector<buildingColour> allColourBuildings;
void startColorBuilding(vector<float> vertices, buildingColour* usedBuilding);
void startIrregularColorBuilding(vector<float> vertices, GLuint& VAO, GLuint& VBO, GLuint& size);
void startPhysicalBuildings();

extern vector<placedMiniBuilding> allMiniBuildings;
extern vector<string> newBuildingLines;
extern vector<vec2> mountainLimits;

void getAllBuildingPositions();
void startBuildingSelectUI();
void getMountainLimits(vector<float> vertices);

bool insideOtherBuilding();
bool insideAreaScale();
bool insideMountain();
bool okToBuild();
void buildCollisions(vec3 &position, int &insideBuildingIndex, float &jumpVelocity, bool &lastOnBench, bool &hitBuilding);

void changeBuildings();
void startBuildings();
void renderBuildings();
void buildingInteractions();

extern bool benchInUse;
extern buildingColour mainBench, mainBlueprint;
extern vec2 currentBuildingScale, currentBuildingPosition;
extern int currentBuildingType;

void startBuildBench();
void buildBenchInteraction();

#endif