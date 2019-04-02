using namespace std;
#include <vector>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <glew.h>

#include "display.h"
#include "shader.h"
#include "saveFiles.h"
#include "worldGeneration.h"

int randomInt(int min, int max){
	int random = rand();
	return min + (random % max);
}

int lineCount = 0;

int newLinePos(vector<string>& usedVector){
	int currentSize = usedVector.size();
	lineCount++;
	usedVector.resize(lineCount);
	return lineCount - 1;
}

void createSave(const char * filePath, int saveType){
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
			float mountainScaleY = (randomInt(0, earthScaleY) * 0.1f);
			float mountainScaleZ = (randomInt(0, 3) * 0.1f);
			//mountain position
			float mountainPositionX = (randomInt(0, earthScaleX * 10) * 0.1f);
			float mountainPositionY = (randomInt(0, earthScaleY * 10) * 0.1f);
			//gradient/steepness
			float mountainGradient = (randomInt(0, 20) * 0.1f);
			//write to file
			saveLines[newLinePos(saveLines)] = "earthMountainPosition " +
				to_string(mountainPositionX) + " " + to_string(mountainPositionY);
			saveLines[newLinePos(saveLines)] = "earthMountainScale " +
				to_string(mountainScaleX) + " " + to_string(mountainScaleY) +
				" " + to_string(mountainScaleZ);
			saveLines[newLinePos(saveLines)] = "earthMountainGradient " +
				to_string(mountainGradient);
		}
		//craters
		int earthCraterCount = 15 - earthMountainCount;
		for (int i = 0; i < earthCraterCount; i++) {
			//crater scale
			float craterScaleX = (randomInt(0, earthScaleX) * 0.1f);
			float craterScaleY = (randomInt(0, earthScaleY) * 0.1f);
			float craterScaleZ = (randomInt(0, 3) * 0.1f);
			//crater position
			float craterPositionX = (randomInt(0, earthScaleX * 10) * 0.1f);
			float craterPositionY = (randomInt(0, earthScaleY * 10) * 0.1f);
			//gradient/steepness
			float craterGradient = (randomInt(0, 20) * 0.1f);
			//save
			saveLines[newLinePos(saveLines)] = "earthCraterPosition " +
				to_string(craterPositionX) + " " + to_string(craterPositionY);
			saveLines[newLinePos(saveLines)] = "earthCraterScale " +
				to_string(craterScaleX) + " " + to_string(craterScaleY) +
				" " + to_string(craterScaleZ);
			saveLines[newLinePos(saveLines)] = "earthCraterGradient " +
				to_string(craterGradient);
		}
	}
	writeLines(filePath, saveLines);
}

void worldGeneration::startTriangle(){
	glGenVertexArrays(1, &vertexArray);
	glGenBuffers(1, &vertexBuffer);
	glBindVertexArray(vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices),
		triangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void worldGeneration::renderTriangle(vector<vec3> allPoints){
	glUseProgram(terrainShader);
	glBindVertexArray(vertexArray);
	glLoadIdentity();
	//update buffer data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	float vertices[9] = {
		allPoints[0].x, allPoints[0].y, allPoints[0].z,
		allPoints[1].x, allPoints[1].y, allPoints[1].z,
		allPoints[2].x, allPoints[2].y, allPoints[2].z
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
		vertices, GL_STATIC_DRAW);
	//draw
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void worldGeneration::startShader(){
	int vertShader = createShader(terrainVertSource, GL_VERTEX_SHADER);
	int fragShader = createShader(terrainFragSource, GL_FRAGMENT_SHADER);
	terrainShader = createProgram({ vertShader, fragShader });
}

vector<triangle> flatTerrainTriangles;
void worldGeneration::beginFlatTerrain(int area){
	vec3 colour;
	vec2 areaScale;
	if (area == PLANET_WORLD) {
		colour = vec3(0.35f, 0.78f, 0.31f);
		areaScale = getVec2File(worldLinesPath, "planetEarthSize");
	}
	float triangleSize = 2.0f;
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
				float colourDifferenceX = ((float)randomInt(-127, 127) / 255.0f) * 0.2f;
				float colourDifferenceY = ((float)randomInt(-127, 127) / 255.0f) * 0.2f;
				float colourDifferenceZ = ((float)randomInt(-127, 127) / 255.0f) * 0.2f;
				vec3 colourDifference = vec3(colourDifferenceX, colourDifferenceY, colourDifferenceZ);
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
}

void worldGeneration::renderFlatTerrain(){
	int alphaLocation = glGetUniformLocation(terrainShader, "alpha");
	int colourLocation = glGetUniformLocation(terrainShader, "inColour");
	int modelLocation = glGetUniformLocation(terrainShader, "model");
	int viewLocation = glGetUniformLocation(terrainShader, "view");
	int projectionLocation = glGetUniformLocation(terrainShader, "projection");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(modelMatrix()));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(viewMatrix()));
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(projectionMatrix()));
	
	int vectorSize = flatTerrainTriangles.size();
	for (int i = 0; i < vectorSize; i++) {
		triangle currentTriangle = flatTerrainTriangles[i];
		// to shader
		glUniform1f(alphaLocation, currentTriangle.alpha);
		vec3 colour = currentTriangle.colour;
		glUniform3f(colourLocation, colour.x, colour.y, colour.z);
		// render
		renderTriangle(currentTriangle.allPoints);
	}
}

void worldGeneration::renderTerrain(int currentArea){
	if (currentArea == PLANET_WORLD) {
		renderFlatTerrain();
	}
}

void worldGeneration::begin(){
	startShader();
	startTriangle();
	allWorldLines = readLines(worldLinesPath);
	beginFlatTerrain(PLANET_WORLD);
}

void worldGeneration::mainloop(){
	if (!active) {
		return;
	}
	renderTerrain(PLANET_WORLD);
}
