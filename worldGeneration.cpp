// this code is not well written

using namespace std;
#include <vector>
#include <iostream>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <glew.h>

#include "display.h"
#include "shader.h"
#include "saveFiles.h"
#include "worldGeneration.h"

int randomInt(int min, int max) {
	int random = rand();
	return min + (random % max);
}

vector<vec2> circleCoords(vec2 position, float radius, int pointCount, float scale) { //used for mountains, craters & camera rot
	// math values
	float pi = (float)atan(1) * 4;
	float k = 2.0f * pi / pointCount;
	vector<vec2> coords;
	for (int i = 0; i < pointCount; i++) {
		float phi = i * k;
		// generate a circle with scale
		float x = radius * cos(phi);
		float y = radius * sin(phi);
		// rescale
		x = x * scale;
		y = y * scale;
		// move to position
		x += position.x;
		y += position.y;
		// assign to vector
		int coordCount = coords.size();
		coords.resize(coordCount + 1);
		coords[coordCount] = vec2(x, y);
	}
	return coords;
}

int lineCount = 0;

int newLinePos(vector<string>& usedVector) { //resize vector & return next pos
	int currentSize = usedVector.size();
	lineCount++;
	usedVector.resize(lineCount);
	return lineCount - 1;
}

void createSave(const char * filePath, int saveType) { // write new world to save
	vector<string> saveLines = { "IN USE" };
	lineCount = 1; //including IN USE line
	if (saveType == DEFAULT_SAVE) {
		//inventory and hotbar with no items (start of world)
		saveLines[newLinePos(saveLines)] = "inventory 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"; //15 slots
		saveLines[newLinePos(saveLines)] = "hotbar 0 0 0 0 0"; //5 slots
		//"current" data
		saveLines[newLinePos(saveLines)] = "currentPlanet earth"; //basic science base on earth
		saveLines[newLinePos(saveLines)] = "currentPosition 0.0f, 0.0f, 0.0f";
		//EARTH SAVED DATA----------------------------------------------------------------------
		//land scale
		int earthScaleX = randomInt(10, 90);
		int earthScaleY = 100 - earthScaleX;
		saveLines[newLinePos(saveLines)] = "planetEarthSize " + to_string(earthScaleX) + " " + to_string(earthScaleY);
		//mountains
		int earthMountainCount = randomInt(1, 10);
		for (int i = 0; i < earthMountainCount; i++) {
			//mountain scale
			float mountainScaleX = (randomInt(0, earthScaleX) * 0.1f);
			float mountainScaleZ = (randomInt(1, 100)*0.1f);
			//mountain position
			float mountainPositionX = (randomInt(0, earthScaleX * 10) * 0.1f);
			float mountainPositionY = (randomInt(0, earthScaleY * 10) * 0.1f);
			//gradient/steepness
			float mountainGradient = (randomInt(0, 20) * 0.1f);
			//write to file
			saveLines[newLinePos(saveLines)] = "earthMountainPosition " +
				to_string(mountainPositionX) + " " + to_string(mountainPositionY);
			saveLines[newLinePos(saveLines)] = "earthMountainScale " +
				to_string(mountainScaleX) + " " + to_string(mountainScaleZ);
			saveLines[newLinePos(saveLines)] = "earthMountainGradient " +
				to_string(mountainGradient);
		}
		//craters
		int earthCraterCount = 15 - earthMountainCount;
		for (int i = 0; i < earthCraterCount; i++) {
			//crater scale
			float craterScaleX = (randomInt(0, earthScaleX) * 0.1f);
			float craterScaleZ = (randomInt(0, 100) * 0.1f);
			//crater position
			float craterPositionX = (randomInt(0, earthScaleX * 10) * 0.1f);
			float craterPositionY = (randomInt(0, earthScaleY * 10) * 0.1f);
			//gradient/steepness
			float craterGradient = -(randomInt(0, 20) * 0.1f);
			//save
			saveLines[newLinePos(saveLines)] = "earthMountainPosition " +
				to_string(craterPositionX) + " " + to_string(craterPositionY);
			saveLines[newLinePos(saveLines)] = "earthMountainScale " +
				to_string(craterScaleX) + " " + to_string(craterScaleZ);
			saveLines[newLinePos(saveLines)] = "earthMountainGradient " +
				to_string(craterGradient);
		}
	}
	writeLines(filePath, saveLines);
}

