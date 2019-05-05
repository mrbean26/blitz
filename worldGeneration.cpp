// this code is not well written

using namespace std;
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vector_relational.hpp>
using namespace glm;

#include <glew.h>

#include "display.h"
#include "shader.h"
#include "saveFiles.h"
#include "worldGeneration.h"

bool insideCircle(vec2 circlePos, float radius, vec2 pointPos) {
	float xSquared = pow(pointPos.x - circlePos.x, 2);
	float ySquared = pow(pointPos.y - circlePos.y, 2);
	float radiusSquared = pow(radius, 2);
	if (xSquared + ySquared < radiusSquared-10) {
		return true;
	}
	return false;
}

int randomSeedCount = 5;
int randomSeed() {
	double timeSquared = pow(randomSeedCount, 4);
	randomSeedCount++;
	return round(timeSquared) * (10 * (cos(glfwGetTime())) * (tan(glfwGetTime()*2)));
}

int randomInt(int min, int max) {
	srand(randomSeed());
	int random = rand() % (max-min) + min;
	return random;
}

int randomColourInt(int min, int max) {
	int random = rand();
	return random % max;
}

vec3 colourDifference(float multiplier) {
	float colourDifferenceX = ((float)randomColourInt(-127, 127) / 255.0f) * multiplier;
	float colourDifferenceY = ((float)randomColourInt(-127, 127) / 255.0f) * multiplier;
	float colourDifferenceZ = ((float)randomColourInt(-127, 127) / 255.0f) * multiplier;
	return vec3(colourDifferenceX, colourDifferenceY, colourDifferenceZ);
}

vector<vec2> circleCoords(vec2 position, float radius, int pointCount, float scale) {
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

int newLinePos(vector<string>& usedVector) {
	int currentSize = usedVector.size();
	lineCount++;
	usedVector.resize(lineCount);
	return lineCount - 1;
}

void createSave(const char * filePath, int saveType) {
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
		int earthScaleX = randomInt(35, 65);
		int earthScaleY = 100 - earthScaleX;
		saveLines[newLinePos(saveLines)] = "planetEarthSize " + to_string(earthScaleX) + " " + to_string(earthScaleY);
		// lists to check if inside each other
		vector<vec2> mountainPositions;
		vector<float> mountainXScales, mountainGradients;
		int failedMountainCreations = 0;
		//mountains
		int earthMountainCount = randomInt(1, 8);;
		for (int i = 0; i < earthMountainCount; i++) {
			//mountain scale
			float mountainScaleX = (randomInt(0, earthScaleX) * 0.1f);
			float mountainScaleZ = (randomInt(1, 60)*0.1f);
			//mountain position
			float mountainPositionX = (randomInt(0, earthScaleX * 10) * 0.1f);
			float mountainPositionY = (randomInt(0, earthScaleY * 10) * 0.1f);
			//gradient/steepness
			float mountainGradient = (randomInt(3, 8) * 0.1f);
			// check if it inside any other mountains
			
			//write to file
			saveLines[newLinePos(saveLines)] = "earthMountainPosition " +
				to_string(mountainPositionX) + " " + to_string(mountainPositionY);
			saveLines[newLinePos(saveLines)] = "earthMountainScale " +
				to_string(mountainScaleX) + " " + to_string(mountainScaleZ);
			saveLines[newLinePos(saveLines)] = "earthMountainGradient " +
				to_string(mountainGradient);
			// assign to vectors
			
		}
		//craters
		int earthCraterCount = 10 - earthMountainCount;
		for (int i = 0; i < earthCraterCount; i++) {
			//crater scale
			float craterScaleX = (randomInt(0, earthScaleX) * 0.1f);
			float craterScaleZ = (randomInt(0, 60) * 0.1f);
			//crater position
			float craterPositionX = (randomInt(0, earthScaleX * 10) * 0.1f);
			float craterPositionY = (randomInt(0, earthScaleY * 10) * 0.1f);
			//gradient/steepness
			float craterGradient = -(randomInt(5, 10) * 0.5f);
			// check if it inside any other mountains

			//save
			saveLines[newLinePos(saveLines)] = "earthMountainPosition " +
				to_string(craterPositionX) + " " + to_string(craterPositionY);
			saveLines[newLinePos(saveLines)] = "earthMountainScale " +
				to_string(craterScaleX) + " " + to_string(craterScaleZ);
			saveLines[newLinePos(saveLines)] = "earthMountainGradient " +
				to_string(craterGradient);
			// assign to vectors
			
		}
	}
	writeLines(filePath, saveLines);
}

void worldGeneration::startTriangle() {
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
	int vertShader = createShader("assets/shaders/terrainVert.txt", GL_VERTEX_SHADER);
	int fragShader = createShader("assets/shaders/terrainFrag.txt", GL_FRAGMENT_SHADER);
	terrainShader = createProgram({ vertShader, fragShader });
}

