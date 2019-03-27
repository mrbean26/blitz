using namespace std;
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <glew.h>


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
	glUseProgram(0);
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

void worldGeneration::renderTerrain(vector<string> allLines, int currentArea){
	if (currentArea == PLANET_WORLD) {

	}
}

void worldGeneration::begin(){
	startTriangle();
	allWorldLines = readLines(worldLinesPath);
}

void worldGeneration::mainloop(){
	if (!active) {
		return;
	}
	renderTerrain(allWorldLines, PLANET_WORLD);
}
