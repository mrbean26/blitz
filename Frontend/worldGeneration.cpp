#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <stb_image.h>
using namespace std;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vector_relational.hpp>
#include <glm/ext/scalar_constants.hpp>
using namespace glm;

#include <glew.h>

#include "display.h"
#include "shader.h"
#include "saveFiles.h"
#include "worldGeneration.h"
#include "frontend.h"

bool insideCircle(vec2 circlePos, float radius, vec2 pointPos, bool terrain) {
	float xSquared = pow(pointPos.x - circlePos.x, 2);
	float ySquared = pow(pointPos.y - circlePos.y, 2);

	float radiusSquared = pow(radius, 2); // pow(2) = squared
	if (terrain) { radiusSquared -= 10; } // remove less triangles if used for terrain

	if (xSquared + ySquared <= radiusSquared) { // pythagoras
		return true;
	}
	return false;
}

int randomSeedCount = 5;
int randomSeed() {
	randomSeedCount++;
	return randomSeedCount;
}

int randomInt(int min, int max) {
	max = max + 1; // includes maximum number
	srand(randomSeed());
	int random = rand() % (max - min) + min;
	return random;
}

int randomColourInt(int min, int max) {
	int random = rand();
	return random % max;
}

vec3 colourDifference(float multiplier) {
	float colourDifferenceX = ((float)randomColourInt(-127, 127) / 255.0f) * multiplier;
	float colourDifferenceY = ((float)randomColourInt(-127, 127) / 255.0f) * multiplier;
	float colourDifferenceZ = ((float)randomColourInt(-127, 127) / 255.0f) * multiplier; // divide by 255 to get rgb for opengl
	return vec3(colourDifferenceX, colourDifferenceY, colourDifferenceZ);
}

vector<vec2> circleCoords(vec2 position, float radius, int pointCount, float scale) {
	// math values
	float k = 2.0f * pi<float>() / pointCount;
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
		coords[newVectorPos(&coords)] = vec2(x, y);
	}
	return coords;
}

bool insideBuildBench(vec2 currentPos, float currentScale, vec2 areaScale) {
	vec2 benchPos = vec2(areaScale.x - 1.0f, areaScale.y / 2.0f);
	currentScale = currentScale * 1.2f;
	currentScale = (currentScale * 100.0f) * 0.025f;
	if (insideCircle(currentPos, currentScale, benchPos)) {
		return true;
	}
	return false;
}

bool insideMountain(vector<vec2> allPositions, vector<float> allScales,
	vec2 currentPos, float currentScale) {
	vector<vec2> allPoints = circleCoords(currentPos, currentScale, 720, 1.0f);
	for (vec2 point : allPoints) {
		int posCount = allPositions.size();
		for (int i = 0; i < posCount; i++) {
			float vectorScale = (allScales[i] * 100.0f) * 0.025f; // the scale used when rendering mountains
			if (insideCircle(allPositions[i], vectorScale * 2, point)) {
				return true;
			}
		}
	}
	return false;
}

