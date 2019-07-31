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

vector<float> getTerrainCoords(string line);

bool insideCircle(vec2 circlePos, float radius, vec2 pointPos, bool terrain = false);

vec3 colourDifference(float multiplier);

vector<vec2> circleCoords(vec2 position, float radius, int pointCount, float scale);

void createSave(const char * filePath, int saveType); // a lot of code

class triangle {
public:
	vector<vec3> allPoints;
	vec3 colour = vec3(1.0f, 1.0f, 1.0f);
	float alpha = 1.0f;
	bool active = true;

	vector<int> edgeVectorIndexes;
};

extern vector<triangle> flatTerrainTriangles, mountainTriangles;
extern vector<float> flatXPoints, flatZPoints;
extern vector<vec2> currentAllMountainPositions;
extern vector<vec3> currentAllMountainScales;
extern vec2 currentPlanetScale;

extern unsigned int skyboxVAO, skyboxVBO, skyboxTexture;
unsigned int loadCubemapTexture(vector<string> faces);
void startSkybox();
void renderSkybox(bool startScreen = false);

extern vec3 lightPos;
extern float lightIntensity, lightRadius, 
	lowestLight, currentTime, timeMultiplier;

class worldGeneration {
public:
	int currentArea = PLANET_WORLD;
	vec2 currentAreaScale;
	vec3 currentAreaColour;
	string currentAreaPrefix;
	vec2 getAreaScale();
	int terrainVerts = 0;

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

	GLuint areaLimitVAO, areaLimitVBO, areaLimitCount;
	void beginAreaLimits();
	void beginTerrrain();

	void renderAreaLimits();
	void renderTerrain(); // this may take some time

	void daynightCycle();

	//called from other files
	void begin(); // called before the first frame
	void mainloop(); // called every frame
};

#endif