vector<triangle> flatTerrainTriangles;
vector<float> flatXPoints; vector<float> flatZPoints;
void worldGeneration::beginFlatTerrain() {
	vec3 colour;
	vec2 areaScale = vec2(0.0f);
	if (currentArea == PLANET_WORLD) {
		colour = vec3(0.35f, 0.78f, 0.31f);
		areaScale = getVec2File(worldLinesPath, "planetEarthSize");
	}
	float triangleSize = 1.0f;
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
				
				vec3 triangleColour = colour + colourDifference(0.2f);

				triangle newTriangle;
				newTriangle.allPoints = { whichPoint[t], pointTwo, pointThree };
				newTriangle.colour = triangleColour;

				for (vec3 v : newTriangle.allPoints) {
					int size = flatXPoints.size();
					flatXPoints.resize(size + 1);
					flatZPoints.resize(size + 1);
					flatXPoints[size] = v.x;
					flatZPoints[size] = -v.z;
				}

				int vectorSize = flatTerrainTriangles.size();
				flatTerrainTriangles.resize(vectorSize + 1);
				flatTerrainTriangles[vectorSize] = newTriangle;
			}
		}
	}
	beginMountains();
}

vector<triangle> mountainTriangles;
vector<vec2> negativeMountainCoords;
vector<vec2> positiveMountainCoords;
void worldGeneration::beginMountains() {
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
	// variables
	int radiusDifference = 30;
	// start triangles
	for (vec2 pos : mountainPositions) {
		vec2 sca = mountainScales[index];
		float gra = mountainGradients[index];
		// create circle radiuses
		vector<int> radiuses;
		float loopDifference = radiusDifference / 100.0f;
		for (float r = 0.2f; r < sca.x; r += loopDifference) { // the lowest needs to be 10
			int size = radiuses.size();
			radiuses.resize(size + 1);
			radiuses[size] = (int)std::round(r*100.0f);
		}
		// reverse radiuses so 1st is highest
		reverse(radiuses.begin(), radiuses.end()); 
		// remove triangles over craters / under mountains
		if (radiuses.size() > 0) {
			removeUselessTriangle(radiuses[0], pos, defaultScale);
		}
		// create circles & triangles here
		for (int radius : radiuses) {
			int currentPointCount = (int)std::round(radius / 4) * 2;
			currentPointCount = (int)std::round(currentPointCount / 5);
			vector<vec2> currentCircle = circleCoords(pos, (float) radius, currentPointCount, defaultScale);
			int nextRadius = radius - radiusDifference;
			if (nextRadius <= 10) {
				break;
			}
			int nextPointCount = (int)std::round(nextRadius / 4) * 2;
			nextPointCount = (int)std::round(nextPointCount / 5);
			vector<vec2> nextCircle = circleCoords(pos, (float) nextRadius, nextPointCount, defaultScale);
			int pointIndex = 0;
			for (vec2 point : currentCircle) {
				int nextIndex = pointIndex + 1;
				int circleSize = currentCircle.size();
				if (nextIndex > circleSize - 1) {
					nextIndex = 0;
				}
				// triangle y positions
				float centralYPos = (radiuses[0] - radius + (10 * gra)) / radiuses[0];
				float innerYPos = (radiuses[0] - nextRadius + (10 * gra)) / radiuses[0];
				if (gra < 0.0f) {
					centralYPos = -centralYPos;
					innerYPos = -innerYPos;
				}
				innerYPos = clamp(innerYPos, -1.0f, 1.0f);
				centralYPos = clamp(centralYPos, -1.0f, 1.0f);
				if (radius == radiuses[0]) {
					centralYPos = 0.0f;
				}
				// line to make circle points into a ring
				vec3 pointOne = vec3(currentCircle[pointIndex].x, centralYPos * sca.y, -currentCircle[pointIndex].y);
				vec3 pointTwo = vec3(currentCircle[nextIndex].x, centralYPos * sca.y, -currentCircle[nextIndex].y);
				// connect to grid
				if (radiuses[0] == radius) {
					vector<vec3> bothPoints = { pointOne, pointTwo };
					for (int i = 0; i < 2; i++) {
						vector<float>::iterator xPosition = lower_bound(flatXPoints.begin(), flatXPoints.end(), bothPoints[i].x);
						int indexX = xPosition - flatXPoints.begin();
						if (indexX == flatXPoints.size()) {
							indexX = indexX - 1;
						}
						float newX = flatXPoints[indexX];
						// z
						vector<float>::iterator zPosition = upper_bound(flatZPoints.begin(), flatZPoints.end(), -bothPoints[i].z); // z positions are positive needed to be chaneged back
						int indexZ = zPosition - flatZPoints.begin();
						if (indexZ == flatZPoints.size()) {
							indexZ = indexZ - 1;
						}
						float newZ = -flatZPoints[indexZ];
						// assign
						bothPoints[i] = vec3(newX, bothPoints[i].y, newZ);
					}
					pointOne = bothPoints[0]; pointTwo = bothPoints[1];
				}
				// line going up & to the right
				int nextCircleIndex = (int)(floor(pointIndex / (currentPointCount / nextPointCount)));
				int nextCircleSize = nextCircle.size();
				if (nextCircleIndex > nextCircleSize - 1) {
					nextCircleIndex = 0;
				}
				vec3 pointThree = vec3(nextCircle[nextCircleIndex].x, innerYPos * sca.y, -nextCircle[nextCircleIndex].y);
				// line going up & to the left
				int lastIndex = nextCircleIndex - 1;
				if (lastIndex < 0) {
					lastIndex = nextCircle.size() - 1;
				}
				vec3 pointFour = vec3(nextCircle[lastIndex].x, innerYPos * sca.y, -nextCircle[lastIndex].y);
				// check if points exceed the area scale and clip them back
				vector<vec3> allPoints = { pointOne, pointTwo, pointThree, pointFour };
				for (int p = 0; p < 4; p++) {
					vec3 currentPoint = allPoints[p];
					currentPoint.x = clamp(currentPoint.x, 0.0f, currentAreaScale.x);
					currentPoint.z = clamp(currentPoint.z, -(currentAreaScale.y+1.0f), 0.0f);
					allPoints[p] = currentPoint;
				}
				pointOne = allPoints[0]; pointTwo = allPoints[1]; pointThree = allPoints[2]; pointFour = allPoints[3];
				// create triangles
				vec3 whichPoint[2] = { pointThree, pointFour };
				for (int t = 0; t < 2; t++) {
					vec3 triangleColour = colour + colourDifference(0.2f);

					triangle newTriangle;
					newTriangle.allPoints = { pointOne, pointTwo, whichPoint[t] };
					newTriangle.colour = triangleColour;

					int triangleCount = mountainTriangles.size();
					mountainTriangles.resize(triangleCount + 1);
					mountainTriangles[triangleCount] = newTriangle;
				}
				vec3 triangleColour = colour + colourDifference(0.2f);

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

void worldGeneration::removeUselessTriangle(int radius, vec2 position, float circleMultiplier){
	// collect triangles over craters / under mountains
	float newRadius = radius * circleMultiplier;
	int triangleIndex = 0;
	for (triangle t : flatTerrainTriangles) {
		vector<vec3> allPoints = t.allPoints;
		int insideCount = 0;
		for (vec3 v : allPoints) {
			if (insideCircle(position, newRadius, vec2(v.x, -v.z))) {
				insideCount = insideCount + 1;
			}
		}
		if (insideCount == 3) {
			flatTerrainTriangles[triangleIndex].colour = vec3(0.0f, 0.0f, 1.0f);
			flatTerrainTriangles[triangleIndex].active = false;
		}
		triangleIndex++;
	}
}

int total = 0;
void worldGeneration::beginTerrrain(){
	vector<float> allVertices;
	int triangleSize = 0;
	// get triangles into float vector
	vector<vector<triangle>> bothVectors = { flatTerrainTriangles, mountainTriangles };
	for (int o = 0; o < 2; o++) {
		vector<triangle> currentVector = bothVectors[o];
		int triangleCount = currentVector.size();
		for (int f = 0; f < triangleCount; f++) {
			if (!currentVector[f].active) {
				continue;
			}
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
			triangleSize = triangleSize + 1;
		}
	}
	// vector to array
	int floatCount = triangleSize*12;
	glBindVertexArray(terrainVAO);
	glBindBuffer(GL_ARRAY_BUFFER_ARB, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER_ARB, allVertices.size() * sizeof(float), allVertices.data(), GL_STATIC_DRAW_ARB);
	total = triangleSize + 3;
}

void worldGeneration::renderTerrain() {
	if (currentArea == PLANET_WORLD) {
		int modelLocation = glGetUniformLocation(terrainShader, "model");
		int projectionLocation = glGetUniformLocation(terrainShader, "projection");
		int viewLocation = glGetUniformLocation(terrainShader, "view");
		glUseProgram(terrainShader);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(modelMatrix()));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(projectionMatrix()));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(viewMatrix()));
		glBindVertexArray(terrainVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * total);
	}
}

vec2 worldGeneration::getAreaScale() {
	if (currentArea == PLANET_WORLD) {
		return getVec2File(worldLinesPath, "planetEarthSize");
	}
	return vec2(0.0f, 0.0f);
}

void worldGeneration::begin() {
	currentAreaScale = getAreaScale();
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