void createSave(const char* filePath, int saveType) {
	vector<string> saveLines = { "IN USE" };
	if (saveType == DEFAULT_SAVE || LARGE_WORLD) {
		//inventory and hotbar with no items (start of world)
		saveLines[newVectorPos(&saveLines)] = "saveType " + to_string(saveType);
		saveLines[newVectorPos(&saveLines)] = "inventory -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,"; //15 slots
		saveLines[newVectorPos(&saveLines)] = "inventoryQuantity 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,";
		saveLines[newVectorPos(&saveLines)] = "currentWeapons 0 0 0 0";
		saveLines[newVectorPos(&saveLines)] = "currentTime 0.0";
		saveLines[newVectorPos(&saveLines)] = "playerPos 0 0 0";
		saveLines[newVectorPos(&saveLines)] = "playerHealth 100";
		//"current" data
		saveLines[newVectorPos(&saveLines)] = "currentPlanet earth"; //basic science base on earth
		saveLines[newVectorPos(&saveLines)] = "currentPosition 0.0f, 0.0f, 0.0f";
		//EARTH SAVED DATA----------------------------------------------------------------------
		//land scale
		int earthScaleX = randomInt(35, 65);
		if(saveType == LARGE_WORLD){
			earthScaleX = randomInt(100, 200);
		}
		int earthScaleY = 100 - earthScaleX;
		if(saveType == LARGE_WORLD){
			earthScaleY = 300 - earthScaleX;
		}
		saveLines[newVectorPos(&saveLines)] = "planetEarthSize " + to_string(earthScaleX) + " " + to_string(earthScaleY);
		saveLines = rewriteLine(saveLines, "playerPos", to_string(earthScaleX / 2.0f) + " 0 " + to_string(earthScaleY / -2.0f));
		// lists to check if inside each other
		vector<vec2> craterPositions;
		vector<float> craterScales;
		int earthMountainCount = randomInt(1, 8);
		if(saveType == LARGE_WORLD){
			earthMountainCount = randomInt(1, 20);
		}
		int earthCraterCount = 10 - earthMountainCount;
		if(saveType == LARGE_WORLD){
			earthCraterCount = 22 - earthMountainCount;
		}
		int failedCraterAttempts = 0;
		// create and write to file
		for (int i = 0; i < earthMountainCount + earthCraterCount; i++) {
			// check if crater
			bool crater = false;
			if (i > earthMountainCount - 1) {
				crater = true;
			}
			//mountain scale
			float mountainScaleX = (randomInt(20, earthScaleX) * 0.1f);
			if(saveType == LARGE_WORLD){
				mountainScaleX = (randomInt(20, 100) * 0.1f);
			}
			float mountainScaleZ = (randomInt(20, 60) * 0.1f);
			//mountain position
			float mountainPositionX = (randomInt(0, earthScaleX * 10) * 0.1f);
			float mountainPositionY = (randomInt(0, earthScaleY * 10) * 0.1f);
			vec2 pos = vec2(mountainPositionX, mountainPositionY);
			//gradient/steepness
			float mountainGradient = (randomInt(3, 8) * 0.1f);
			if (crater) { mountainGradient *= -1.0f; }
			// check if inside mountain
			if (insideMountain(craterPositions, craterScales, pos, mountainScaleX)) {
				i = i - 1;
				failedCraterAttempts++;
				if (failedCraterAttempts > 19) {
					i = i + 1;
					failedCraterAttempts = 0;
				}
				continue;
			}
			if (insideBuildBench(pos, mountainScaleX, vec2(earthScaleX, earthScaleY))) {
				i = i - 1;
				failedCraterAttempts++;
				if (failedCraterAttempts > 19) {
					i = i + 1;
					failedCraterAttempts = 0;
				}
				continue;
			}
			//write to file
			saveLines[newVectorPos(&saveLines)] = "earthMountainPosition " +
				to_string(mountainPositionX) + " " + to_string(mountainPositionY);
			saveLines[newVectorPos(&saveLines)] = "earthMountainScale " +
				to_string(mountainScaleX) + " " + to_string(mountainScaleZ);
			saveLines[newVectorPos(&saveLines)] = "earthMountainGradient " +
				to_string(mountainGradient);
			// assign to vectors
			craterPositions[newVectorPos(&craterPositions)] = pos;
			craterScales[newVectorPos(&craterScales)] = mountainScaleX;
		}
		saveLines[newVectorPos(&saveLines)] = "earthBuildingType 2"; // bench
		saveLines[newVectorPos(&saveLines)] = "earthBuildingPosition " + to_string(earthScaleX - 1.0f) + " " + to_string(earthScaleY / -2.0f);
		saveLines[newVectorPos(&saveLines)] = "earthBuildingRotation 0.0";
	}
	writeLines(filePath, saveLines);
}

GLuint loadCubemapTexture(vector<string> faces){
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++){
		unsigned char * data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data){
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 
				width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			cout << "Cubemap texture failed to load at path: " << faces[i] << endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureId;
}
 
