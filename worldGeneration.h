#pragma once

#ifndef WORLD_GEN_H
#define WORLD_GEN_H

using namespace std;
#include <cstdlib>
#include <vector>
#include <string>

#include <glew.h>
#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#define DEFAULT_SAVE 0
#define PLANET_WORLD 0

extern int randomSeedCount;
int randomInt(int min, int max); // quick maths

bool insideCircle(vec2 circlePos, float radius, vec2 pointPos, bool terrain = false);

vec3 colourDifference(float multiplier);

vector<vec2> circleCoords(vec2 position, float radius, int pointCount, float scale);

extern int lineCount;
int newLinePos(vector<string> &usedVector); // resize a vector of strings and return the position to be edited

void createSave(const char * filePath, int saveType); // a lot of code

int newVectorPos(vector<float>* usedVector);

class triangle {
public:
	vector<vec3> allPoints;
	vec3 colour = vec3(1.0f, 1.0f, 1.0f);
	float alpha = 1.0f;
	bool active = true;
};

extern vector<vec2> currentAllMountainPositions;
extern vector<vec3> currentAllMountainScales;
extern vec2 currentPlanetScale;

extern unsigned int skyboxVAO, skyboxVBO, skyboxTexture;
unsigned int loadCubemapTexture(vector<string> faces);
void startSkybox();
void renderSkybox(bool startScreen = false);

class worldGeneration {
public:
	int currentArea = PLANET_WORLD;
	vec2 currentAreaScale;
	vec2 getAreaScale();

	bool active = false;
	bool startedBegin = false;

	GLuint terrainVAO, terrainVBO;
	void reserveMemory(); //reserve data to vao & vbo

	unsigned int terrainShader;
	void startShader();

	vector<string> allWorldLines;
	const char * worldLinesPath;

	void beginFlatTerrain();
	void beginMountains();
	void removeUselessTriangle(int radius, vec2 position, float circleMultiplier, bool mountain);

	GLuint areaLimitVAO, areaLimitVBO, areaLimitCount;
	void beginAreaLimits();
	void beginTerrrain();

	void renderAreaLimits();
	void renderTerrain(); // this may take some time

	//called from other files
	void begin(); // called before the first frame
	void mainloop(); // called every frame
};

#endif