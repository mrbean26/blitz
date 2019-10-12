#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "stb_image.h"
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
#include "player.h"
#include "structures.h"
#include "rocket.h"

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

int randomInt(int min, int max) {
    max = max + 1; // includes maximum number
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

bool insideRocket(vec2 currentPos, float currentScale, vec2 areaScale) {
	vec2 rocketPos = vec2(areaScale.x / 2.0f, 0.0f);
	currentScale = currentScale * 1.5f;
	currentScale = (currentScale * 100.0f) * 0.025f;
	if (insideCircle(currentPos, currentScale, rocketPos)) {
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
		saveLines[newVectorPos(&saveLines)] = "playerInRocket 0";
		saveLines[newVectorPos(&saveLines)] = "flyingRocket 0";
		//land scale
		int earthScaleX = randomInt(35, 65);
		if (saveType == LARGE_WORLD) {
			earthScaleX = randomInt(100, 200);
		}
		int earthScaleY = 100 - earthScaleX;
		if (saveType == LARGE_WORLD) {
			earthScaleY = 300 - earthScaleX;
		}
        //"current" data
        saveLines[newVectorPos(&saveLines)] = "currentPlanet earth"; //basic science base on earth
        saveLines[newVectorPos(&saveLines)] = "currentPosition 0.0f, 0.0f, 0.0f";
		saveLines[newVectorPos(&saveLines)] = "rocketPos " + to_string(earthScaleX / 2.0f) + " 10.1 " + "0.0";
		saveLines[newVectorPos(&saveLines)] = "rocketRot 0.0 0.0 0.0";
		saveLines[newVectorPos(&saveLines)] = "rocketSpeed 0.0";
        //EARTH SAVED DATA----------------------------------------------------------------------
        saveLines[newVectorPos(&saveLines)] = "planetEarthSize " + to_string(earthScaleX) + " " + to_string(earthScaleY);
		saveLines[newVectorPos(&saveLines)] = "earthDataPoints 0";
        saveLines = rewriteLine(saveLines, "playerPos", to_string(earthScaleX / 2.0f) + " 0 " + to_string(earthScaleY / -2.0f));
        // lists to check if inside each other
        vector<vec2> craterPositions;
        vector<float> craterScales;
        int earthMountainCount = randomInt(1, 8);
        if(saveType == LARGE_WORLD){
            earthMountainCount = randomInt(1, 11);
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
			if (insideRocket(pos, mountainScaleX, vec2(earthScaleX, earthScaleY))) {
				i = i - 1;
				failedCraterAttempts++;
				if (failedCraterAttempts > 19) {
					i = i + 1;
					failedCraterAttempts = 0;
				}
				continue;
			}
            // check if water in crater
            bool waterInCrater = false;
            if(mountainGradient < 0){
				waterInCrater = 1;
            }
            //write to file
            saveLines[newVectorPos(&saveLines)] = "earthMountainPosition " +
            to_string(mountainPositionX) + " " + to_string(mountainPositionY) + " " + to_string(waterInCrater);
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

void beginRandom(){
    vector<string> inputLines = readLines("assets/saves/inputs.save");
    int seed = stoi(inputLines[12]);
    srand(seed);
}

void endRandom(){
    vector<string> inputLines = readLines("assets/saves/inputs.save");
    int current = stoi(inputLines[12]);
    current += 1;
    inputLines[12] = to_string(current);
    writeLines("assets/saves/inputs.save", inputLines);
}

GLuint loadCubemapTexture(vector<string> faces){
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    
    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++){
        unsigned char * data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
                         width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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
        "assets/images/skybox/side.png",
        "assets/images/skybox/side.png",
        "assets/images/skybox/bottom.png",
        "assets/images/skybox/top.png",
        "assets/images/skybox/side.png",
        "assets/images/skybox/side.png"
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
		setShaderVecThree(skyboxShader, "skyColour", vec3(0.164f, 0.4f, 0.698f));
    }
    if (!startScreen) {
        newView = mat4(mat3(viewMatrix()));
		setShaderVecThree(skyboxShader, "skyColour", vec3(0.25f, 0.95f, 0.86f) * vec3(lowestLight));
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

buildingColour cloudOne, cloudTwo, cloudThree;
vector<vec4> allClouds;

void cloudMainloop(bool startScreen){
	spawnClouds();
	moveClouds();
	renderClouds(startScreen);
}

void moveClouds() {
	int cCount = allClouds.size();
	for (int c = 0; c < cCount; c++) {
		allClouds[c].x -= deltaTime * CLOUD_SPEED;

		allClouds[c].y = CLOUD_DEFAULT_HEIGHT + cos(radians(allClouds[c].x * CLOUD_FLOATING_MULTIPLIER));
	}
}

void spawnClouds() {
	vector<float> zPositions;
	auto iterator = allClouds.begin();
	while (iterator != allClouds.end()) {
		if ((*iterator).x < (-CLOUD_DEFAULT_SCALE / 2.0f)) {
			zPositions[newVectorPos(&zPositions)] = (*iterator).z;
			iterator = allClouds.erase(iterator);
		}
		else {
			iterator++;
		}
	}

	int zCount = zPositions.size();
	for (int z = 0; z < zCount; z++) {
		vec3 newCloudPos = vec3(0.0f, CLOUD_DEFAULT_HEIGHT, zPositions[z]);
		newCloudPos.x = (-CLOUD_DEFAULT_SCALE / 2.0f) + ((CLOUD_DEFAULT_SCALE / CLOUD_X_SLOTS) * (CLOUD_X_SLOTS));
		float type = (float) randomInt(0, 2);

		allClouds[newVectorPos(&allClouds)] = vec4(newCloudPos, type);
	}
}

void startClouds(){
	vector<float> cloudOneVerts = { -1.750000f, 0.500000f, -1.482152f, 0.983039f, 0.985196f, 0.996765f, -1.750000f, -0.500000f, -0.482152f, 0.983039f, 0.985196f, 0.996765f, -1.750000f, 0.500000f, -0.482152f, 0.983039f, 0.985196f, 0.996765f, 0.250000f, 0.500000f, -1.482152f, 0.991471f, 0.998333f, 0.995196f, -1.750000f, -0.500000f, -1.482152f, 0.991471f, 0.998333f, 0.995196f, -1.750000f, 0.500000f, -1.482152f, 0.991471f, 0.998333f, 0.995196f, 0.250000f, 0.500000f, -0.482152f, 0.984412f, 0.979118f, 0.982451f, 0.250000f, -0.500000f, -1.482152f, 0.984412f, 0.979118f, 0.982451f, 0.250000f, 0.500000f, -1.482152f, 0.984412f, 0.979118f, 0.982451f, -1.750000f, 0.500000f, -0.482152f, 0.990686f, 0.997941f, 0.990294f, 0.250000f, -0.500000f, -0.482152f, 0.990686f, 0.997941f, 0.990294f, 0.250000f, 0.500000f, -0.482152f, 0.990686f, 0.997941f, 0.990294f, -1.750000f, -0.500000f, -1.482152f, 0.982843f, 0.987353f, 0.985784f, 0.250000f, -0.500000f, -0.482152f, 0.982843f, 0.987353f, 0.985784f, -1.750000f, -0.500000f, -0.482152f, 0.982843f, 0.987353f, 0.985784f, 0.250000f, 0.500000f, -1.482152f, 0.996569f, 0.989510f, 0.975784f, -1.750000f, 0.500000f, -0.482152f, 0.996569f, 0.989510f, 0.975784f, 0.250000f, 0.500000f, -0.482152f, 0.996569f, 0.989510f, 0.975784f, -1.750000f, 0.500000f, -1.482152f, 0.975196f, 0.995000f, 0.976177f, -1.750000f, -0.500000f, -1.482152f, 0.975196f, 0.995000f, 0.976177f, -1.750000f, -0.500000f, -0.482152f, 0.975196f, 0.995000f, 0.976177f, 0.250000f, 0.500000f, -1.482152f, 0.999706f, 0.993039f, 0.980098f, 0.250000f, -0.500000f, -1.482152f, 0.999706f, 0.993039f, 0.980098f, -1.750000f, -0.500000f, -1.482152f, 0.999706f, 0.993039f, 0.980098f, 0.250000f, 0.500000f, -0.482152f, 0.982451f, 0.987353f, 0.979314f, 0.250000f, -0.500000f, -0.482152f, 0.982451f, 0.987353f, 0.979314f, 0.250000f, -0.500000f, -1.482152f, 0.982451f, 0.987353f, 0.979314f, -1.750000f, 0.500000f, -0.482152f, 0.984216f, 0.981471f, 0.991275f, -1.750000f, -0.500000f, -0.482152f, 0.984216f, 0.981471f, 0.991275f, 0.250000f, -0.500000f, -0.482152f, 0.984216f, 0.981471f, 0.991275f, -1.750000f, -0.500000f, -1.482152f, 0.997157f, 0.976765f, 0.982647f, 0.250000f, -0.500000f, -1.482152f, 0.997157f, 0.976765f, 0.982647f, 0.250000f, -0.500000f, -0.482152f, 0.997157f, 0.976765f, 0.982647f, 0.250000f, 0.500000f, -1.482152f, 0.996176f, 0.992843f, 0.994608f, -1.750000f, 0.500000f, -1.482152f, 0.996176f, 0.992843f, 0.994608f, -1.750000f, 0.500000f, -0.482152f, 0.996176f, 0.992843f, 0.994608f, -0.250000f, 0.500000f, 0.500000f, 0.977549f, 0.976961f, 0.978333f, -0.250000f, -0.500000f, 1.500000f, 0.977549f, 0.976961f, 0.978333f, -0.250000f, 0.500000f, 1.500000f, 0.977549f, 0.976961f, 0.978333f, 1.750000f, 0.500000f, 0.500000f, 0.997549f, 0.975196f, 0.987157f, -0.250000f, -0.500000f, 0.500000f, 0.997549f, 0.975196f, 0.987157f, -0.250000f, 0.500000f, 0.500000f, 0.997549f, 0.975196f, 0.987157f, 1.750000f, 0.500000f, 1.500000f, 0.990686f, 0.995980f, 0.978922f, 1.750000f, -0.500000f, 0.500000f, 0.990686f, 0.995980f, 0.978922f, 1.750000f, 0.500000f, 0.500000f, 0.990686f, 0.995980f, 0.978922f, -0.250000f, 0.500000f, 1.500000f, 0.993039f, 0.980490f, 0.992843f, 1.750000f, -0.500000f, 1.500000f, 0.993039f, 0.980490f, 0.992843f, 1.750000f, 0.500000f, 1.500000f, 0.993039f, 0.980490f, 0.992843f, -0.250000f, -0.500000f, 0.500000f, 0.986569f, 0.976961f, 0.978922f, 1.750000f, -0.500000f, 1.500000f, 0.986569f, 0.976961f, 0.978922f, -0.250000f, -0.500000f, 1.500000f, 0.986569f, 0.976961f, 0.978922f, 1.750000f, 0.500000f, 0.500000f, 0.991667f, 0.979510f, 0.998137f, -0.250000f, 0.500000f, 1.500000f, 0.991667f, 0.979510f, 0.998137f, 1.750000f, 0.500000f, 1.500000f, 0.991667f, 0.979510f, 0.998137f, -0.250000f, 0.500000f, 0.500000f, 0.994216f, 0.981275f, 0.992059f, -0.250000f, -0.500000f, 0.500000f, 0.994216f, 0.981275f, 0.992059f, -0.250000f, -0.500000f, 1.500000f, 0.994216f, 0.981275f, 0.992059f, 1.750000f, 0.500000f, 0.500000f, 0.992451f, 0.994020f, 0.978137f, 1.750000f, -0.500000f, 0.500000f, 0.992451f, 0.994020f, 0.978137f, -0.250000f, -0.500000f, 0.500000f, 0.992451f, 0.994020f, 0.978137f, 1.750000f, 0.500000f, 1.500000f, 0.999314f, 0.997353f, 0.993039f, 1.750000f, -0.500000f, 1.500000f, 0.999314f, 0.997353f, 0.993039f, 1.750000f, -0.500000f, 0.500000f, 0.999314f, 0.997353f, 0.993039f, -0.250000f, 0.500000f, 1.500000f, 0.987549f, 0.981667f, 0.976373f, -0.250000f, -0.500000f, 1.500000f, 0.987549f, 0.981667f, 0.976373f, 1.750000f, -0.500000f, 1.500000f, 0.987549f, 0.981667f, 0.976373f, -0.250000f, -0.500000f, 0.500000f, 0.979510f, 0.985196f, 0.992059f, 1.750000f, -0.500000f, 0.500000f, 0.979510f, 0.985196f, 0.992059f, 1.750000f, -0.500000f, 1.500000f, 0.979510f, 0.985196f, 0.992059f, 1.750000f, 0.500000f, 0.500000f, 0.983235f, 0.977745f, 0.985980f, -0.250000f, 0.500000f, 0.500000f, 0.983235f, 0.977745f, 0.985980f, -0.250000f, 0.500000f, 1.500000f, 0.983235f, 0.977745f, 0.985980f, -2.500000f, 0.500000f, -0.500000f, 0.977941f, 0.999510f, 0.997157f, -2.500000f, -0.500000f, 0.500000f, 0.977941f, 0.999510f, 0.997157f, -2.500000f, 0.500000f, 0.500000f, 0.977941f, 0.999510f, 0.997157f, 2.500000f, 0.500000f, -0.500000f, 0.996569f, 0.978922f, 0.997157f, -2.500000f, -0.500000f, -0.500000f, 0.996569f, 0.978922f, 0.997157f, -2.500000f, 0.500000f, -0.500000f, 0.996569f, 0.978922f, 0.997157f, 2.500000f, 0.500000f, 0.500000f, 0.976765f, 0.998333f, 0.988922f, 2.500000f, -0.500000f, -0.500000f, 0.976765f, 0.998333f, 0.988922f, 2.500000f, 0.500000f, -0.500000f, 0.976765f, 0.998333f, 0.988922f, -2.500000f, 0.500000f, 0.500000f, 0.984804f, 0.990882f, 0.977941f, 2.500000f, -0.500000f, 0.500000f, 0.984804f, 0.990882f, 0.977941f, 2.500000f, 0.500000f, 0.500000f, 0.984804f, 0.990882f, 0.977941f, -2.500000f, -0.500000f, -0.500000f, 0.992059f, 0.980686f, 0.977549f, 2.500000f, -0.500000f, 0.500000f, 0.992059f, 0.980686f, 0.977549f, -2.500000f, -0.500000f, 0.500000f, 0.992059f, 0.980686f, 0.977549f, 2.500000f, 0.500000f, -0.500000f, 0.976961f, 0.992255f, 0.978725f, -2.500000f, 0.500000f, 0.500000f, 0.976961f, 0.992255f, 0.978725f, 2.500000f, 0.500000f, 0.500000f, 0.976961f, 0.992255f, 0.978725f, -2.500000f, 0.500000f, -0.500000f, 0.985392f, 0.985588f, 0.997549f, -2.500000f, -0.500000f, -0.500000f, 0.985392f, 0.985588f, 0.997549f, -2.500000f, -0.500000f, 0.500000f, 0.985392f, 0.985588f, 0.997549f, 2.500000f, 0.500000f, -0.500000f, 0.978922f, 0.989902f, 0.989706f, 2.500000f, -0.500000f, -0.500000f, 0.978922f, 0.989902f, 0.989706f, -2.500000f, -0.500000f, -0.500000f, 0.978922f, 0.989902f, 0.989706f, 2.500000f, 0.500000f, 0.500000f, 0.997745f, 0.984804f, 0.988529f, 2.500000f, -0.500000f, 0.500000f, 0.997745f, 0.984804f, 0.988529f, 2.500000f, -0.500000f, -0.500000f, 0.997745f, 0.984804f, 0.988529f, -2.500000f, 0.500000f, 0.500000f, 0.981078f, 0.976373f, 0.975000f, -2.500000f, -0.500000f, 0.500000f, 0.981078f, 0.976373f, 0.975000f, 2.500000f, -0.500000f, 0.500000f, 0.981078f, 0.976373f, 0.975000f, -2.500000f, -0.500000f, -0.500000f, 0.988137f, 0.988137f, 0.994216f, 2.500000f, -0.500000f, -0.500000f, 0.988137f, 0.988137f, 0.994216f, 2.500000f, -0.500000f, 0.500000f, 0.988137f, 0.988137f, 0.994216f, 2.500000f, 0.500000f, -0.500000f, 0.979706f, 0.976373f, 0.978922f, -2.500000f, 0.500000f, -0.500000f, 0.979706f, 0.976373f, 0.978922f, -2.500000f, 0.500000f, 0.500000f, 0.979706f, 0.976373f, 0.978922f, };
	vector<float> cloudTwoVerts = { -1.500000f, 0.500000f, -1.000000f, 0.983039f, 0.985196f, 0.996765f, -1.500000f, -0.500000f, 1.000000f, 0.983039f, 0.985196f, 0.996765f, -1.500000f, 0.500000f, 1.000000f, 0.983039f, 0.985196f, 0.996765f, 1.500000f, 0.500000f, -1.000000f, 0.991471f, 0.998333f, 0.995196f, -1.500000f, -0.500000f, -1.000000f, 0.991471f, 0.998333f, 0.995196f, -1.500000f, 0.500000f, -1.000000f, 0.991471f, 0.998333f, 0.995196f, 1.500000f, 0.500000f, 1.000000f, 0.984412f, 0.979118f, 0.982451f, 1.500000f, -0.500000f, -1.000000f, 0.984412f, 0.979118f, 0.982451f, 1.500000f, 0.500000f, -1.000000f, 0.984412f, 0.979118f, 0.982451f, -1.500000f, 0.500000f, 1.000000f, 0.990686f, 0.997941f, 0.990294f, 1.500000f, -0.500000f, 1.000000f, 0.990686f, 0.997941f, 0.990294f, 1.500000f, 0.500000f, 1.000000f, 0.990686f, 0.997941f, 0.990294f, -1.500000f, -0.500000f, -1.000000f, 0.982843f, 0.987353f, 0.985784f, 1.500000f, -0.500000f, 1.000000f, 0.982843f, 0.987353f, 0.985784f, -1.500000f, -0.500000f, 1.000000f, 0.982843f, 0.987353f, 0.985784f, 1.500000f, 0.500000f, -1.000000f, 0.996569f, 0.989510f, 0.975784f, -1.500000f, 0.500000f, 1.000000f, 0.996569f, 0.989510f, 0.975784f, 1.500000f, 0.500000f, 1.000000f, 0.996569f, 0.989510f, 0.975784f, -1.500000f, 0.500000f, -1.000000f, 0.975196f, 0.995000f, 0.976177f, -1.500000f, -0.500000f, -1.000000f, 0.975196f, 0.995000f, 0.976177f, -1.500000f, -0.500000f, 1.000000f, 0.975196f, 0.995000f, 0.976177f, 1.500000f, 0.500000f, -1.000000f, 0.999706f, 0.993039f, 0.980098f, 1.500000f, -0.500000f, -1.000000f, 0.999706f, 0.993039f, 0.980098f, -1.500000f, -0.500000f, -1.000000f, 0.999706f, 0.993039f, 0.980098f, 1.500000f, 0.500000f, 1.000000f, 0.982451f, 0.987353f, 0.979314f, 1.500000f, -0.500000f, 1.000000f, 0.982451f, 0.987353f, 0.979314f, 1.500000f, -0.500000f, -1.000000f, 0.982451f, 0.987353f, 0.979314f, -1.500000f, 0.500000f, 1.000000f, 0.984216f, 0.981471f, 0.991275f, -1.500000f, -0.500000f, 1.000000f, 0.984216f, 0.981471f, 0.991275f, 1.500000f, -0.500000f, 1.000000f, 0.984216f, 0.981471f, 0.991275f, -1.500000f, -0.500000f, -1.000000f, 0.997157f, 0.976765f, 0.982647f, 1.500000f, -0.500000f, -1.000000f, 0.997157f, 0.976765f, 0.982647f, 1.500000f, -0.500000f, 1.000000f, 0.997157f, 0.976765f, 0.982647f, 1.500000f, 0.500000f, -1.000000f, 0.996176f, 0.992843f, 0.994608f, -1.500000f, 0.500000f, -1.000000f, 0.996176f, 0.992843f, 0.994608f, -1.500000f, 0.500000f, 1.000000f, 0.996176f, 0.992843f, 0.994608f, };
	vector<float> cloudThreeVerts = { -2.000000f, 0.500000f, 0.000000f, 0.983039f, 0.985196f, 0.996765f, -2.000000f, -0.500000f, 1.000000f, 0.983039f, 0.985196f, 0.996765f, -2.000000f, 0.500000f, 1.000000f, 0.983039f, 0.985196f, 0.996765f, -1.000000f, 0.500000f, 0.000000f, 0.991471f, 0.998333f, 0.995196f, -2.000000f, -0.500000f, 0.000000f, 0.991471f, 0.998333f, 0.995196f, -2.000000f, 0.500000f, 0.000000f, 0.991471f, 0.998333f, 0.995196f, -1.000000f, 0.500000f, 1.000000f, 0.984412f, 0.979118f, 0.982451f, -1.000000f, -0.500000f, 0.000000f, 0.984412f, 0.979118f, 0.982451f, -1.000000f, 0.500000f, 0.000000f, 0.984412f, 0.979118f, 0.982451f, -2.000000f, 0.500000f, 1.000000f, 0.990686f, 0.997941f, 0.990294f, -1.000000f, -0.500000f, 1.000000f, 0.990686f, 0.997941f, 0.990294f, -1.000000f, 0.500000f, 1.000000f, 0.990686f, 0.997941f, 0.990294f, -2.000000f, -0.500000f, 0.000000f, 0.982843f, 0.987353f, 0.985784f, -1.000000f, -0.500000f, 1.000000f, 0.982843f, 0.987353f, 0.985784f, -2.000000f, -0.500000f, 1.000000f, 0.982843f, 0.987353f, 0.985784f, -1.000000f, 0.500000f, 0.000000f, 0.996569f, 0.989510f, 0.975784f, -2.000000f, 0.500000f, 1.000000f, 0.996569f, 0.989510f, 0.975784f, -1.000000f, 0.500000f, 1.000000f, 0.996569f, 0.989510f, 0.975784f, -2.000000f, 0.500000f, 0.000000f, 0.975196f, 0.995000f, 0.976177f, -2.000000f, -0.500000f, 0.000000f, 0.975196f, 0.995000f, 0.976177f, -2.000000f, -0.500000f, 1.000000f, 0.975196f, 0.995000f, 0.976177f, -1.000000f, 0.500000f, 0.000000f, 0.999706f, 0.993039f, 0.980098f, -1.000000f, -0.500000f, 0.000000f, 0.999706f, 0.993039f, 0.980098f, -2.000000f, -0.500000f, 0.000000f, 0.999706f, 0.993039f, 0.980098f, -1.000000f, 0.500000f, 1.000000f, 0.982451f, 0.987353f, 0.979314f, -1.000000f, -0.500000f, 1.000000f, 0.982451f, 0.987353f, 0.979314f, -1.000000f, -0.500000f, 0.000000f, 0.982451f, 0.987353f, 0.979314f, -2.000000f, 0.500000f, 1.000000f, 0.984216f, 0.981471f, 0.991275f, -2.000000f, -0.500000f, 1.000000f, 0.984216f, 0.981471f, 0.991275f, -1.000000f, -0.500000f, 1.000000f, 0.984216f, 0.981471f, 0.991275f, -2.000000f, -0.500000f, 0.000000f, 0.997157f, 0.976765f, 0.982647f, -1.000000f, -0.500000f, 0.000000f, 0.997157f, 0.976765f, 0.982647f, -1.000000f, -0.500000f, 1.000000f, 0.997157f, 0.976765f, 0.982647f, -1.000000f, 0.500000f, 0.000000f, 0.996176f, 0.992843f, 0.994608f, -2.000000f, 0.500000f, 0.000000f, 0.996176f, 0.992843f, 0.994608f, -2.000000f, 0.500000f, 1.000000f, 0.996176f, 0.992843f, 0.994608f, -3.000000f, 0.500000f, -1.000000f, 0.977549f, 0.976961f, 0.978333f, -3.000000f, -0.500000f, 0.000000f, 0.977549f, 0.976961f, 0.978333f, -3.000000f, 0.500000f, 0.000000f, 0.977549f, 0.976961f, 0.978333f, -1.000000f, 0.500000f, -1.000000f, 0.997549f, 0.975196f, 0.987157f, -3.000000f, -0.500000f, -1.000000f, 0.997549f, 0.975196f, 0.987157f, -3.000000f, 0.500000f, -1.000000f, 0.997549f, 0.975196f, 0.987157f, -1.000000f, 0.500000f, 0.000000f, 0.990686f, 0.995980f, 0.978922f, -1.000000f, -0.500000f, -1.000000f, 0.990686f, 0.995980f, 0.978922f, -1.000000f, 0.500000f, -1.000000f, 0.990686f, 0.995980f, 0.978922f, -3.000000f, 0.500000f, 0.000000f, 0.993039f, 0.980490f, 0.992843f, -1.000000f, -0.500000f, 0.000000f, 0.993039f, 0.980490f, 0.992843f, -1.000000f, 0.500000f, 0.000000f, 0.993039f, 0.980490f, 0.992843f, -3.000000f, -0.500000f, -1.000000f, 0.986569f, 0.976961f, 0.978922f, -1.000000f, -0.500000f, 0.000000f, 0.986569f, 0.976961f, 0.978922f, -3.000000f, -0.500000f, 0.000000f, 0.986569f, 0.976961f, 0.978922f, -1.000000f, 0.500000f, -1.000000f, 0.991667f, 0.979510f, 0.998137f, -3.000000f, 0.500000f, 0.000000f, 0.991667f, 0.979510f, 0.998137f, -1.000000f, 0.500000f, 0.000000f, 0.991667f, 0.979510f, 0.998137f, -3.000000f, 0.500000f, -1.000000f, 0.994216f, 0.981275f, 0.992059f, -3.000000f, -0.500000f, -1.000000f, 0.994216f, 0.981275f, 0.992059f, -3.000000f, -0.500000f, 0.000000f, 0.994216f, 0.981275f, 0.992059f, -1.000000f, 0.500000f, -1.000000f, 0.992451f, 0.994020f, 0.978137f, -1.000000f, -0.500000f, -1.000000f, 0.992451f, 0.994020f, 0.978137f, -3.000000f, -0.500000f, -1.000000f, 0.992451f, 0.994020f, 0.978137f, -1.000000f, 0.500000f, 0.000000f, 0.999314f, 0.997353f, 0.993039f, -1.000000f, -0.500000f, 0.000000f, 0.999314f, 0.997353f, 0.993039f, -1.000000f, -0.500000f, -1.000000f, 0.999314f, 0.997353f, 0.993039f, -3.000000f, 0.500000f, 0.000000f, 0.987549f, 0.981667f, 0.976373f, -3.000000f, -0.500000f, 0.000000f, 0.987549f, 0.981667f, 0.976373f, -1.000000f, -0.500000f, 0.000000f, 0.987549f, 0.981667f, 0.976373f, -3.000000f, -0.500000f, -1.000000f, 0.979510f, 0.985196f, 0.992059f, -1.000000f, -0.500000f, -1.000000f, 0.979510f, 0.985196f, 0.992059f, -1.000000f, -0.500000f, 0.000000f, 0.979510f, 0.985196f, 0.992059f, -1.000000f, 0.500000f, -1.000000f, 0.983235f, 0.977745f, 0.985980f, -3.000000f, 0.500000f, -1.000000f, 0.983235f, 0.977745f, 0.985980f, -3.000000f, 0.500000f, 0.000000f, 0.983235f, 0.977745f, 0.985980f, -1.000000f, 0.500000f, -2.500000f, 0.977941f, 0.999510f, 0.997157f, -1.000000f, -0.500000f, -1.500000f, 0.977941f, 0.999510f, 0.997157f, -1.000000f, 0.500000f, -1.500000f, 0.977941f, 0.999510f, 0.997157f, 0.000000f, 0.500000f, -2.500000f, 0.996569f, 0.978922f, 0.997157f, -1.000000f, -0.500000f, -2.500000f, 0.996569f, 0.978922f, 0.997157f, -1.000000f, 0.500000f, -2.500000f, 0.996569f, 0.978922f, 0.997157f, 0.000000f, 0.500000f, -1.500000f, 0.976765f, 0.998333f, 0.988922f, 0.000000f, -0.500000f, -2.500000f, 0.976765f, 0.998333f, 0.988922f, 0.000000f, 0.500000f, -2.500000f, 0.976765f, 0.998333f, 0.988922f, -1.000000f, 0.500000f, -1.500000f, 0.984804f, 0.990882f, 0.977941f, 0.000000f, -0.500000f, -1.500000f, 0.984804f, 0.990882f, 0.977941f, 0.000000f, 0.500000f, -1.500000f, 0.984804f, 0.990882f, 0.977941f, -1.000000f, -0.500000f, -2.500000f, 0.992059f, 0.980686f, 0.977549f, 0.000000f, -0.500000f, -1.500000f, 0.992059f, 0.980686f, 0.977549f, -1.000000f, -0.500000f, -1.500000f, 0.992059f, 0.980686f, 0.977549f, 0.000000f, 0.500000f, -2.500000f, 0.976961f, 0.992255f, 0.978725f, -1.000000f, 0.500000f, -1.500000f, 0.976961f, 0.992255f, 0.978725f, 0.000000f, 0.500000f, -1.500000f, 0.976961f, 0.992255f, 0.978725f, -1.000000f, 0.500000f, -2.500000f, 0.985392f, 0.985588f, 0.997549f, -1.000000f, -0.500000f, -2.500000f, 0.985392f, 0.985588f, 0.997549f, -1.000000f, -0.500000f, -1.500000f, 0.985392f, 0.985588f, 0.997549f, 0.000000f, 0.500000f, -2.500000f, 0.978922f, 0.989902f, 0.989706f, 0.000000f, -0.500000f, -2.500000f, 0.978922f, 0.989902f, 0.989706f, -1.000000f, -0.500000f, -2.500000f, 0.978922f, 0.989902f, 0.989706f, 0.000000f, 0.500000f, -1.500000f, 0.997745f, 0.984804f, 0.988529f, 0.000000f, -0.500000f, -1.500000f, 0.997745f, 0.984804f, 0.988529f, 0.000000f, -0.500000f, -2.500000f, 0.997745f, 0.984804f, 0.988529f, -1.000000f, 0.500000f, -1.500000f, 0.981078f, 0.976373f, 0.975000f, -1.000000f, -0.500000f, -1.500000f, 0.981078f, 0.976373f, 0.975000f, 0.000000f, -0.500000f, -1.500000f, 0.981078f, 0.976373f, 0.975000f, -1.000000f, -0.500000f, -2.500000f, 0.988137f, 0.988137f, 0.994216f, 0.000000f, -0.500000f, -2.500000f, 0.988137f, 0.988137f, 0.994216f, 0.000000f, -0.500000f, -1.500000f, 0.988137f, 0.988137f, 0.994216f, 0.000000f, 0.500000f, -2.500000f, 0.979706f, 0.976373f, 0.978922f, -1.000000f, 0.500000f, -2.500000f, 0.979706f, 0.976373f, 0.978922f, -1.000000f, 0.500000f, -1.500000f, 0.979706f, 0.976373f, 0.978922f, 1.000000f, 0.500000f, 0.000000f, 0.984804f, 0.981667f, 0.988333f, 1.000000f, -0.500000f, 1.000000f, 0.984804f, 0.981667f, 0.988333f, 1.000000f, 0.500000f, 1.000000f, 0.984804f, 0.981667f, 0.988333f, 2.000000f, 0.500000f, 0.000000f, 0.983431f, 0.975000f, 0.979314f, 1.000000f, -0.500000f, 0.000000f, 0.983431f, 0.975000f, 0.979314f, 1.000000f, 0.500000f, 0.000000f, 0.983431f, 0.975000f, 0.979314f, 2.000000f, 0.500000f, 1.000000f, 0.984216f, 0.997353f, 0.995980f, 2.000000f, -0.500000f, 0.000000f, 0.984216f, 0.997353f, 0.995980f, 2.000000f, 0.500000f, 0.000000f, 0.984216f, 0.997353f, 0.995980f, 1.000000f, 0.500000f, 1.000000f, 0.990490f, 0.978333f, 0.975392f, 2.000000f, -0.500000f, 1.000000f, 0.990490f, 0.978333f, 0.975392f, 2.000000f, 0.500000f, 1.000000f, 0.990490f, 0.978333f, 0.975392f, 1.000000f, -0.500000f, 0.000000f, 0.996569f, 0.991078f, 0.978333f, 2.000000f, -0.500000f, 1.000000f, 0.996569f, 0.991078f, 0.978333f, 1.000000f, -0.500000f, 1.000000f, 0.996569f, 0.991078f, 0.978333f, 2.000000f, 0.500000f, 0.000000f, 0.993431f, 0.975588f, 0.975392f, 1.000000f, 0.500000f, 1.000000f, 0.993431f, 0.975588f, 0.975392f, 2.000000f, 0.500000f, 1.000000f, 0.993431f, 0.975588f, 0.975392f, 1.000000f, 0.500000f, 0.000000f, 0.981471f, 0.990686f, 0.993431f, 1.000000f, -0.500000f, 0.000000f, 0.981471f, 0.990686f, 0.993431f, 1.000000f, -0.500000f, 1.000000f, 0.981471f, 0.990686f, 0.993431f, 2.000000f, 0.500000f, 0.000000f, 0.997353f, 0.983824f, 0.976373f, 2.000000f, -0.500000f, 0.000000f, 0.997353f, 0.983824f, 0.976373f, 1.000000f, -0.500000f, 0.000000f, 0.997353f, 0.983824f, 0.976373f, 2.000000f, 0.500000f, 1.000000f, 0.984804f, 0.976373f, 0.999118f, 2.000000f, -0.500000f, 1.000000f, 0.984804f, 0.976373f, 0.999118f, 2.000000f, -0.500000f, 0.000000f, 0.984804f, 0.976373f, 0.999118f, 1.000000f, 0.500000f, 1.000000f, 0.975196f, 0.998333f, 0.993039f, 1.000000f, -0.500000f, 1.000000f, 0.975196f, 0.998333f, 0.993039f, 2.000000f, -0.500000f, 1.000000f, 0.975196f, 0.998333f, 0.993039f, 1.000000f, -0.500000f, 0.000000f, 0.994608f, 0.981667f, 0.992647f, 2.000000f, -0.500000f, 0.000000f, 0.994608f, 0.981667f, 0.992647f, 2.000000f, -0.500000f, 1.000000f, 0.994608f, 0.981667f, 0.992647f, 2.000000f, 0.500000f, 0.000000f, 0.993627f, 0.988137f, 0.993824f, 1.000000f, 0.500000f, 0.000000f, 0.993627f, 0.988137f, 0.993824f, 1.000000f, 0.500000f, 1.000000f, 0.993627f, 0.988137f, 0.993824f, -1.000000f, 0.500000f, -1.500000f, 0.991863f, 0.978725f, 0.990294f, -1.000000f, -0.500000f, 1.500000f, 0.991863f, 0.978725f, 0.990294f, -1.000000f, 0.500000f, 1.500000f, 0.991863f, 0.978725f, 0.990294f, 1.000000f, 0.500000f, -1.500000f, 0.995784f, 0.992843f, 0.984020f, -1.000000f, -0.500000f, -1.500000f, 0.995784f, 0.992843f, 0.984020f, -1.000000f, 0.500000f, -1.500000f, 0.995784f, 0.992843f, 0.984020f, 1.000000f, 0.500000f, 1.500000f, 0.989118f, 0.985588f, 0.989510f, 1.000000f, -0.500000f, -1.500000f, 0.989118f, 0.985588f, 0.989510f, 1.000000f, 0.500000f, -1.500000f, 0.989118f, 0.985588f, 0.989510f, -1.000000f, 0.500000f, 1.500000f, 0.989902f, 0.980882f, 0.991667f, 1.000000f, -0.500000f, 1.500000f, 0.989902f, 0.980882f, 0.991667f, 1.000000f, 0.500000f, 1.500000f, 0.989902f, 0.980882f, 0.991667f, -1.000000f, -0.500000f, -1.500000f, 0.977157f, 0.990490f, 0.993235f, 1.000000f, -0.500000f, 1.500000f, 0.977157f, 0.990490f, 0.993235f, -1.000000f, -0.500000f, 1.500000f, 0.977157f, 0.990490f, 0.993235f, 1.000000f, 0.500000f, -1.500000f, 0.975784f, 0.977157f, 0.976765f, -1.000000f, 0.500000f, 1.500000f, 0.975784f, 0.977157f, 0.976765f, 1.000000f, 0.500000f, 1.500000f, 0.975784f, 0.977157f, 0.976765f, -1.000000f, 0.500000f, -1.500000f, 0.987745f, 0.992647f, 0.991667f, -1.000000f, -0.500000f, -1.500000f, 0.987745f, 0.992647f, 0.991667f, -1.000000f, -0.500000f, 1.500000f, 0.987745f, 0.992647f, 0.991667f, 1.000000f, 0.500000f, -1.500000f, 0.976177f, 0.982059f, 0.995392f, 1.000000f, -0.500000f, -1.500000f, 0.976177f, 0.982059f, 0.995392f, -1.000000f, -0.500000f, -1.500000f, 0.976177f, 0.982059f, 0.995392f, 1.000000f, 0.500000f, 1.500000f, 0.984020f, 0.993235f, 0.975784f, 1.000000f, -0.500000f, 1.500000f, 0.984020f, 0.993235f, 0.975784f, 1.000000f, -0.500000f, -1.500000f, 0.984020f, 0.993235f, 0.975784f, -1.000000f, 0.500000f, 1.500000f, 0.995000f, 0.995196f, 0.994216f, -1.000000f, -0.500000f, 1.500000f, 0.995000f, 0.995196f, 0.994216f, 1.000000f, -0.500000f, 1.500000f, 0.995000f, 0.995196f, 0.994216f, -1.000000f, -0.500000f, -1.500000f, 0.979706f, 0.978529f, 0.983431f, 1.000000f, -0.500000f, -1.500000f, 0.979706f, 0.978529f, 0.983431f, 1.000000f, -0.500000f, 1.500000f, 0.979706f, 0.978529f, 0.983431f, 1.000000f, 0.500000f, -1.500000f, 0.991667f, 0.996569f, 0.994608f, -1.000000f, 0.500000f, -1.500000f, 0.991667f, 0.996569f, 0.994608f, -1.000000f, 0.500000f, 1.500000f, 0.991667f, 0.996569f, 0.994608f, };

	startColorBuilding(cloudOneVerts, &cloudOne);
	startColorBuilding(cloudTwoVerts, &cloudTwo);
	startColorBuilding(cloudThreeVerts, &cloudThree);

	for (int x = 0; x < CLOUD_X_SLOTS; x++) {
		for (int z = 0; z < CLOUD_Z_SLOTS; z++) {
			float type = (float) randomInt(0, 2);
			vec3 position = vec3(0.0f, 30.0f, 0.0f);

			position.x = (-CLOUD_DEFAULT_SCALE / 2.0f) + ((CLOUD_DEFAULT_SCALE / CLOUD_X_SLOTS) * (x + 0));
			position.z = (CLOUD_DEFAULT_SCALE / 4.0f) - ((CLOUD_DEFAULT_SCALE / CLOUD_Z_SLOTS) * (z + 0));

			if (z % 2 == 0) {
				if (x % 2 == 0) {
					position.z = position.z + (CLOUD_DEFAULT_SCALE / CLOUD_X_SLOTS) / 2.0f;
					position.x = position.x + (CLOUD_DEFAULT_SCALE / CLOUD_X_SLOTS) / 4.0f;
				}
			}

			allClouds[newVectorPos(&allClouds)] = vec4(position, type);
		}
	}
}

void renderClouds(bool startScreen){
	vector<buildingColour> cloudTypes = { cloudOne, cloudTwo, cloudThree };

	glUseProgram(playerShader);

	bool inWater = false;
	if (cameraThirdPos.y <= 0.05f) {
		int index = (int)terrainColliders(cameraThirdPos, 0.0f).w;
		index = index - 1;
		if (index > -1) {
			inWater = currentAllMountainWaters[index];
		}
	}

	setMat4(playerShader, "projection", projectionMatrix());
	setMat4(playerShader, "view", viewMatrix());

	setShaderInt(playerShader, "cameraInWater", inWater);
	setShaderVecThree(playerShader, "waterMultiplyColour", WorldGeneration.waterMultiplyColour);

	setShaderVecThree(playerShader, "multiplyColour", vec3(1.0f));
	setShaderVecThree(playerShader, "lightPos", lightPos);
	setShaderFloat(playerShader, "lightIntensity", lightIntensity);
	setShaderFloat(playerShader, "lightRadius", lightRadius);
	setShaderInt(playerShader, "useLight", !startScreen);
	setShaderFloat(playerShader, "lowestLight", lowestLight);
	if (startScreen) {
		setShaderVecThree(playerShader, "multiplyColour", vec3(0.6f));
	}

	int cCount = allClouds.size();
	for (int c = 0; c < cCount; c++) {
		vec4 currentCloud = allClouds[c];
		buildingColour cloud = cloudTypes[(int) currentCloud.w];

		setMat4(playerShader, "model", modelMatrix(vec3(currentCloud)));
		glBindVertexArray(cloud.VAO);
		glDrawArrays(GL_TRIANGLES, 0, cloud.size);
	}
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

vector<triangle> flatTerrainTriangles, mountainTriangles, waterTriangles;
vector<float> flatXPoints, flatZPoints;
void worldGeneration::beginFlatTerrain() {
    vec2 areaScale = vec2(0.0f);
    if (currentArea == PLANET_WORLD) {
        currentAreaColour = vec3(0.35f, 0.78f, 0.31f);
        areaScale = getVec2File(worldLinesPath, "planetEarthSize");
    }
	waterMultiplyColour = vec3(0.0f, 0.0f, currentAreaColour.z * 2.0f);
    for (int x = (int) -areaScale.x; x < (areaScale.x * 2.0f) / TRIANGLE_SIZE_FLAT; x++) {
        for (int y = (int) -areaScale.y; y < (areaScale.y * 2.0f) / TRIANGLE_SIZE_FLAT; y++) {
            // draw triangle
            // multipliers
            float xMultiplied = x * TRIANGLE_SIZE_FLAT;
            float yMultiplied = y * TRIANGLE_SIZE_FLAT;
            // points
            vec3 pointOne = vec3(xMultiplied, 0.0f, -yMultiplied);
            vec3 pointTwo = vec3(xMultiplied + TRIANGLE_SIZE_FLAT, 0.0f, -yMultiplied);
            vec3 pointThree = vec3(xMultiplied, 0.0f, -(yMultiplied + TRIANGLE_SIZE_FLAT));
            vec3 pointFour = vec3(xMultiplied + TRIANGLE_SIZE_FLAT, 0.0f, -(yMultiplied + TRIANGLE_SIZE_FLAT));
            vector<vec3> whichPoint = { pointOne, pointFour };
            // assign triangles to vector
            for (int t = 0; t < 2; t++) {
                // new Triangle
                vec3 triangleColour = currentAreaColour + colourDifference(0.2f);
                triangle newTriangle;
                newTriangle.allPoints = { whichPoint[t], pointTwo, pointThree };
                newTriangle.colour = triangleColour;
                
				int mIndex = -1;
                int insideMountainCount = 0;
                for (vec3 v : newTriangle.allPoints) {
                    flatXPoints[newVectorPos(&flatXPoints)] = v.x;
                    flatZPoints[newVectorPos(&flatZPoints)] = -v.z;
                    
                    int mountainCount = currentAllMountainPositions.size();
                    for(int m = 0; m < mountainCount; m++){
                        vec2 positionMountain = currentAllMountainPositions[m];
                        vec3 scaleMountain = currentAllMountainScales[m];
                        float mountainRadius = scaleMountain.x * 100.0f * 0.025f;
                        if(scaleMountain.z > 0.0f){
                            mountainRadius = mountainRadius * 0.9f;
                        }

						if (scaleMountain.x > 4.0f && scaleMountain.z < 0.0f) {
							mountainRadius -= (scaleMountain.x - 4.0f) * 0.75f;
						}

                        if(insideCircle(positionMountain, mountainRadius, vec2(v.x, -v.z), true)){
                            insideMountainCount++;
							mIndex = m;
                        }
                    }
                }
                
                if(insideMountainCount == 3){
					if (currentAllMountainWaters[mIndex]) {
						waterTriangles[newVectorPos(&waterTriangles)] = newTriangle;
					}
                    continue;
                }
                
                flatTerrainTriangles[newVectorPos(&flatTerrainTriangles)] = newTriangle;
            }
        }
    }
    beginWater();
}

vector<vec2> currentAllMountainPositions;
vector<bool> currentAllMountainWaters;
vector<vec3> currentAllMountainScales;
vec2 currentPlanetScale;

void worldGeneration::beginExternalFlat(){
	vector<float> vertices;
	for (int x = (int)(currentAreaScale.x / -2.0f); x < (int)((currentAreaScale.x / 2.0f) / TRIANGLE_SIZE_FLAT); x++) {
		for (int z = int(currentAreaScale.y / -2.0f); z < (int)((currentAreaScale.y / 2.0f) / TRIANGLE_SIZE_FLAT); z++) {
			float xMultiplied = x * TRIANGLE_SIZE_FLAT;
			float zMultiplied = z * TRIANGLE_SIZE_FLAT;

			vec3 pointOne = vec3(xMultiplied, 0.0f, -zMultiplied);
			vec3 pointTwo = vec3(xMultiplied + TRIANGLE_SIZE_FLAT, 0.0f, -zMultiplied);
			vec3 pointThree = vec3(xMultiplied, 0.0f, -(zMultiplied + TRIANGLE_SIZE_FLAT));
			vec3 pointFour = vec3(xMultiplied + TRIANGLE_SIZE_FLAT, 0.0f, -(zMultiplied + TRIANGLE_SIZE_FLAT));
			vector<vec3> whichPoint = { pointOne, pointFour };

			for (int t = 0; t < 2; t++) {
				vec3 colour = WorldGeneration.currentAreaColour + colourDifference(0.2f);
				vec3 points[] = { whichPoint[t], pointTwo, pointThree };

				for (int v = 0; v < 3; v++) {
					vec3 point = points[v];

					vertices[newVectorPos(&vertices)] = point.x;
					vertices[newVectorPos(&vertices)] = point.y;
					vertices[newVectorPos(&vertices)] = point.z;

					vertices[newVectorPos(&vertices)] = colour.x;
					vertices[newVectorPos(&vertices)] = colour.y;
					vertices[newVectorPos(&vertices)] = colour.z;
				}
			}
		}
	}

	startIrregularColorBuilding(vertices, externalVAO, externalVBO, externalSize);
}

void worldGeneration::beginWater(){
    vector<float> points;
    
    int tCount = waterTriangles.size();
    for(int t = 0; t < tCount; t++){
        triangle currentTriangle = waterTriangles[t];
        currentTriangle.colour = vec3(0.0f, 0.0f, currentAreaColour.z) + colourDifference(0.2f);
        
        for(int p = 0; p < 3; p++){
            points[newVectorPos(&points)] = currentTriangle.allPoints[p].x;
            points[newVectorPos(&points)] = currentTriangle.allPoints[p].y;
            points[newVectorPos(&points)] = currentTriangle.allPoints[p].z;
            
            points[newVectorPos(&points)] = currentTriangle.colour.x;
            points[newVectorPos(&points)] = currentTriangle.colour.y;
            points[newVectorPos(&points)] = currentTriangle.colour.z;
        }
    }
    
    // memory
    glGenVertexArrays(1, &waterVAO);
    glGenBuffers(1, &waterVBO);
    glBindVertexArray(waterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // position attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // colour attribute
    waterSize = points.size() / 3;
	
    beginTerrrain();
}

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

int researchStatusText;
void startResearchText(){
	int dataPoints = getIntFile(WorldGeneration.worldLinesPath, WorldGeneration.currentAreaPrefix + "DataPoints");

	researchStatusText = createText();
	allTexts[researchStatusText].centered = true;
	allTexts[researchStatusText].fontCharacters = getFont("assets/fonts/zekton.ttf", display_x / 30);
	allTexts[researchStatusText].displayedText = "Data Points: " + to_string(dataPoints);
	allTexts[researchStatusText].colour = vec3(0.0f);
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
        vec3 currentLineMountainPos = getVec3File(worldLinesPath, mountainName + "Position", l);
        if (currentLineMountainPos != vec3(-1.0f, -1.0f, -1.0f)) {
            // mountain found
            vec2 mountainScale = getVec2File(worldLinesPath, mountainName + "Scale", l + 1);
            float mountainGradient = getFloatFile(worldLinesPath, mountainName + "Gradient", l + 2);
            // add to vector
            // add values
            mountainPositions[newVectorPos(&mountainPositions)] = vec2(currentLineMountainPos.x, currentLineMountainPos.y);
            mountainScales[newVectorPos(&mountainScales)] = mountainScale;
            mountainGradients[newVectorPos(&mountainGradients)] = mountainGradient;
            currentAllMountainWaters[newVectorPos(&currentAllMountainWaters)] = (int) currentLineMountainPos.z;
        }
    }
    // assign to global
    currentAllMountainPositions = mountainPositions;
    int mCount = mountainPositions.size();
	for (int i = 0; i < mCount; i++) {
		currentAllMountainScales[newVectorPos(&currentAllMountainScales)] = vec3(mountainScales[i], mountainGradients[i]);
	}
    // start triangles
    int index = -1;
    // start triangles
    for (vec2 mPos : mountainPositions) {
        index++;
        vec2 sca = mountainScales[index];
        float gra = mountainGradients[index];
        bool mtn = false;
        if (gra > 0) {
            mtn = true;
        }
        // create circle radiuses
        vector<int> radiuses;
        float mainRad = (sca.x * 100.0f) * MOUNTAIN_SCALE_DEFAULT;
        for(int r = 0; r < (int) (mainRad / RADIUS_DIFFERENCE); r++){
            float rad = mainRad - r * RADIUS_DIFFERENCE;
            radiuses[newVectorPos(&radiuses)] = (int) rad;
        }
        
        int rCount = radiuses.size();
        for(int r = 0; r < rCount; r++){
            float single = 360.0f / POINT_COUNT;
            
            bool last = r + 1 == rCount;
            float nextRad = 0.0f;
            if(!last){
                nextRad = (float) radiuses[r + 1];
            }
            
            for(int p = 0; p < POINT_COUNT; p++){
                float distanceCurrent = glm::clamp((mainRad - radiuses[r]) / mainRad, -1.0f, 1.0f);
                if(gra < 0){ distanceCurrent = -distanceCurrent; }
                vec3 one = vec3(mPos.x, 0.0f, -mPos.y) +
                            vec3(cos(radians(single * p)) * radiuses[r],
                                distanceCurrent * sca.y,
                                -sin(radians(single * p)) * radiuses[r]);
                vec3 two = vec3(mPos.x, 0.0f, -mPos.y) +
                            vec3(cos(radians(single * (p + 1))) * radiuses[r],
                                distanceCurrent * sca.y,
                                 -sin(radians(single * (p + 1))) * radiuses[r]);
                
                vec3 three, four;
                if(last){
                    three = vec3(mPos.x, sca.y, -mPos.y);
                    four = vec3(mPos.x, sca.y, -mPos.y);
                    if(gra < 0){
                        three.y = -three.y;
                        four.y = -four.y;
                    }
                }
                if(!last){
                    float distanceNext = glm::clamp((mainRad - nextRad) / mainRad, -1.0f, 1.0f);
                    if(gra < 0){ distanceNext = -distanceNext; }
                    three = vec3(mPos.x, 0.0f, -mPos.y) +
                                vec3(cos(radians(single * p)) * nextRad,
                                     distanceNext * sca.y,
                                     -sin(radians(single * p)) * nextRad);
                    four = vec3(mPos.x, 0.0f, -mPos.y) +
                                vec3(cos(radians(single * (p + 1))) * nextRad,
                                     distanceNext * sca.y,
                                     -sin(radians(single * (p + 1))) * nextRad);
                }
                
                if(radiuses[r] == floor(mainRad)){
                    one.y = 0.0f;
                    two.y = 0.0f;
                }
                
                vector<vec3> colours = colourVector(2, currentAreaColour);
                
                triangle triOne;
                triOne.allPoints = {one, two, three};
                triOne.colour = colours[0];
                if(radiuses[r] == floor(mainRad)){
                    triOne.edgeVectorIndexes = {0, 1};
                }
                
                triangle triTwo;
                triTwo.allPoints = {four, two, three};
                triTwo.colour = colours[1];
                if(radiuses[r] == floor(mainRad)){
                    triTwo.edgeVectorIndexes = {1};
                }
                
                
                mountainTriangles[newVectorPos(&mountainTriangles)] = triOne;
                mountainTriangles[newVectorPos(&mountainTriangles)] = triTwo;
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
        setShaderFloat(terrainShader, "alpha", 1.0f);
		setShaderInt(terrainShader, "water", 0);
        
        setShaderVecThree(terrainShader, "multiplyColour", vec3(1.0f));
        if(cameraThirdPos.y <= 0.05f){
			int index = (int) terrainColliders(cameraThirdPos, 0.0f).w;
			index = index - 1;
			if (index > -1) {
				setShaderInt(terrainShader, "cameraInWater", currentAllMountainWaters[index]);
			}
			setShaderVecThree(terrainShader, "waterMultiplyColour", waterMultiplyColour);
        }
        
        glBindVertexArray(terrainVAO);
		glDrawArrays(GL_TRIANGLES, 0, total * 3);

		vector<vec3> externalPositions;
		vec3 centerChunkPos = vec3(
			(glm::floor(mainPlayer.position.x / currentAreaScale.x) * currentAreaScale.x) + (currentAreaScale.x / 2.0f),
			0.0f,
			(glm::floor(mainPlayer.position.z / currentAreaScale.y) * currentAreaScale.y) + (currentAreaScale.y / 2.0f)
		);

		vec3 mainChunkPos = vec3(
			currentAreaScale.x / 2.0f,
			0.0f,
			currentAreaScale.y / -2.0f
		);

		for (int x = -EXTERNAL_TERRAIN_CHUNKS_HALF; x <= EXTERNAL_TERRAIN_CHUNKS_HALF; x++) {
			for (int y = -EXTERNAL_TERRAIN_CHUNKS_HALF; y <= EXTERNAL_TERRAIN_CHUNKS_HALF; y++) {
				vec3 newPosition = centerChunkPos;
				newPosition.x += currentAreaScale.x * (float) x;
				newPosition.z += currentAreaScale.y * (float) y;

				// check not inside original flat terrain
				float distanceLowest = glm::distance(mainChunkPos + vec3(currentAreaScale.x, 0.0f, currentAreaScale.y), mainChunkPos);
				if (glm::distance(newPosition, mainChunkPos) < distanceLowest) {
					continue;
				}
				if (glm::distance(newPosition, mainChunkPos) == distanceLowest) {
					if (newPosition.x != mainChunkPos.x) {
						continue;
					}
					if (newPosition.z != mainChunkPos.z) {
						continue;
					}
				}

				externalPositions[newVectorPos(&externalPositions)] = newPosition;
			}
		}

		glBindVertexArray(externalVAO);
		int posCount = externalPositions.size();
		for(int p = 0; p < posCount; p++){
			setMat4(terrainShader, "model", modelMatrix(externalPositions[p]));
			glDrawArrays(GL_TRIANGLES, 0, externalSize);
		}
    }
}

void worldGeneration::worldGenLast(){
    glUseProgram(terrainShader);
    
    setMat4(terrainShader, "model", modelMatrix());
    setMat4(terrainShader, "projection", projectionMatrix());
    setMat4(terrainShader, "view", viewMatrix());
    
    setShaderVecThree(terrainShader, "lightPos", lightPos);
    setShaderFloat(terrainShader, "lightIntensity", lightIntensity);
    setShaderFloat(terrainShader, "lightRadius", lightRadius);
    setShaderInt(terrainShader, "useLight", 1);
    setShaderFloat(terrainShader, "lowestLight", lowestLight);
    setShaderFloat(terrainShader, "alpha", 0.4f);

	setShaderFloat(terrainShader, "areaScaleX", currentAreaScale.x + 20.0f);
	setShaderFloat(terrainShader, "waveWidth", waveWidth);
	setShaderFloat(terrainShader, "waveHeight", waveHeight);
	setShaderFloat(terrainShader, "wavePercentage", wavePercentage);
	setShaderInt(terrainShader, "water", 1);
    
    glBindVertexArray(waterVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3 * waterSize);
}

void worldGeneration::waveBegin(){
	if (currentArea == PLANET_WORLD) {
		waveWidth = EARTH_WAVE_WIDTH;
		waveHeight = EARTH_WAVE_HEIGHT;
		waveSpeed = EARTH_WAVE_SPEED;
		waveDelayMax = EARTH_WAVE_DELAY;
	}
}

void worldGeneration::waveMainloop(){
	wavePercentage += deltaTime * waveSpeed;
	waveDelayCurrent -= deltaTime;
	if (wavePercentage > 100.0f && !setWaveDelay) {
		waveDelayCurrent = waveDelayMax;
		setWaveDelay = true;
	}
	if (setWaveDelay && waveDelayCurrent < 0.0f) {
		wavePercentage = -20.0f;
		setWaveDelay = false;
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
	beginExternalFlat();
    currentTime = getFloatFile(worldLinesPath, "currentTime");
    mainPlayer.health = getFloatFile(WorldGeneration.worldLinesPath, "playerHealth");
    mainPlayer.position = getVec3File(WorldGeneration.worldLinesPath, "playerPos");
	waveBegin();
	startResearchText();
}

void worldGeneration::mainloop() {
    if (!active) { return; }
    renderTerrain();
    renderAreaLimits();
    daynightCycle();
	waveMainloop();

	allTexts[researchStatusText].position.x = (float) allButtons[ammoIcon].minX - (float) allButtons[healthIcon].minX;
	allTexts[researchStatusText].position.y = display_y - ((allButtons[ammoIcon].maxY + allButtons[ammoIcon].minY) / 2.0f) + (display_y / 10);
}

void worldGeneration::beginAreaLimits() {
    // generate points
    vector<float> points;
    float triangleSize = 0.1f;
    float maxHeight = 30.0f;
    // red lines
    for (int w = 0; w < 2; w++) {
        float z[] = { 0.0f, -currentAreaScale.y };
        float yLast = terrainColliders(vec3(0.0f, 0.0f, z[w]), 0.135f).y;
        for (float x = -triangleSize; x < currentAreaScale.x; x += triangleSize) {
            vec4 terrain = terrainColliders(vec3(x + triangleSize, 0.0f, z[w]), 0.135f);
            float yNew = terrain.y;
            
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