unsigned int skyboxVAO, skyboxVBO, skyboxTexture, skyboxShader;
void startSkybox(){
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	// reserve memory
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// texture
	vector<string> faces = {
		"assets/images/skybox/right.jpg",
		"assets/images/skybox/left.jpg",
		"assets/images/skybox/bottom.jpg",
		"assets/images/skybox/top.jpg",
		"assets/images/skybox/front.jpg",
		"assets/images/skybox/back.jpg"
	};
	skyboxTexture = loadCubemapTexture(faces);
	// shader
	int vertex = createShader("assets/shaders/skyboxVert.txt", GL_VERTEX_SHADER);
	int fragment = createShader("assets/shaders/skyboxFrag.txt", GL_FRAGMENT_SHADER);
	skyboxShader = createProgram({ vertex, fragment });
	setShaderInt(skyboxShader, "skybox", 0);
}

void renderSkybox(bool startScreen){
	glDepthFunc(GL_LEQUAL);
	glUseProgram(skyboxShader);
	mat4 newView = mat4(1.0f);
	if (startScreen) {
		newView = rotate(newView, radians((float) glfwGetTime() * 10.0f), vec3(0.0f, 1.0f, 0.0f));
		setShaderFloat(skyboxShader, "lightBrightness", 1.0f);
	}
	if (!startScreen) {
		newView = mat4(mat3(viewMatrix()));
		setShaderFloat(skyboxShader, "lightBrightness", lowestLight);
	}
	setMat4(skyboxShader, "view", -newView);
	setMat4(skyboxShader, "projection", projectionMatrix());
	// draw
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

vec3 lightPos = vec3(30.0f, 0.0f, -20.0f);
float lightIntensity = 1.0f, lightRadius = 1.0f, lowestLight = 0.3f;
float currentTime = 0.0f, timeMultiplier = 0.025f; // 0.25f == 3 min daytime

void worldGeneration::reserveMemory() {
	glGenVertexArrays(1, &terrainVAO);
	glGenBuffers(1, &terrainVBO);

	glBindVertexArray(terrainVAO);
	glBindBuffer(GL_ARRAY_BUFFER_ARB, terrainVBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // position attribute

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // colour attribute
}

void worldGeneration::startShader() {
	int vertShader = createShader("assets/shaders/terrainVert.txt", GL_VERTEX_SHADER);
	int fragShader = createShader("assets/shaders/terrainFrag.txt", GL_FRAGMENT_SHADER);
	terrainShader = createProgram({ vertShader, fragShader });
}

vector<triangle> flatTerrainTriangles, mountainTriangles;
vector<float> flatXPoints, flatZPoints;
float triangleSize = 1.0f;
void worldGeneration::beginFlatTerrain() {
	vec2 areaScale = vec2(0.0f);
	if (currentArea == PLANET_WORLD) {
		currentAreaColour = vec3(0.35f, 0.78f, 0.31f);
		areaScale = getVec2File(worldLinesPath, "planetEarthSize");
	}
	for (int x = -50; x < (areaScale.x + 50) / triangleSize; x++) {
		for (int y = -50; y < (areaScale.y + 50) / triangleSize; y++) {
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
				// new Triangle
				vec3 triangleColour = currentAreaColour + colourDifference(0.2f);
				triangle newTriangle;
				newTriangle.allPoints = { whichPoint[t], pointTwo, pointThree };
				newTriangle.colour = triangleColour;

				int insideMountainCount = 0;
				for (vec3 v : newTriangle.allPoints) {
					flatXPoints[newVectorPos(&flatXPoints)] = v.x;
					flatZPoints[newVectorPos(&flatZPoints)] = -v.z;

					int mountainCount = currentAllMountainPositions.size();
					for(int m = 0; m < mountainCount; m++){
						vec2 positionMountain = currentAllMountainPositions[m];
						vec3 scaleMountain = currentAllMountainScales[m];
						float mountainRadius = scaleMountain.x * 100.0f * 0.025f;

						if(insideCircle(positionMountain, mountainRadius, vec2(v.x, -v.z), true)){
							insideMountainCount++;
						}
					}
				}

				if(insideMountainCount == 3){ continue; }

				flatTerrainTriangles[newVectorPos(&flatTerrainTriangles)] = newTriangle;
			}
		}
	}
	beginTerrrain();
}

vector<vec2> currentAllMountainPositions;
vector<vec3> currentAllMountainScales;
vec2 currentPlanetScale;

vec4 terrainColliders(vec3 original, float yAddition){
	int mCount = currentAllMountainPositions.size();
	float yHighest = yAddition;
	int inMountainIndex = -1;
	for(int m = 0; m < mCount; m++){
		vec2 mPos = currentAllMountainPositions[m];
		mPos.y = -mPos.y;

		vec3 mSca = currentAllMountainScales[m];
		bool crater = mSca.z < 0;
		float currentRad = (mSca.x * 100.0f) * 0.025f;

		vec3 mThree = vec3(mPos.x, 0.0f, mPos.y);
		vec3 originalThree = vec3(original.x, 0.0f, original.z);

		float distance = glm::distance(mThree, originalThree);
		distance = currentRad - distance;
		if(distance > 0){
			inMountainIndex = m + 1;
			distance = distance / currentRad;
			distance = glm::clamp(distance, -1.0f, 1.0f);
			float pointY = distance * mSca.y;
			if(crater){
				pointY = -pointY;
			}
			pointY += yAddition;
			yHighest = pointY;
		}
	}
	return vec4(original.x, yHighest, original.z, (float) inMountainIndex);
}
#include "monsters.h"
vector<vec2> negativeMountainCoords, positiveMountainCoords;
void worldGeneration::beginMountains() {
	string mountainName;
	int lineCount = allWorldLines.size();
	if (currentArea == PLANET_WORLD) {
		mountainName = "earthMountain";
		currentAreaPrefix = "earth";
		currentAreaColour = vec3(0.35f, 0.78f, 0.31f);
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
			// add values
			mountainPositions[newVectorPos(&mountainPositions)] = currentLineMountainPos;
			mountainScales[newVectorPos(&mountainScales)] = mountainScale;
			mountainGradients[newVectorPos(&mountainGradients)] = mountainGradient;
		}
	}
	// assign to global
	currentAllMountainPositions = mountainPositions;
	int mCount = mountainPositions.size();
	for (int i = 0; i < mCount; i++) {
		currentAllMountainScales[newVectorPos(&currentAllMountainScales)] = vec3(mountainScales[i], mountainGradients[i]);
	}
	// mountain stats
	float defaultScale = 0.025f;
	// start triangles
	int index = -1;
	// variables
	int radiusDifference = 30;
	// start triangles
	for (vec2 pos : mountainPositions) {
		index++;
		vec2 sca = mountainScales[index];
		float gra = mountainGradients[index];
		bool mtn = false;
		if (gra > 0) {
			mtn = true;
		}
		// create circle radiuses
		vector<int> radiuses;
		float loopDifference = radiusDifference / 100.0f;
		for (float r = 0.2f; r < sca.x; r += loopDifference) { // the lowest needs to be 10
			radiuses[newVectorPos(&radiuses)] = (int)std::round(r * 100.0f);
		}
		// reverse radiuses so 1st is highest
		reverse(radiuses.begin(), radiuses.end());
		// create circles & triangles here
		for (int radius : radiuses) {
			int currentPointCount = (int)std::round(radius / 4) * 2;
			currentPointCount = (int)std::round(currentPointCount / 5);
			vector<vec2> currentCircle = circleCoords(pos, (float)radius, currentPointCount, defaultScale);
			int nextRadius = radius - radiusDifference;
			if (nextRadius <= 10) {
				break;
			}
			int nextPointCount = (int)std::round(nextRadius / 4) * 2;
			nextPointCount = (int)std::round(nextPointCount / 5);
			vector<vec2> nextCircle = circleCoords(pos, (float)nextRadius, nextPointCount, defaultScale);
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
				innerYPos = glm::clamp(innerYPos, -1.0f, 1.0f);
				centralYPos = glm::clamp(centralYPos, -1.0f, 1.0f);
				if (radius == radiuses[0]) {
					centralYPos = 0.0f;
				}
				// line to make circle points into a ring
				float yPos = (centralYPos * sca.y) - 0.01f;
				vec3 pointOne = vec3(currentCircle[pointIndex].x, yPos, -currentCircle[pointIndex].y);
				vec3 pointTwo = vec3(currentCircle[nextIndex].x, yPos, -currentCircle[nextIndex].y);
				if(radiuses[0] == radius){
					vector<vec3> bothPoints = { pointOne, pointTwo };
					for(int v = 0; v < 2; v++){
						vec3 current = bothPoints[v];
						if(current.x > pos.x){
							current.x = glm::ceil(current.x);
						}
						if(current.x <= pos.x){
							current.x = glm::floor(current.x);
						}
						if(current.z > -pos.y){
							current.z = ceil(current.z);
						}
						if(current.z < -pos.y){
							current.z = floor(current.z);
						}
						bothPoints[v] = current;
					}
					pointOne = bothPoints[0];
					pointTwo = bothPoints[1];
				}
				// line going up & to the right
				int nextCircleIndex = (int)(floor(pointIndex / (currentPointCount / nextPointCount)));
				int nextCircleSize = nextCircle.size();
				if (nextCircleIndex > nextCircleSize - 1) {
					nextCircleIndex = 0;
				}
				float innerY = (innerYPos * sca.y) - 0.01f;
				vec3 pointThree = vec3(nextCircle[nextCircleIndex].x, innerY, -nextCircle[nextCircleIndex].y);
				// line going up & to the left
				int lastIndex = nextCircleIndex - 1;
				if (lastIndex < 0) {
					lastIndex = nextCircle.size() - 1;
				}
				vec3 pointFour = vec3(nextCircle[lastIndex].x, innerY, -nextCircle[lastIndex].y);
				// check if points exceed the area scale and clip them back
				vector<vec3> allPoints = { pointOne, pointTwo, pointThree, pointFour };
				for (int p = 0; p < 4; p++) {
					vec3 currentPoint = allPoints[p];
					//currentPoint.x = clamp(currentPoint.x, 0.0f, currentAreaScale.x);
					//currentPoint.z = clamp(currentPoint.z, -(currentAreaScale.y + 1.0f), 0.0f);
					allPoints[p] = currentPoint;
				}
				pointOne = allPoints[0]; pointTwo = allPoints[1]; pointThree = allPoints[2]; pointFour = allPoints[3];
				// create triangles
				vec3 whichPoint[3] = { pointThree, pointFour, pointFour };
				vec3 whichPointTwo[3] = { pointTwo, pointTwo, pointThree };
				for (int t = 0; t < 3; t++) {
					vec3 triangleColour = currentAreaColour + colourDifference(0.2f);

					triangle newTriangle;
					newTriangle.allPoints = { pointOne, whichPointTwo[t], whichPoint[t] };
					newTriangle.colour = triangleColour;

					if(radiuses[0] == radius){
						newTriangle.edgeVectorIndexes = {0};
						if(t==0){
							newTriangle.edgeVectorIndexes[newVectorPos(&newTriangle.edgeVectorIndexes)] = 1;
						}
					}

					mountainTriangles[newVectorPos(&mountainTriangles)] = newTriangle;
				}
				pointIndex++;
			}
		}
	}
	beginFlatTerrain();
}