void worldGeneration::startTriangle() { // reserve data in memory for triangle
	glGenVertexArrays(1, &terrainVAO);
	glGenBuffers(1, &terrainVBO);
	glBindVertexArray(terrainVAO);
	glBindBuffer(GL_ARRAY_BUFFER_ARB, terrainVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void worldGeneration::startShader() {
	int vertShader = createShader(terrainVertSource, GL_VERTEX_SHADER);
	int fragShader = createShader(terrainFragSource, GL_FRAGMENT_SHADER);
	terrainShader = createProgram({ vertShader, fragShader });
}

vec3 colourDifference(float multiplier){
	float colourDifferenceX = ((float)randomInt(-127, 127) / 255.0f) * multiplier;
	float colourDifferenceY = ((float)randomInt(-127, 127) / 255.0f) * multiplier;
	float colourDifferenceZ = ((float)randomInt(-127, 127) / 255.0f) * multiplier;
	return vec3(colourDifferenceX, colourDifferenceY, colourDifferenceZ);
}

vector<triangle> flatTerrainTriangles;
void worldGeneration::beginFlatTerrain() { // simple x+y for loop
	vec3 colour;
	vec2 areaScale;
	if (currentArea == PLANET_WORLD) {
		colour = vec3(0.35f, 0.78f, 0.31f);
		areaScale = getVec2File(worldLinesPath, "planetEarthSize");
	}
	float triangleSize = 0.75f;
	for (int x = 0; x < areaScale.x / triangleSize; x++) {
		for (int y = 0; y < areaScale.y / triangleSize; y++) {
			// draw triangle
			// multipliers
			float xMultiplied = x * triangleSize;
			float yMultiplied = y * triangleSize;
			// points
			vec3 pointOne = vec3(xMultiplied, 0.0f, -yMultiplied);
			vec3 pointTwo = vec3(xMultiplied + triangleSize, 0.0f, -yMultiplied);
			vec3 pointThree = vec3(xMultiplied, 0.0f, -(yMultiplied + triangleSize));
			vec3 pointFour = vec3(xMultiplied + triangleSize, 0.0f, -(yMultiplied + triangleSize));
			vector<vec3> whichPoint = { pointOne, pointFour };
			// assign triangles to vector
			for (int t = 0; t < 2; t++) {
				// triangle colour
				vec3 colourDifference = colourDifference(0.2f);
				vec3 triangleColour = colour + colourDifference;

				triangle newTriangle;
				newTriangle.allPoints = { whichPoint[t], pointTwo, pointThree };
				newTriangle.colour = triangleColour;

				int vectorSize = flatTerrainTriangles.size();
				flatTerrainTriangles.resize(vectorSize + 1);
				flatTerrainTriangles[vectorSize] = newTriangle;
			}
		}
	}
	beginMountains();
}

vector<triangle> mountainTriangles;
void worldGeneration::beginMountains() { // use circles 
	vec3 colour;
	string mountainName;
	int lineCount = allWorldLines.size();
	if (currentArea == PLANET_WORLD) {
		colour = vec3(0.35f, 0.78f, 0.31f);
		mountainName = "earthMountain";
	}
	// get all mountain stats
	vector<vec2> mountainPositions;
	vector<vec2> mountainScales;
	vector<float> mountainGradients;
	// run through each line
	for (int l = 0; l < lineCount; l++) {
		vec2 currentLineMountainPos = getVec2File(worldLinesPath, mountainName + "Position", l);
		if (currentLineMountainPos != vec2(-1.0f, -1.0f)) {
			// mountain found
			vec2 mountainScale = getVec2File(worldLinesPath, mountainName + "Scale", l + 1);
			float mountainGradient = getFloatFile(worldLinesPath, mountainName + "Gradient", l + 2);
			// add to vector
			// resize
			int currentVectorSize = mountainPositions.size();
			mountainPositions.resize(currentVectorSize + 1);
			mountainGradients.resize(currentVectorSize + 1);
			mountainScales.resize(currentVectorSize + 1);
			// add values
			mountainPositions[currentVectorSize] = currentLineMountainPos;
			mountainScales[currentVectorSize] = mountainScale;
			mountainGradients[currentVectorSize] = mountainGradient;
		}
	}
	// mountain stats
	float defaultScale = 0.025f;
	// start triangles
	int index = 0;
	for (vec2 pos : mountainPositions) {
		vec2 sca = mountainScales[index];
		float gra = mountainGradients[index];
		// create circle radiuses
		vector<int> radiuses;
		for (float r = sca.x; r > 0.0f; r -= 0.1f) {
			int rCount = radiuses.size();
			radiuses.resize(rCount + 1);
			radiuses[rCount] = (int)std::round(r * 100.0f);
		}
		// create circles & triangles here
		for (int radius : radiuses) {
			int currentPointCount = (int)std::round(radius / 4) * 2;
			currentPointCount = (int)std::round(currentPointCount / 5);
			vector<vec2> currentCircle = circleCoords(pos, radius, currentPointCount, defaultScale);

			int nextRadius = radius - 10;
			if (nextRadius <= 10) {
				break;
			}
			int nextPointCount = (int)std::round(nextRadius / 4) * 2;
			nextPointCount = (int)std::round(nextPointCount / 5);
			vector<vec2> nextCircle = circleCoords(pos, nextRadius, nextPointCount, defaultScale);

			int pointIndex = 0;
			for (vec2 point : currentCircle) {
				int nextIndex = pointIndex + 1;
				if (nextIndex > currentCircle.size() - 1) {
					nextIndex = 0;
				}
				// triangle y positions
				float centralYPos = (radiuses[0] - radius + (10 * gra)) / radiuses[0];
				float innerYPos = (radiuses[0] - nextRadius + (10 * gra)) / radiuses[0];
				if (gra < 0.0f) {
					centralYPos = -centralYPos;
					innerYPos = -innerYPos;
				}
				centralYPos = std:clamp(centralYPos, -1.0f, 1.0f);
				innerYPos = std::clamp(innerYPos, -1.0f, 1.0f);
				if (radius == radiuses[0]) {
					centralYPos = 0.0f;
				}
				// line to make circle points into a ring
				vec3 pointOne = vec3(currentCircle[pointIndex].x, centralYPos * sca.y, -currentCircle[pointIndex].y);
				vec3 pointTwo = vec3(currentCircle[nextIndex].x, centralYPos * sca.y, -currentCircle[nextIndex].y);
				// line going up & to the right
				int nextCircleIndex = floor(pointIndex / (currentPointCount / nextPointCount));
				if (nextCircleIndex > nextCircle.size() - 1) {
					nextCircleIndex = 0;
				}
				vec3 pointThree = vec3(nextCircle[nextCircleIndex].x, innerYPos * sca.y, -nextCircle[nextCircleIndex].y);
				// line going up & to the left
				int lastIndex = nextCircleIndex - 1;
				if (lastIndex < 0) {
					lastIndex = nextCircle.size() - 1;
				}
				vec3 pointFour = vec3(nextCircle[lastIndex].x, innerYPos * sca.y, -nextCircle[lastIndex].y);
				// create triangles
				vec3 whichPoint[2] = { pointThree, pointFour };
				for (int t = 0; t < 2; t++) {
					vec3 colourDifference = colourDifference(0.2f);
					vec3 triangleColour = colour + colourDifference;
					triangle newTriangle;
					newTriangle.allPoints = { pointOne, pointTwo, whichPoint[t] };
					newTriangle.colour = triangleColour;
					int triangleCount = mountainTriangles.size();
					mountainTriangles.resize(triangleCount + 1);
					mountainTriangles[triangleCount] = newTriangle;
				}
				vec3 colourDifference = colourDifference(0.2f);
				vec3 triangleColour = colour + colourDifference;

				triangle newTriangle;
				newTriangle.allPoints = { pointOne, pointThree, pointFour };
				newTriangle.colour = triangleColour;
				
				int triangleCount = mountainTriangles.size();
				mountainTriangles.resize(triangleCount + 1);
				mountainTriangles[triangleCount] = newTriangle;
				pointIndex++;
			}
		}
		index++;
	}
	beginTerrrain();
}

int total = 0;
void worldGeneration::beginTerrrain(){ // update buffer data to hold new triangles
	vector<float> allVertices;
	// get triangles into float vector
	vector<vector<triangle>> bothVectors = { flatTerrainTriangles, mountainTriangles };
	for (int vector = 0; vector < 2; vector++) {
		vector<triangle> currentVector = bothVectors[vector];
		int triangleCount = currentVector.size();
		for (int f = 0; f < triangleCount; f++) {
			vector<vec3> points = currentVector[f].allPoints;
			for (vec3 point : points) {
				for (int v = 0; v < 3; v++) {
					int floatCount = allVertices.size();
					allVertices.resize(floatCount + 1);
					allVertices[floatCount] = point[v];

				}
				//color
				vec3 colour = currentVector[f].colour;
				for (int c = 0; c < 3; c++) {
					int newFloatCount = allVertices.size();
					allVertices.resize(newFloatCount + 1);
					allVertices[newFloatCount] = colour[c];
				}
			}
		}
	}
	// vector to array
	int floatCount = (mountainTriangles.size() * 12) + (flatTerrainTriangles.size() * 12);
	glBindVertexArray(terrainVAO);
	glBindBuffer(GL_ARRAY_BUFFER_ARB, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER_ARB, allVertices.size() * sizeof(float), allVertices.data(), GL_STATIC_DRAW_ARB);
	total = (mountainTriangles.size() * 3) + (flatTerrainTriangles.size() * 3);
}

void worldGeneration::renderTerrain() {
	if (currentArea == PLANET_WORLD) {
		int modelLocation = glGetUniformLocation(terrainShader, "model");
		int projectionLocation = glGetUniformLocation(terrainShader, "projection");
		int viewLocation = glGetUniformLocation(terrainShader, "view");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(modelMatrix()));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(projectionMatrix()));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(viewMatrix()));
		glBindVertexArray(terrainVAO);
		glUseProgram(terrainShader);
		glDrawArrays(GL_TRIANGLES, 0, 3 * total);
	}
}

void worldGeneration::begin() {
	startShader();
	startTriangle();
	allWorldLines = readLines(worldLinesPath);
	beginFlatTerrain();
}

void worldGeneration::mainloop() {
	if (!active) {
		return;
	}
	renderTerrain();
}
