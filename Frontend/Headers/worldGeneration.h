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
#define LARGE_WORLD 1
#define PLANET_WORLD 0

#define RADIUS_DIFFERENCE 0.5f
#define POINT_COUNT 32
#define MOUNTAIN_SCALE_DEFAULT 0.025f
#define TRIANGLE_SIZE_FLAT 1.0f

#define EARTH_WAVE_WIDTH 2.0f
#define EARTH_WAVE_HEIGHT 0.5f
#define EARTH_WAVE_DELAY 1.0f
#define EARTH_WAVE_SPEED 10.0f

#define CLOUD_X_SLOTS 16
#define CLOUD_Z_SLOTS 16
#define CLOUD_DEFAULT_SCALE 250
#define CLOUD_SPEED 1.0f
#define CLOUD_DEFAULT_HEIGHT 30.0f
#define CLOUD_FLOATING_MULTIPLIER 30.0f

int randomInt(int min, int max);

bool insideCircle(vec2 circlePos, float radius, vec2 pointPos, bool terrain = false);

vec3 colourDifference(float multiplier);

vector<vec2> circleCoords(vec2 position, float radius, int pointCount, float scale);

void createSave(const char * filePath, int saveType); 

void beginRandom();
void endRandom();

class triangle {
public:
	vector<vec3> allPoints;
	vec3 colour = vec3(1.0f, 1.0f, 1.0f);
	float alpha = 1.0f;
	bool active = true;

	vector<int> edgeVectorIndexes;
};

extern vector<triangle> flatTerrainTriangles, mountainTriangles, waterTriangles;
extern vector<float> flatXPoints, flatZPoints;
extern vector<vec2> currentAllMountainPositions;
extern vector<vec3> currentAllMountainScales;
extern vector<bool> currentAllMountainWaters;
extern vec2 currentPlanetScale;

vec4 terrainColliders(vec3 original, float yAddition);

extern int researchStatusText;
void startResearchText();

extern unsigned int skyboxVAO, skyboxVBO, skyboxTexture;
unsigned int loadCubemapTexture(vector<string> faces);
void startSkybox();
void renderSkybox(bool startScreen = false);

void spawnClouds();
void cloudMainloop(bool startScreen);
void startClouds();
void renderClouds(bool startScreen);
void moveClouds();

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
    
	GLuint externalVAO, externalVBO, externalSize;
	void beginExternalFlat();

    GLuint waterVAO, waterVBO;
    GLuint waterSize;
	vec3 waterMultiplyColour = vec3(1.0f);
    void beginWater();
    void worldGenLast();

	void waveBegin();
	void waveMainloop();
	bool setWaveDelay = false;

	float wavePercentage = 0.0f;
	float waveWidth = 0.0f;
	float waveHeight = 0.0f;
	float waveDelayMax = 0.0f;
	float waveDelayCurrent = 0.0f;
	float waveSpeed = 0.0f;

	int dataPoints = 0;
    
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
