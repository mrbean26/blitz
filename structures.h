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

void startBuildings();
void renderBuildings();

extern GLuint benchVAO, benchVBO;
extern vec3 benchPos, benchScale, benchRot;

extern GLuint benchBlueprintVAO, benchBlueprintVBO;
extern vec3 benchBlueprintPos, benchBlueprintScale, benchBluePrintRot;
void startBuildBench();