int total = 0;
void worldGeneration::beginTerrrain() {
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
			vector<int> edgeIndexes = currentVector[f].edgeVectorIndexes;
			int edgeVertexCount = edgeIndexes.size();
			for(int e = 0; e < edgeVertexCount; e++){
				break;
				vec3 currentVertex = points[edgeIndexes[e]];

				vector<float>::iterator xPosition = lower_bound(flatXPoints.begin(), flatXPoints.end(), currentVertex.x);
				int indexX = xPosition - flatXPoints.begin();
				if (indexX == flatXPoints.size()) {
					indexX = indexX - 1;
				}
				currentVertex.x = flatXPoints[indexX];

				vector<float>::iterator zPosition = upper_bound(flatZPoints.begin(), flatZPoints.end(), currentVertex.z); // z positions are positive needed to be chaneged back
				int indexZ = zPosition - flatZPoints.begin();
				if (indexZ == flatZPoints.size()) {
					indexZ = indexZ - 1;
				}
				currentVertex.z = flatZPoints[indexZ];

				//points[edgeIndexes[e]] = currentVertex;
			}
			for (vec3 point : points) {
				for (int v = 0; v < 3; v++) {
					allVertices[newVectorPos(&allVertices)] = point[v];
				}
				//color
				vec3 colour = currentVector[f].colour;
				for (int c = 0; c < 3; c++) {
					allVertices[newVectorPos(&allVertices)] = colour[c];
				}
			}
			triangleSize = triangleSize + 1;
		}
	}
	terrainVerts = triangleSize * 3;
	// vector to array
	int floatCount = triangleSize * 12;
	glBindVertexArray(terrainVAO);
	glBindBuffer(GL_ARRAY_BUFFER_ARB, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER_ARB, allVertices.size() * sizeof(float), allVertices.data(), GL_STATIC_DRAW_ARB);
	total = triangleSize;
}

