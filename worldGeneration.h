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

int randomInt(int min, int max); // quick maths

extern int lineCount;
int newLinePos(vector<string> &usedVector); // resize a vector of strings and return the position to be edited

void createSave(const char * filePath, int saveType); // a lot of code

class worldGeneration {
public:
	bool active = false;
	bool startedBegin = false;
	float triangleVertices[9] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f
	};
	GLuint vertexArray, vertexBuffer;
	void startTriangle(); //reserve data to vao & vbo
	void renderTriangle(vector<vec3> allPoints); 

	unsigned int terrainShader;
	void startShader();

	vector<string> allWorldLines;
	const char * worldLinesPath;
	void renderTerrain(vector<string> allLines, int currentArea); // this may take some time
	//called from other files
	void begin(); // called before the first frame
	void mainloop(); // called every frame
};

#endif