void worldGeneration::renderAreaLimits(){
	// render
	glUseProgram(terrainShader);

	setMat4(terrainShader, "model", mat4(1.0f));
	setMat4(terrainShader, "projection", projectionMatrix());
	setMat4(terrainShader, "view", viewMatrix());
	setShaderFloat(terrainShader, "alpha", 1.0f);
	setShaderInt(terrainShader, "useLight", 0);
	
	glBindVertexArray(areaLimitVAO);
	glDrawArrays(GL_TRIANGLES, 0, areaLimitCount * 3);

	glLinkProgram(terrainShader); // reset uniform values for terrain render
}

void worldGeneration::renderTerrain() {
	if (currentArea == PLANET_WORLD) {
		glUseProgram(terrainShader);

		setMat4(terrainShader, "model", modelMatrix());
		setMat4(terrainShader, "projection", projectionMatrix());
		setMat4(terrainShader, "view", viewMatrix());

		setShaderVecThree(terrainShader, "lightPos", lightPos);
		setShaderFloat(terrainShader, "lightIntensity", lightIntensity);
		setShaderFloat(terrainShader, "lightRadius", lightRadius);
		setShaderInt(terrainShader, "useLight", 1);
		setShaderFloat(terrainShader, "lowestLight", lowestLight);

		glBindVertexArray(terrainVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * total);
	}
}

void worldGeneration::daynightCycle(){
	currentTime += deltaTime * timeMultiplier * 50.0f;
	float valueX = tan(radians(currentTime)) * currentAreaScale.x;
	lightRadius = currentAreaScale.x * 2;
	lightPos.x = valueX;
	lightPos.z = -currentAreaScale.y / 2.0f;
	// lightPos.z = valueZ;

	// lowest brightness
	float maxBrightness = 0.9f;
	float meanBrightness = 0.6f;
	float brightnessDifference = maxBrightness - meanBrightness;

	float xClamped = glm::clamp(valueX, 0.0f, currentAreaScale.x);
	float newValueX = xClamped - (currentAreaScale.x / 2.0f);
	float multiplier = (newValueX / (currentAreaScale.x / 2.0f)) * brightnessDifference;
	if (multiplier < 0) { multiplier *= -1.0f; }
	lowestLight = meanBrightness + (brightnessDifference - multiplier);
	
}

vec2 worldGeneration::getAreaScale() {
	if (currentArea == PLANET_WORLD) {
		return getVec2File(worldLinesPath, "planetEarthSize");
	}
	return vec2(0.0f, 0.0f);
}

void worldGeneration::begin() {
	currentPlanetScale = currentAreaScale = getAreaScale();
	allWorldLines = readLines(worldLinesPath);
	startShader();
	reserveMemory();
	beginMountains();
	beginAreaLimits();
	currentTime = getFloatFile(worldLinesPath, "currentTime");
	mainPlayer.health = getFloatFile(WorldGeneration.worldLinesPath, "playerHealth");
	mainPlayer.position = getVec3File(WorldGeneration.worldLinesPath, "playerPos");
}

void worldGeneration::mainloop() {
	if (!active) { return; }
	renderTerrain();
	renderAreaLimits();
	daynightCycle();
}

void worldGeneration::beginAreaLimits() {
	// generate points
	vector<float> points;
	vec3 colour = vec3(1.0f) - (currentAreaColour * vec3(0.5f));
	float triangleSize = 0.1f;
	float maxHeight = 30.0f;
	// red lines
	for (int w = 0; w < 2; w++) {
		float z[] = { 0.0f, -currentAreaScale.y };
		float yLast = terrainColliders(vec3(0.0f, 0.0f, z[w]), 0.135f).y;
		for (float x = -triangleSize; x < currentAreaScale.x; x += triangleSize) {
			vec4 terrain = terrainColliders(vec3(x + triangleSize, 0.0f, z[w]), 0.135f);
			float yNew = terrain.y;

			if (yNew < 0) {
				yNew = yNew + (-yNew * 0.175f);
			}
			if (yNew > -0.3f && yNew < 0.135f) {
				yNew = 0.135f;
			}

			vec3 one = vec3(x, yLast, -triangleSize + z[w]);
			vec3 two = vec3(x + triangleSize, yNew, -triangleSize + z[w]);
			vec3 three = vec3(x + triangleSize, yNew, triangleSize + z[w]);
			vec3 four = vec3(x, yLast, triangleSize + z[w]);

			vector<vec3> triangleOne = { one, two, four };
			vector<vec3> triangleTwo = { three, four, two };
			vector<vector<vec3>> bothTriangles = { triangleOne, triangleTwo };

			for (int t = 0; t < 2; t++) {
				vector<vec3> currentTriangle = bothTriangles[t];
				for (int p = 0; p < 3; p++) {
					points[newVectorPos(&points)] = currentTriangle[p].x;
					points[newVectorPos(&points)] = currentTriangle[p].y;
					points[newVectorPos(&points)] = currentTriangle[p].z;

					points[newVectorPos(&points)] = 1.0f;
					points[newVectorPos(&points)] = 0.0f;
					points[newVectorPos(&points)] = 0.0f;
				}
			}
			yLast = yNew;
		}
	}
	

	for (int w = 0; w < 2; w++) {
		float z[] = { 0.0f, currentAreaScale.x };
		float yLast = terrainColliders(vec3(z[w], 0.0f, 0.0f), 0.135f).y;
		for (float x = 0.0f; x < currentAreaScale.y; x += triangleSize) {
			vec4 terrain = terrainColliders(vec3(z[w], 0.0f, -x - triangleSize), 0.135f);
			float yNew = terrain.y;

			if (yNew < 0) {
				yNew = yNew + (-yNew * 0.175f);
			}
			if (yNew > -0.3f && yNew < 0.135f) {
				yNew = 0.135f;
			}

			vec3 one = vec3(-triangleSize + z[w], yLast, -x);
			vec3 two = vec3(-triangleSize + z[w], yNew, -x - triangleSize);
			vec3 three = vec3(triangleSize + z[w], yNew, -x - triangleSize);
			vec3 four = vec3(triangleSize + z[w], yLast, -x);

			vector<vec3> triangleOne = { one, two, four };
			vector<vec3> triangleTwo = { three, four, two };
			vector<vector<vec3>> bothTriangles = { triangleOne, triangleTwo };

			for (int t = 0; t < 2; t++) {
				vector<vec3> currentTriangle = bothTriangles[t];
				for (int p = 0; p < 3; p++) {
					points[newVectorPos(&points)] = currentTriangle[p].x;
					points[newVectorPos(&points)] = currentTriangle[p].y;
					points[newVectorPos(&points)] = currentTriangle[p].z;

					points[newVectorPos(&points)] = 1.0f;
					points[newVectorPos(&points)] = 0.0f;
					points[newVectorPos(&points)] = 0.0f;
				}
			}
			yLast = yNew;
		}
	}
	// memory
	glGenVertexArrays(1, &areaLimitVAO);
	glGenBuffers(1, &areaLimitVBO);
	glBindVertexArray(areaLimitVAO);
	glBindBuffer(GL_ARRAY_BUFFER, areaLimitVBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // position attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // colour attribute
	areaLimitCount = points.size() / 3;
}