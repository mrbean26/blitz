#include "player.h"
#include "shader.h"
#include "display.h"
#include "worldGeneration.h"
#include "backend.h"
#include "frontend.h"
#include "structures.h"
#include "weapons.h"
#include "monsters.h"

#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <iostream>
#include <vector>
using namespace std;

vector<vec3> colourVector(int size, vec3 colour, float multiplier) {
	vector<vec3> newVector;
	for (int i = 0; i < size; i++) {
		newVector[newVectorPos(&newVector)] = colour + colourDifference(multiplier);
	}
	return newVector;
}

GLuint playerShader;
vector<string> inputLines;
void player::begin(){
	startCharacterVertices();
	playerView = true;
	inputLines = readLines("assets/saves/inputs.save");
	startPauseUI();
}

void player::mainloop(){
	if (!WorldGeneration.startedBegin) { return; }
	renderPlayer();
	movement();
	cameraMovement();
	collisions();
	pauseUIInteraction();
	shoot();
	reload();
}

int continueButton, exitButton;
bool paused = false;
void startPauseUI() {
	continueButton = createButton();
	allButtons[continueButton].texture = loadTexture("assets/images/continueButton.png");
	allButtons[continueButton].scale = vec2(1.2f, 0.45f);
	allButtons[continueButton].position = vec3(0.0f, 1.2f, 0.0f);
	allButtons[continueButton].active = false;
	exitButton = createButton();
	allButtons[exitButton].texture = loadTexture("assets/images/exitButton.png");
	allButtons[exitButton].scale = vec2(1.2f, 0.45f);
	allButtons[exitButton].position = vec3(0.0f, -1.2f, 0.0f);
	allButtons[exitButton].active = false;
}

void exitToMenus() {
	// exit to main menu
			// world generation
	glDeleteVertexArrays(1, &WorldGeneration.terrainVAO);
	glDeleteVertexArrays(1, &WorldGeneration.areaLimitVAO);
	glDeleteBuffers(1, &WorldGeneration.terrainVBO);
	glDeleteBuffers(1, &WorldGeneration.areaLimitVBO);
	WorldGeneration.areaLimitCount = 0;
	// player
	mainPlayer.deleteMemory();
	// bools
	mainPlayer.active = false;
	allButtons.clear();
	allTexts.clear();
	StartScreen.active = true;
	allButtons = previousAllButtons;
	allTexts = previousAllTexts;
	loading = false;
	mainPlayer.canMove = true;
	paused = false;
	// worldGeneration.h
	currentAllMountainPositions.clear();
	currentAllMountainScales.clear();
	flatTerrainTriangles.clear();
	mountainTriangles.clear();
	flatXPoints.clear();
	flatZPoints.clear();
	WorldGeneration.startedBegin = false;
	WorldGeneration.active = false;
	// write new buildings to file
	int lineCount = newBuildingLines.size();
	vector<string> currentAllLines = readLines(WorldGeneration.worldLinesPath);
	if (lineCount > 0) {
		currentAllLines.insert(currentAllLines.end(), newBuildingLines.begin(), newBuildingLines.end());
	}
	// time values & weapon values
	string newValue = to_string(currentWeapons[0]) + " " + to_string(currentWeapons[1]) + " " +
		to_string(currentWeapons[2]) + " " + to_string(currentWeapons[3]);
	currentAllLines = rewriteLine(currentAllLines, "currentWeapons", newValue);
	currentAllLines = rewriteLine(currentAllLines, "currentTime", to_string(currentTime));
	// monster values
	auto iterator = currentAllLines.begin();
	while (iterator != currentAllLines.end()) {
		if (contains(*iterator, WorldGeneration.currentAreaPrefix + "Monster")) { // delete values
			iterator = currentAllLines.erase(iterator);
		}
		else {
			++iterator;
		}
	}
	// write monsters
	int mCount = allMonsters.size();
	for (int m = 0; m < mCount; m++) {
		string newLine = WorldGeneration.currentAreaPrefix + "Monster ";
		newLine += to_string(allMonsters[m].position.x) + " ";
		newLine += to_string(allMonsters[m].position.y) + " ";
		newLine += to_string(allMonsters[m].position.z) + " ";
		newLine += to_string(allMonsters[m].monsterName);

		currentAllLines[newVectorPos(&currentAllLines)] = newLine;
	}
	allMonsters.clear();
	// write
	writeLines(WorldGeneration.worldLinesPath, currentAllLines);
	// structures.h
	mountainLimits.clear();
	allMiniBuildings.clear();
	newBuildingLines.clear();
}

void pauseUIInteraction() {
	bool changePaused = false;
	if (paused) {
		if (allButtons[continueButton].clickUp) {
			changePaused = true;
		}
		if (allButtons[exitButton].clickUp) {
			exitToMenus();
			return;
		}
	}
	if ((checkKeyDown(GLFW_KEY_ESCAPE) && !benchInUse) || changePaused) {
		if (!paused) {
			paused = true;
			allButtons[exitButton].active = true;
			allButtons[continueButton].active = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			mainPlayer.canMove = false;
			return;
		}
		if (paused) {
			paused = false;
			allButtons[exitButton].active = false;
			allButtons[continueButton].active = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			mainPlayer.canMove = true;
		}
	}
}

float startedLowestY = -999.0f;
void player::deleteMemory(){
	glDeleteVertexArrays(1, &headVAO);
	glDeleteVertexArrays(1, &armVAO);
	glDeleteVertexArrays(1, &torsoVAO);
	glDeleteVertexArrays(1, &legVAO);

	glDeleteBuffers(1, &headVBO);
	glDeleteBuffers(1, &armVBO);
	glDeleteBuffers(1, &torsoVBO);
	glDeleteBuffers(1, &legVBO);

	position = vec3(0.0f);
}

bool lastOnBench = false;
float lastPitch;
void player::movement(){
	if (!canMove) { return; }
	int forwardKey = stoi(inputLines[0]);
	int leftKey = stoi(inputLines[1]);
	int backKey = stoi(inputLines[2]);
	int rightKey = stoi(inputLines[3]);
	
	int sprintKey = stoi(inputLines[7]);
	int jumpKey = stoi(inputLines[9]);
	int aimButton = stoi(inputLines[4]);

	float walkSpeed = 8.0f;
	float runSpeed = 12.0f;
	float backwardSpeed = -0.75f;

	float jumpSpeed = 12.0f;
	float jumpHeight = 8.0f;

	movingMultiplier = 0.0f;
	velocity = vec3(0.0f);
	if (checkKey(backKey) && !checkKey(forwardKey)) {
		movingMultiplier = 25.0f;
		velocity = vec3(backwardSpeed);
	}
	if (checkKey(forwardKey) && !checkKey(backKey)) {
		velocity = vec3(walkSpeed);
		movingMultiplier = 30.0f;
		if (checkKey(sprintKey)) {
			movingMultiplier = 50.0f;
			velocity = vec3(runSpeed);
		}
	}
	if (checkKeyDown(jumpKey) && !jumping) {
		jumping = true;
		startedLowestY = lowestY;
		jumpVelocity = jumpHeight;
	}
	if (jumping) {
		jumpVelocity -= (float) deltaTime * jumpSpeed;
		if (position.y < lowestY + 0.02f && jumpVelocity < 0) {
			jumpVelocity = 0.0f;
			jumping = false;
		}
		/* I don't think this affects jumping mechanics 
		if (lowestY > startedLowestY) {
			if (position.y < lowestY + 0.02f) {
				jumpVelocity = 0.0f;
				jumping = false;
			}
			
		}
		if (lowestY < startedLowestY) {
			if (position.y < lowestY + 0.02f) {
				jumpVelocity = 0.0f;
				jumping = false;
			}
		}
		*/
	}
	velocity.y = jumpVelocity;
	// rotations
	float spinMultiplier = 140.0f;
	if (checkKey(rightKey)) { 
		rotation.y -= (float) deltaTime * spinMultiplier;
	}
	if (checkKey(leftKey)) {
		rotation.y += (float) deltaTime * spinMultiplier;
	}
	runAnimation(movingMultiplier);
	if (checkKey(aimButton)) {
		if (!aiming) {
			armRotation.x = 90.0f;
			armRotationTwo.x = 90.0f;
			armRotationTwo.z = -45.0f;
			lastPitch = playerPitch;
			equippingReloading = true;
			reloadSpeed = allWeapons[currentWeapon].equipTime;
		}
		aiming = true;
		if (!equippingReloading) {
			armRotationTwo.z = -45.0f;
		}
		armRotation.x += playerPitch - lastPitch;
		armRotationTwo.x += playerPitch - lastPitch;
		armPositionTwo = vec3(0.55f, -0.425f, 0.0f);
		aimingView = true;

		lastPitch = playerPitch;
	}
	if (!checkKey(aimButton)) {
		if (aiming) {
			armRotation.x = 0.0f;
			armPositionTwo = vec3(0.7f, -0.55f, 0.0f);
			armRotationTwo = vec3(0.0f);
			aimingView = false;
		}
		aiming = false;
	}

	if (equippingReloading) {
		equipReloadAnimation(allWeapons[currentWeapon].equipTime);
	}

	// physical movement

	float xGap = -sin(radians(rotation.y));
	float zGap = -cos(radians(rotation.y));
	vec3 rotationMovement = vec3(xGap, 1.0f, zGap) * vec3((float) deltaTime);
	velocity = velocity * rotationMovement;
	position += velocity;
}

float distanceIntoCircle(float radius, vec2 circlePos, vec2 playerPos) {
	// squared values
	float radSquared = pow(radius, 2);
	float xSquared = pow(playerPos.x - circlePos.x, 2);
	float ySquared = pow(playerPos.y - circlePos.y, 2);
	// return distance
	return xSquared + ySquared;
}

int insideBuildingIndex = -1;
float lowestY = -999.0f;
void player::collisions(){
	// mountains and craters
	bool inMountain = false;
	bool cameraInMountain = false;
	int mCount = currentAllMountainPositions.size();
	float highestPoint = -999.0f;
	float highestPointCamera = -999.0f;
	for (int i = 0; i < mCount; i++) {
		vec2 currentMPos = currentAllMountainPositions[i]; currentMPos.y *= -1.0f;
		float currentRad = (currentAllMountainScales[i].x * 100.0f) * 0.025f;
		vec2 floorPoint = vec2(position.x, position.z);
		bool crater = false;
		
		vec3 mountainThree = vec3(currentMPos.x, 0.0f, currentMPos.y);
		vec3 playerThree = vec3(position.x, 0.0f, position.z);
		vec3 cameraThree = vec3(cameraThirdPos.x, 0.0f, cameraThirdPos.z);
		if (currentAllMountainScales[i].z < 0) { crater = true; }

		float distance = glm::distance(mountainThree, playerThree);
		float distanceCamera = glm::distance(mountainThree, cameraThree);
		distance = currentRad - distance;
		distanceCamera = currentRad - distanceCamera;

		if (distance > 0) {
			inMountain = true;
			distance = distance / currentRad;
			distance = clamp(distance, -1.0f, 1.0f);
			float pointY = distance*currentAllMountainScales[i].y;
			if (crater) { pointY = -pointY; }

			pointY += 2.3f; // feet on floor
			if (pointY > highestPoint) { highestPoint = pointY; } // assign
		}
		if (distanceCamera > 0) {
			cameraInMountain = true;
			distanceCamera = distanceCamera / currentRad;
			distanceCamera = clamp(distanceCamera, -1.0f, 1.0f);
			float pointY = distanceCamera * currentAllMountainScales[i].y;
			if (crater) { pointY = -pointY; }
			if (pointY > highestPointCamera) { highestPointCamera = pointY; }
		}
	}
	if (position.y < highestPoint && jumping) { position.y = highestPoint; }
	if (!jumping) { position.y = highestPoint; }
	lowestY = highestPoint;
	lowestCameraY = highestPointCamera + 0.5f;
	// outside of buildings
	int buildingCount = allColourBuildings.size();
	float playerDifference = 1.0f;
	float playerYDifference = 0.5f;
	bool onBenchCurrent = false;
	insideBuildingIndex = -1;
	for (int b = 0; b < buildingCount; b++) {
		buildingColour current = allColourBuildings[b];
		vec3 pos = current.position;
		vec3 sca = current.scale;
		vec3 rot = current.rotation;
		rot.y = rot.y - (floor((rot.y / 360.0f)) * 360.0f);
		if (current.buildingType == 0) { // normal house
			// check if inside for camera collisions
			if (position.x >= pos.x - 4.0f * sca.x && position.x <= pos.x + 4.0f * sca.x) {
				if (position.z >= pos.z - 4.0f * sca.z && position.z <= pos.z + 4.0f * sca.z) {
					insideBuildingIndex = b;
				}
			}
										 // outside walls
			if (position.x >= pos.x - 4.0f * sca.x && position.x <= pos.x + 4.0f * sca.x) {
				if (position.z >= pos.z + 4.0f * sca.z && position.z <= playerDifference + pos.z + 4.0f * sca.z) {
					// 270 deg
					if (radians(90.0f) == radians(rot.y)) {
						if (position.x >= pos.x + 0.5f * sca.x) {
							position.z = playerDifference + pos.z + 4.0f * sca.z;
						}
					}
					else {
						position.z = playerDifference + pos.z + 4.0f * sca.z;
					}
				}
				if (position.z <= pos.z - 4.0f * sca.z && position.z >= -playerDifference + pos.z - 4.0f * sca.z) {
					// 90 deg
					if (radians(270.0f) == radians(rot.y)) {
						if (position.x <= pos.x - 0.5f * sca.x) {
							position.z = -playerDifference + pos.z - 4.0f * sca.z;
						}
					}
					else {
						position.z = -playerDifference + pos.z - 4.0f * sca.z;
					}
				}
			}
			if (position.z >= pos.z - 4.0f * sca.z && position.z <= pos.z + 4.0f * sca.z) {
				if (position.x >= pos.x + 4.0f * sca.x && position.x <= playerDifference + pos.x + 4.0f * sca.x) {
					// 180 deg
					if (radians(180.0f) == radians(rot.y)) {
						if (position.z <= pos.z - 0.5f * sca.z) {
							position.x = playerDifference + pos.x + 4.0f * sca.x;
						}
					}
					else {
						position.x = playerDifference + pos.x + 4.0f * sca.x;
					}
				}
				if (position.x <= pos.x - 4.0f * sca.x && position.x >= -playerDifference + pos.x - 4.0f * sca.x) {
					// 0 deg
					if (radians(0.0f) == radians(rot.y)) {
						if (position.z >= pos.z + 0.5f * sca.z) {
							position.x = -playerDifference + pos.x - 4.0f * sca.x;
						}
					}
					else {
						position.x = -playerDifference + pos.x - 4.0f * sca.x;
					}
				}
			}
			// inside walls
			if (position.x >= pos.x - 4.0f * sca.x && position.x <= pos.x + 4.0f * sca.x) {
				if (position.z <= pos.z + 4.0f * sca.z && position.z >= -playerDifference + pos.z + 4.0f * sca.z) {
					// 270 deg
					if (radians(90.0f) == radians(rot.y)) {
						if (position.x >= pos.x + 0.5f * sca.x) {
							position.z = -playerDifference + pos.z + 4.0f * sca.z;
						}
					}
					else {
						position.z = -playerDifference + pos.z + 4.0f * sca.z;
					}
				}
				if (position.z >= pos.z - 4.0f * sca.z && position.z <= playerDifference + pos.z - 4.0f * sca.z) {
					// 90 deg
					if (radians(270.0f) == radians(rot.y)) {
						if (position.x <= pos.x - 0.5f * sca.x) {
							position.z = playerDifference + pos.z - 4.0f * sca.z;
						}
					}
					else {
						position.z = playerDifference + pos.z - 4.0f * sca.z;
					}
				}
			}
			if (position.z >= pos.z - 4.0f * sca.z && position.z <= pos.z + 4.0f * sca.z) {
				if (position.x <= pos.x + 4.0f * sca.x && position.x >= -playerDifference + pos.x + 4.0f * sca.x) {
					// 180 deg
					if (radians(180.0f) == radians(rot.y)) {
						if (position.z <= pos.z - 0.5f * sca.z) {
							position.x = -playerDifference + pos.x + 4.0f * sca.x;
						}
					}
					else {
						position.x = -playerDifference + pos.x + 4.0f * sca.x;
					}
				}
				if (position.x >= pos.x - 4.0f * sca.x && position.x <= playerDifference + pos.x - 4.0f * sca.x) {
					// 0 deg
					if (radians(0.0f) == radians(rot.y)) {
						if (position.z >= pos.z + 0.5f * sca.z) {
							position.x = playerDifference + pos.x - 4.0f * sca.x;
						}
					}
					else {
						position.x = playerDifference + pos.x - 4.0f * sca.x;
					}
				}
			}
		}
		if (current.buildingType == 1) { // pointy house
			// outside walls
			if (position.x >= pos.x - 4.0f * sca.x && position.x <= pos.x + 4.0f * sca.x) {
				if (position.z >= pos.z + 4.0f * sca.z && position.z <= playerDifference + pos.z + 4.0f * sca.z) {
					// 270 deg
					if (radians(90.0f) == radians(rot.y)) {
						if (position.x >= pos.x - playerDifference + 2.0f * sca.x || position.x <= pos.x + playerDifference - 2.0f * sca.x) {
							position.z = playerDifference + pos.z + 4.0f * sca.z;
						}
					}
					else {
						position.z = playerDifference + pos.z + 4.0f * sca.z;
					}
				}
				if (position.z <= pos.z - 4.0f * sca.z && position.z >= -playerDifference + pos.z - 4.0f * sca.z) {
					// 90 deg
					if (radians(270.0f) == radians(rot.y)) {
						if (position.x <= pos.x + playerDifference - 2.0f * sca.x || position.x >= pos.x - playerDifference + 2.0f * sca.x) {
							position.z = -playerDifference + pos.z - 4.0f * sca.z;
						}
					}
					else {
						position.z = -playerDifference + pos.z - 4.0f * sca.z;
					}
				}
			}
			if (position.z >= pos.z - 4.0f * sca.z && position.z <= pos.z + 4.0f * sca.z) {
				if (position.x >= pos.x + 4.0f * sca.x && position.x <= playerDifference + pos.x + 4.0f * sca.x) {
					// 180 deg
					if (radians(180.0f) == radians(rot.y)) {
						if (position.z <= pos.z + playerDifference - 2.0f * sca.z || position.z >= pos.z - playerDifference + 2.0f * sca.z) {
							position.x = playerDifference + pos.x + 4.0f * sca.x;
						}
					}
					else {
						position.x = playerDifference + pos.x + 4.0f * sca.x;
					}
				}
				if (position.x <= pos.x - 4.0f * sca.x && position.x >= -playerDifference + pos.x - 4.0f * sca.x) {
					// 0 deg
					if (radians(0.0f) == radians(rot.y)) {
						if (position.z >= pos.z - playerDifference + 2.0f * sca.z || position.z <= pos.z + playerDifference - 2.0f * sca.z) {
							position.x = -playerDifference + pos.x - 4.0f * sca.x;
						}
					}
					else {
						position.x = -playerDifference + pos.x - 4.0f * sca.x;
					}
				}
			}
			if (position.x >= pos.x - 4.0f * sca.x && position.x <= pos.x + 4.0f * sca.x) {
				if (position.z <= pos.z + 4.0f * sca.z && position.z >= pos.z - 4.0f * sca.z) {
					insideBuildingIndex = b;
				}
			}
			// inside walls
			if (position.x >= pos.x - 4.0f * sca.x && position.x <= pos.x + 4.0f * sca.x) {
				if (position.z <= pos.z + 4.0f * sca.z && position.z >= -playerDifference + pos.z + 4.0f * sca.z) {
					// 270 deg
					if (radians(90.0f) == radians(rot.y)) {
						if (position.x >= pos.x - playerDifference + 2.0f * sca.x || position.x <= pos.x + playerDifference - 2.0f * sca.x) {
							position.z = -playerDifference + pos.z + 4.0f * sca.z;
						}
					}
					else {
						position.z = -playerDifference + pos.z + 4.0f * sca.z;
					}
				}
				if (position.z >= pos.z - 4.0f * sca.z && position.z <= playerDifference + pos.z - 4.0f * sca.z) {
					// 90 deg
					if (radians(270.0f) == radians(rot.y)) {
						if (position.x <= pos.x + playerDifference - 2.0f * sca.x || position.x >= pos.x - playerDifference + 2.0f * sca.x) {
							position.z = playerDifference + pos.z - 4.0f * sca.z;
						}
					}
					else {
						position.z = playerDifference + pos.z - 4.0f * sca.z;
					}
				}
			}
			if (position.z >= pos.z - 4.0f * sca.z && position.z <= pos.z + 4.0f * sca.z) {
				if (position.x <= pos.x + 4.0f * sca.x && position.x >= -playerDifference + pos.x + 4.0f * sca.x) {
					// 180 deg
					if (radians(180.0f) == radians(rot.y)) {
						if (position.z <= pos.z + playerDifference - 2.0f * sca.z || position.z >= pos.z - playerDifference + 2.0f * sca.z) {
							position.x = -playerDifference + pos.x + 4.0f * sca.x;
						}
					}
					else {
						position.x = -playerDifference + pos.x + 4.0f * sca.x;
					}
				}
				if (position.x >= pos.x - 4.0f * sca.x && position.x <= playerDifference + pos.x - 4.0f * sca.x) {
					// 0 deg
					if (radians(0.0f) == radians(rot.y)) {
						if (position.z >= pos.z - playerDifference + 2.0f * sca.z || position.z <= pos.z + playerDifference - 2.0f * sca.z) {
							position.x = playerDifference + pos.x - 4.0f * sca.x;
						}
					}
					else {
						position.x = playerDifference + pos.x - 4.0f * sca.x;
					}
				}
			}
			// ceiling colliders
			if (position.x >= pos.x - 6.0f * sca.x && position.x <= pos.x + 6.0f * sca.x) {
				if (position.z >= pos.z - 6.0f * sca.z && position.z <= pos.z + 6.0f * sca.z) {
					if (position.y + 2.3f >= playerYDifference + pos.y + 6.0f * sca.y) {
						position.y = playerYDifference + -2.3f + pos.y + 6.0f * sca.y;
					}
				}
			}
		}
		if (current.buildingType == 2) { // bench
			if (position.y - 2.3f <= pos.y + 2.25f * sca.y) {
				if (position.x >= pos.x - 0.5f * sca.x && position.x <= pos.x + 1.5f * sca.x) {
					// on top
					if (position.z >= pos.z - 2.0f * sca.z && position.z <= pos.z + 3.0f * sca.z) {
						if (position.y - 2.3f <= pos.y + 2.25f * sca.y) {
							position.y = 2.3f + pos.y + 2.25f * sca.y;
							if (jumpVelocity < 0) { jumpVelocity = 0.0f; }
							int jumpKey = stoi(inputLines[9]);
							float jumpHeight = 8.0f;
							if (checkKeyDown(jumpKey)) {
								jumpVelocity = jumpHeight;
							}
							onBenchCurrent = true;
							lastOnBench = true;
						}
					}
					// sides
					if (position.z >= pos.z + 3.0f * sca.z && position.z <= playerDifference + pos.z + 3.0f * sca.z) {
						position.z = playerDifference + pos.z + 3.0f * sca.z;
					}
					if (position.z <= pos.z - 2.0f * sca.z && position.z >= -playerDifference + pos.z - 2.0f * sca.z) {
						position.z = -playerDifference + pos.z - 2.0f * sca.z;
					}
				}
				if (position.z >= pos.z - 2.0f * sca.z && position.z <= pos.z + 3.0f * sca.z) {
					if (position.x >= pos.x + 1.5f * sca.x && position.x <= playerDifference + pos.x + 1.5f * sca.x) {
						position.x = playerDifference + pos.x + 1.5f * sca.x;
					}
					if (position.x <= pos.x - 0.5f * sca.x && position.x >= -playerDifference + pos.x - 0.5f * sca.x) {
						position.x = -playerDifference + pos.x - 0.5f * sca.x;
					}
				}
			}
			if (!onBenchCurrent && lastOnBench) {
				lastOnBench = false;
			}
		}
	}
	// flat terrain collisions
	float legPos = position.y - 2.3f;
	if (legPos < 0 && !inMountain && !jumping) { position.y += -legPos; }
	if (!inMountain) { lowestY = 2.2f; }
	if (!cameraInMountain) { lowestCameraY = 0.5f; }
	// distance outside of area scale
	position.x = clamp(position.x, 0.0f, currentPlanetScale.x);
	position.z = clamp(position.z, -currentPlanetScale.y, 0.0f);
}

vec3 cameraBuildingCollisions(vec3 original) {
	int bCount = allColourBuildings.size();
	float cameraDifference = -0.25f;
	if (insideBuildingIndex != -1) {
		buildingColour current = allColourBuildings[insideBuildingIndex];
		vec3 bPos = current.position;
		vec3 bSca = current.scale;
		int bType = current.buildingType;
		// inside a building
		if (bType == 0 || bType == 1) {
			if (original.x <= -cameraDifference + bPos.x - 4.0f * bSca.x) {
				original.x = -cameraDifference + bPos.x - 4.0f * bSca.x;
			}
			if (original.x >= cameraDifference + bPos.x + 4.0f * bSca.x) {
				original.x = cameraDifference + bPos.x + 4.0f * bSca.x;
			}
			if (original.z <= -cameraDifference + bPos.z - 4.0f * bSca.z) {
				original.z = -cameraDifference + bPos.z - 4.0f * bSca.z;
			}
			if (original.z >= cameraDifference + bPos.z + 4.0f * bSca.z) {
				original.z = cameraDifference + bPos.z + 4.0f * bSca.z;
			}
			if (original.y >= cameraDifference + bPos.y + 8.0f * bSca.y) {
				original.y = cameraDifference + bPos.y + 8.0f * bSca.y;
			}
		}
	}
	if (insideBuildingIndex == -1) {
		// for outsides of buildings
		for (int b = 0; b < bCount; b++) {
			buildingColour current = allColourBuildings[b];
			vec3 bPos = current.position;
			vec3 bSca = current.scale;
			int bType = current.buildingType;

			vec3 playerPos = mainPlayer.position;
			if (bType == 0 || bType == 1) {
				// side 1
				if (original.x >= bPos.x - 4.0f * bSca.x && original.x <= bPos.x + 4.0f * bSca.x ||
						playerPos.x >= bPos.x - 4.0f * bSca.x && playerPos.x <= bPos.x + 4.0f * bSca.x) {
					if (playerPos.z <= bPos.z - 4.0f * bSca.z) {
						if (original.z >= cameraDifference + bPos.z - 4.0f * bSca.z) {
							original.z = cameraDifference + bPos.z - 4.0f * bSca.z;
						}
					}
				}
				// side 2
				if (original.x >= bPos.x - 4.0f * bSca.x && original.x <= bPos.x + 4.0f * bSca.x ||
						playerPos.x >= bPos.x - 4.0f * bSca.x && playerPos.x <= bPos.x + 4.0f * bSca.x) {
					if (playerPos.z >= bPos.z + 4.0f * bSca.z) {
						if (original.z <= -cameraDifference + bPos.z + 4.0f + bSca.z) {
							original.z = -cameraDifference + bPos.z + 4.0f * bSca.z;
						}
					}
				}
				// side 3
				if (original.z >= bPos.z - 4.0f * bSca.z && original.z <= bPos.z + 4.0f * bSca.z ||
					playerPos.z >= bPos.z - 4.0f * bSca.z && playerPos.z <= bPos.z + 4.0f * bSca.z) {
					if (playerPos.x <= bPos.x - 4.0f * bSca.x) {
						if (original.x >= cameraDifference + bPos.x - 4.0f * bSca.x) {
							original.x = cameraDifference + bPos.x - 4.0f * bSca.x;
						}
					}
				}
				// side 4
				if (original.z >= bPos.z - 4.0f * bSca.z && original.z <= bPos.z + 4.0f * bSca.z ||
					playerPos.z >= bPos.z - 4.0f * bSca.z && playerPos.z <= bPos.z + 4.0f * bSca.z) {
					if (playerPos.x >= bPos.x + 4.0f * bSca.x) {
						if (original.x <= -cameraDifference + bPos.x + 4.0f + bSca.x) {
							original.x = -cameraDifference + bPos.x + 4.0f * bSca.x;
						}
					}
				}
			}
			if (bType == 1) {
				if ((original.x >= bPos.x - 6.0f * bSca.x && original.x <= bPos.x + 6.0f * bSca.x) ||
						(playerPos.x >= bPos.x - 6.0f * bSca.x && playerPos.x <= bPos.x + 6.0f * bSca.x)) {
					if ((original.z >= bPos.z - 6.0f * bSca.z && original.z <= bPos.z + 6.0f * bSca.z) ||
							(playerPos.z >= bPos.z - 6.0f * bSca.z && playerPos.z <= bPos.z + 6.0f * bSca.z)) {
						if (original.y >= cameraDifference + bPos.y + 6.0f * bSca.y) {
							original.y = cameraDifference + bPos.y + 6.0f * bSca.y;
						}
					}
				}
			}
			if (bType == 2) {
				if (original.y < bPos.y + 2.25f * bSca.y) {
					// side 1
					if (original.x >= bPos.x - 0.5f * bSca.x && original.x <= bPos.x + 1.5f * bSca.x ||
						playerPos.x >= bPos.x - 0.5f * bSca.x && playerPos.x <= bPos.x + 1.5f * bSca.x) {
						if (playerPos.z <= bPos.z - 2.0f * bSca.z) {
							if (original.z >= cameraDifference + bPos.z - 2.0f * bSca.z) {
								original.z = cameraDifference + bPos.z - 2.0f * bSca.z;
							}
						}
					}
					// side 2
					if (original.x >= bPos.x - 0.5f * bSca.x && original.x <= bPos.x + 1.5f * bSca.x ||
						playerPos.x >= bPos.x - 0.5f * bSca.x && playerPos.x <= bPos.x + 1.5f * bSca.x) {
						if (playerPos.z >= bPos.z + 3.0f * bSca.z) {
							if (original.z <= -cameraDifference + bPos.z + 3.0f + bSca.z) {
								original.z = -cameraDifference + bPos.z + 3.0f * bSca.z;
							}
						}
					}
					// side 3
					if (original.z >= bPos.z - 2.0f * bSca.z && original.z <= bPos.z + 3.0f * bSca.z ||
						playerPos.z >= bPos.z - 2.0f * bSca.z && playerPos.z <= bPos.z + 3.0f * bSca.z) {
						if (playerPos.x <= bPos.x - 0.5f * bSca.x) {
							if (original.x >= cameraDifference + bPos.x - 0.5f * bSca.x) {
								original.x = cameraDifference + bPos.x - 0.5f * bSca.x;
							}
						}
					}
					// side 4
					if (original.z >= bPos.z - 2.0f * bSca.z && original.z <= bPos.z + 3.0f * bSca.z ||
						playerPos.z >= bPos.z - 2.0f * bSca.z && playerPos.z <= bPos.z + 3.0f * bSca.z) {
						if (playerPos.x >= bPos.x + 1.5f * bSca.x) {
							if (original.x <= -cameraDifference + bPos.x + 1.5f + bSca.x) {
								original.x = -cameraDifference + bPos.x + 1.5f * bSca.x;
							}
						}
					}
				}
			}
		}
	}
	return original;
}

bool lastFrameMove = true;
void player::cameraMovement(){
	if (!canMove || !canMoveCamera) { 
		lastFrameMove = false;
		return; 
	}
	vec2 mouseDiffer = mouseDifferences();
	if (lastFrameMove) {
		playerYaw -= mouseDiffer.x * sensitivity;
		playerPitch += mouseDiffer.y * sensitivity;
		if (aiming) {
			rotation.y -= mouseDiffer.x * sensitivity;
		}
	}
	playerPitch = clamp(playerPitch, -80.0f, 80.0f);
	headLookAtY = position.y;
	headLookAtY = headLookAtY + 1.25f;
	lastFrameMove = true;
}

void player::renderPlayer(){
	vector<vec3> positions = { headPosition, torsoPosition, armPosition, legPosition, armPositionTwo, legPositionTwo };
	vector<vec3> rotations = { headRotation, torsoRotation, armRotation, legRotation, armRotationTwo, legRotationTwo };
	vector<vec3> scales = { headScale, torsoScale, armScale, legScale, armScaleTwo, legScaleTwo };

	vector<GLuint> vaos = { headVAO, torsoVAO, armVAO, legVAO, armVAO, legVAO };
	vector<int> vertCounts = { 48, 36, 60, 60, 60, 60 };

	vec3 legParentPos = position - vec3(0.0f, 0.775f, 0.0f);
	vec3 armParentPos = position + vec3(0.0f, 0.5f, 0.0f);
	vector<vec3> parentPositions = { position, position, armParentPos, legParentPos, armParentPos, legParentPos };
	setShaderVecThree(playerShader, "multiplyColour", vec3(1.0f));
	glUseProgram(playerShader);
	for (int i = 0; i < 6; i++) {
		
		vec3 combinedScale = scale * scales[i];

		setMat4(playerShader, "model", modelMatrix(positions[i], rotations[i], combinedScale, 
			true, parentPositions[i], rotation));
		setMat4(playerShader, "view", viewMatrix());
		setMat4(playerShader, "projection", projectionMatrix());

		setShaderVecThree(playerShader, "lightPos", lightPos);
		setShaderFloat(playerShader, "lightIntensity", lightIntensity);
		setShaderFloat(playerShader, "lightRadius", lightRadius);
		setShaderInt(playerShader, "useLight", 1);
		setShaderFloat(playerShader, "lowestLight", lowestLight);

		glBindVertexArray(vaos[i]);
		glDrawArrays(GL_TRIANGLES, 0, vertCounts[i]);
	}
	// laser
	armRotation.x = clamp(armRotation.x, -1000.0f, 105.0f);
	armRotationTwo.x = clamp(armRotationTwo.x, -1000.0f, 105.0f);
	if (aiming) {
		setShaderVecThree(playerShader, "multiplyColour", laserColour);
		setShaderInt(playerShader, "useLight", 0);
		glBindVertexArray(laserVAO);
		setShaderFloat(playerShader, "alpha", 0.5f);
		setMat4(playerShader, "model", modelMatrix(vec3(2.0f, 0.0f, -0.65f), vec3(armRotation.x - 90.0f, 90.0f, 0.0f), vec3(250.0f, 0.1f, 0.1f),
			true, vec3(position.x, position.y + 0.5f, position.z), rotation));
		glDrawArrays(GL_TRIANGLES, 0, 24);
		glLinkProgram(playerShader);
		// weapon
		if (currentWeapon == 0) {
			allWeapons[currentWeapon].render(modelMatrix(vec3(1.2f, 0.0f, -0.7f), 
				vec3(armRotation.x - 90.0f, 90.0f, 0.0f), vec3(0.4f), true, 
					vec3(position.x, position.y + 0.25f, position.z), rotation));
		}
		if (currentWeapon == 1) {
			allWeapons[currentWeapon].render(modelMatrix(vec3(0.6f, 0.0f, 1.2f),
				vec3(armRotation.x - 90.0f, 180.0f, 0.0f), vec3(0.8f), true,
				vec3(position.x, position.y + 0.5f, position.z), rotation));
		}
	}
}

void player::startCharacterVertices(){
	startHead();
	startArm();
	startTorso();
	startLeg();
	startLaserForBullets();
}

void player::startLaserForBullets() {
	float laserVertices[] = {
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		// top & bottom
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	// start ints
	glCreateVertexArrays(1, &laserVAO);
	glCreateBuffers(1, &laserVBO);
	glBindVertexArray(laserVAO);
	glBindBuffer(GL_ARRAY_BUFFER, laserVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(laserVertices), laserVertices, GL_STATIC_DRAW);
	// pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void player::startHead(){
	vector<vec3> headColours = colourVector(16, vec3(0.26f, 0.53, 0.96));
	float headVertices[] = {
		// top section of head
		0.0f, 0.75f, 0.0f, headColours[0].x, headColours[0].y, headColours[0].z,
		1.0f, 0.25f, 0.0f, headColours[0].x, headColours[0].y, headColours[0].z,
		0.0f, 0.25f, 1.0f, headColours[0].x, headColours[0].y, headColours[0].z,

		0.0f, 0.75f, 0.0f, headColours[1].x, headColours[1].y, headColours[1].z,
		1.0f, 0.25f, 0.0f, headColours[1].x, headColours[1].y, headColours[1].z,
		0.0f, 0.25f, -1.0f, headColours[1].x, headColours[1].y, headColours[1].z,

		0.0f, 0.75f, 0.0f, headColours[2].x, headColours[2].y, headColours[2].z,
		-1.0f, 0.25f, 0.0f, headColours[2].x, headColours[2].y, headColours[2].z,
		0.0f, 0.25f, 1.0f, headColours[2].x, headColours[2].y, headColours[2].z,

		0.0f, 0.75f, 0.0f, headColours[3].x, headColours[3].y, headColours[3].z,
		-1.0f, 0.25f, 0.0f, headColours[3].x, headColours[3].y, headColours[3].z,
		0.0f, 0.25f, -1.0f, headColours[3].x, headColours[3].y, headColours[3].z,

		//middle section of head
		0.0f, 0.25f, -1.0f, headColours[4].x, headColours[4].y, headColours[4].z,
		-1.0f, 0.25f, 0.0f, headColours[4].x, headColours[4].y, headColours[4].z,
		-1.0f, -0.25f, 0.0f, headColours[4].x, headColours[4].y, headColours[4].z,

		0.0f, -0.25f, -1.0f,  headColours[5].x, headColours[5].y, headColours[5].z,
		0.0f, 0.25f, -1.0f, headColours[5].x, headColours[5].y, headColours[5].z,
		-1.0f, -0.25f, 0.0f, headColours[5].x, headColours[5].y, headColours[5].z,

		0.0f, 0.25f, 1.0f, headColours[6].x, headColours[6].y, headColours[6].z,
		-1.0f, 0.25f, 0.0f, headColours[6].x, headColours[6].y, headColours[6].z,
		-1.0f, -0.25f, 0.0f, headColours[6].x, headColours[6].y, headColours[6].z,

		0.0f, -0.25f, 1.0f, headColours[7].x, headColours[7].y, headColours[7].z,
		0.0f, 0.25f, 1.0f, headColours[7].x, headColours[7].y, headColours[7].z,
		-1.0f, -0.25f, 0.0f, headColours[7].x, headColours[7].y, headColours[7].z,

		0.0f, 0.25f, -1.0f, headColours[8].x, headColours[8].y, headColours[8].z,
		1.0f, 0.25f, 0.0f, headColours[8].x, headColours[8].y, headColours[8].z,
		1.0f, -0.25f, 0.0f, headColours[8].x, headColours[8].y, headColours[8].z,

		0.0f, -0.25f, -1.0f, headColours[9].x, headColours[9].y, headColours[9].z,
		0.0f, 0.25f, -1.0f, headColours[9].x, headColours[9].y, headColours[9].z,
		1.0f, -0.25f, 0.0f, headColours[9].x, headColours[9].y, headColours[9].z,

		0.0f, 0.25f, 1.0f, headColours[10].x, headColours[10].y, headColours[10].z,
		1.0f, 0.25f, 0.0f, headColours[10].x, headColours[10].y, headColours[10].z,
		1.0f, -0.25f, 0.0f, headColours[10].x, headColours[10].y, headColours[10].z,

		0.0f, -0.25f, 1.0f, headColours[11].x, headColours[11].y, headColours[11].z,
		0.0f, 0.25f, 1.0f, headColours[11].x, headColours[11].y, headColours[11].z,
		1.0f, -0.25f, 0.0f, headColours[11].x, headColours[11].y, headColours[11].z,

		// bottom section of head
		0.0f, -0.25f, 0.0f, headColours[12].x, headColours[12].y, headColours[12].z,
		1.0f, -0.25f, 0.0f, headColours[12].x, headColours[12].y, headColours[12].z,
		0.0f, -0.25f, 1.0f, headColours[12].x, headColours[12].y, headColours[12].z,

		0.0f, -0.25f, 0.0f, headColours[13].x, headColours[13].y, headColours[13].z,
		1.0f, -0.25f, 0.0f, headColours[13].x, headColours[13].y, headColours[13].z,
		0.0f, -0.25f, -1.0f, headColours[13].x, headColours[13].y, headColours[13].z,

		0.0f, -0.25f, 0.0f, headColours[14].x, headColours[14].y, headColours[14].z,
		-1.0f, -0.25f, 0.0f, headColours[14].x, headColours[14].y, headColours[14].z,
		0.0f, -0.25f, 1.0f, headColours[14].x, headColours[14].y, headColours[14].z,

		0.0f, -0.25f, 0.0f, headColours[15].x, headColours[15].y, headColours[15].z,
		-1.0f, -0.25f, 0.0f, headColours[15].x, headColours[15].y, headColours[15].z,
		0.0f, -0.25f, -1.0f, headColours[15].x, headColours[15].y, headColours[15].z,
	};
	// start ints
	glCreateVertexArrays(1, &headVAO);
	glCreateBuffers(1, &headVBO);
	glBindVertexArray(headVAO);
	glBindBuffer(GL_ARRAY_BUFFER, headVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(headVertices), headVertices, GL_STATIC_DRAW);
	// pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void player::startArm() {
	vector<vec3> armColour = colourVector(10, vec3(0.96f, 0.26f, 0.26f));
	vector<vec3> handColour = colourVector(10, vec3(0.26f, 0.53f, 0.96f));
	float armVertices[] = {
		0.3f, 1.5f, 0.3f, armColour[0].x, armColour[0].y, armColour[0].z,
		-0.3f, 1.5f, 0.3f, armColour[0].x, armColour[0].y, armColour[0].z,
		-0.3f, -1.5f, 0.3f, armColour[0].x, armColour[0].y, armColour[0].z,

		-0.3f, -1.5f, 0.3f, armColour[1].x, armColour[1].y, armColour[1].z,
		0.3f, -1.5f, 0.3f, armColour[1].x, armColour[1].y, armColour[1].z,
		0.3f, 1.5f, 0.3f, armColour[1].x, armColour[1].y, armColour[1].z,

		0.3f, 1.5f, -0.3f, armColour[2].x, armColour[2].y, armColour[2].z,
		-0.3f, 1.5f, -0.3f, armColour[2].x, armColour[2].y, armColour[2].z,
		-0.3f, -1.5f, -0.3f, armColour[2].x, armColour[2].y, armColour[2].z,

		-0.3f, -1.5f, -0.3f, armColour[3].x, armColour[3].y, armColour[3].z,
		0.3f, -1.5f, -0.3f, armColour[3].x, armColour[3].y, armColour[3].z,
		0.3f, 1.5f, -0.3f, armColour[3].x, armColour[3].y, armColour[3].z,

		// z
		0.3f, 1.5f, 0.3f, armColour[4].x, armColour[4].y, armColour[4].z,
		0.3f, 1.5f, -0.3f, armColour[4].x, armColour[4].y, armColour[4].z,
		0.3f, -1.5f, -0.3f, armColour[4].x, armColour[4].y, armColour[4].z,

		0.3f, -1.5f, -0.3f, armColour[5].x, armColour[5].y, armColour[5].z,
		0.3f, -1.5f, 0.3f, armColour[5].x, armColour[5].y, armColour[5].z,
		0.3f, 1.5f, 0.3f, armColour[5].x, armColour[5].y, armColour[5].z,

		-0.3f, 1.5f, 0.3f, armColour[6].x, armColour[6].y, armColour[6].z,
		-0.3f, 1.5f, -0.3f, armColour[6].x, armColour[6].y, armColour[6].z,
		-0.3f, -1.5f, -0.3f, armColour[6].x, armColour[6].y, armColour[6].z,

		-0.3f, -1.5f, -0.3f, armColour[7].x, armColour[7].y, armColour[7].z,
		-0.3f, -1.5f, 0.3f, armColour[7].x, armColour[7].y, armColour[7].z,
		-0.3f, 1.5f, 0.3f, armColour[7].x, armColour[7].y, armColour[7].z,

		// top
		-0.3f, 1.5f, -0.3f, armColour[8].x, armColour[8].y, armColour[8].z,
		-0.3f, 1.5f, 0.3f, armColour[8].x, armColour[8].y, armColour[8].z,
		0.3f, 1.5f, -0.3f, armColour[8].x, armColour[8].y, armColour[8].z,

		0.3f, 1.5f, 0.3f, armColour[9].x, armColour[9].y, armColour[9].z,
		-0.3f, 1.5f, 0.3f, armColour[9].x, armColour[9].y, armColour[9].z,
		0.3f, 1.5f, -0.3f, armColour[9].x, armColour[9].y, armColour[9].z,

		-0.3f, -2.0f, -0.3f, handColour[0].x, handColour[0].y, handColour[0].z,
		-0.3f, -2.0f, 0.3f, handColour[0].x, handColour[0].y, handColour[0].z,
		0.3f, -2.0f, -0.3f, handColour[0].x, handColour[0].y, handColour[0].z,

		0.3f, -2.0f, 0.3f, handColour[1].x, handColour[1].y, handColour[1].z,
		-0.3f, -2.0f, 0.3f, handColour[1].x, handColour[1].y, handColour[1].z,
		0.3f, -2.0f, -0.3f, handColour[1].x, handColour[1].y, handColour[1].z,

		// hand
		0.3f, -1.5f, 0.3f, handColour[2].x, handColour[2].y, handColour[2].z,
		0.3f, -2.0f, 0.3f, handColour[2].x, handColour[2].y, handColour[2].z,
		0.3f, -2.0f, -0.3f, handColour[2].x, handColour[2].y, handColour[2].z,

		0.3f, -2.0f, -0.3f, handColour[3].x, handColour[3].y, handColour[3].z,
		0.3f, -1.5f, -0.3f, handColour[3].x, handColour[3].y, handColour[3].z,
		0.3f, -1.5f, 0.3f, handColour[3].x, handColour[3].y, handColour[3].z,

		-0.3f, -1.5f, 0.3f, handColour[4].x, handColour[4].y, handColour[4].z,
		-0.3f, -2.0f, 0.3f, handColour[4].x, handColour[4].y, handColour[4].z,
		-0.3f, -2.0f, -0.3f, handColour[4].x, handColour[4].y, handColour[4].z,

		-0.3f, -2.0f, -0.3f, handColour[5].x, handColour[5].y, handColour[5].z,
		-0.3f, -1.5f, -0.3f, handColour[5].x, handColour[5].y, handColour[5].z,
		-0.3f, -1.5f, 0.3f, handColour[5].x, handColour[5].y, handColour[5].z,

		0.3f, -1.5f, 0.3f, handColour[6].x, handColour[6].y, handColour[6].z,
		0.3f, -2.0f, 0.3f, handColour[6].x, handColour[6].y, handColour[6].z,
		-0.3f, -2.0f, 0.3f, handColour[6].x, handColour[6].y, handColour[6].z,

		-0.3f, -2.0f, 0.3f, handColour[7].x, handColour[7].y, handColour[7].z,
		-0.3f, -1.5f, 0.3f, handColour[7].x, handColour[7].y, handColour[7].z,
		0.3f, -1.5f, 0.3f, handColour[7].x, handColour[7].y, handColour[7].z,

		0.3f, -1.5f, -0.3f, handColour[8].x, handColour[8].y, handColour[8].z,
		0.3f, -2.0f, -0.3f, handColour[8].x, handColour[8].y, handColour[8].z,
		-0.3f, -2.0f, -0.3f, handColour[8].x, handColour[8].y, handColour[8].z,

		-0.3f, -2.0f, -0.3f, handColour[9].x, handColour[9].y, handColour[9].z,
		-0.3f, -1.5f, -0.3f, handColour[9].x, handColour[9].y, handColour[9].z,
		0.3f, -1.5f, -0.3f, handColour[9].x, handColour[9].y, handColour[9].z,
	};
	// start ints
	glCreateVertexArrays(1, &armVAO);
	glCreateBuffers(1, &armVBO);
	glBindVertexArray(armVAO);
	glBindBuffer(GL_ARRAY_BUFFER, armVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(armVertices), armVertices, GL_STATIC_DRAW);
	// pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void player::startLeg() {
	vector<vec3> armColour = colourVector(10, vec3(0.96f, 0.26f, 0.26f));
	vector<vec3> handColour = colourVector(10, vec3(0.26f, 0.53f, 0.96f));
	float legVertices[] = {
		0.5f, 1.75f, 0.5f, armColour[0].x, armColour[0].y, armColour[0].z,
		-0.5f, 1.75f, 0.5f, armColour[0].x, armColour[0].y, armColour[0].z,
		-0.5f, -1.75f, 0.5f, armColour[0].x, armColour[0].y, armColour[0].z,

		-0.5f, -1.75f, 0.5f, armColour[1].x, armColour[1].y, armColour[1].z,
		0.5f, -1.75f, 0.5f, armColour[1].x, armColour[1].y, armColour[1].z,
		0.5f, 1.75f, 0.5f, armColour[1].x, armColour[1].y, armColour[1].z,

		0.5f, 1.75f, -0.5f, armColour[2].x, armColour[2].y, armColour[2].z,
		-0.5f, 1.75f, -0.5f, armColour[2].x, armColour[2].y, armColour[2].z,
		-0.5f, -1.75f, -0.5f, armColour[2].x, armColour[2].y, armColour[2].z,

		-0.5f, -1.75f, -0.5f, armColour[3].x, armColour[3].y, armColour[3].z,
		0.5f, -1.75f, -0.5f, armColour[3].x, armColour[3].y, armColour[3].z,
		0.5f, 1.75f, -0.5f, armColour[3].x, armColour[3].y, armColour[3].z,

		// z
		0.5f, 1.75f, 0.5f, armColour[4].x, armColour[4].y, armColour[4].z,
		0.5f, 1.75f, -0.5f, armColour[4].x, armColour[4].y, armColour[4].z,
		0.5f, -1.75f, -0.5f, armColour[4].x, armColour[4].y, armColour[4].z,

		0.5f, -1.75f, -0.5f, armColour[5].x, armColour[5].y, armColour[5].z,
		0.5f, -1.75f, 0.5f, armColour[5].x, armColour[5].y, armColour[5].z,
		0.5f, 1.75f, 0.5f, armColour[5].x, armColour[5].y, armColour[5].z,

		-0.5f, 1.75f, 0.5f, armColour[6].x, armColour[6].y, armColour[6].z,
		-0.5f, 1.75f, -0.5f, armColour[6].x, armColour[6].y, armColour[6].z,
		-0.5f, -1.75f, -0.5f, armColour[6].x, armColour[6].y, armColour[6].z,

		-0.5f, -1.75f, -0.5f, armColour[7].x, armColour[7].y, armColour[7].z,
		-0.5f, -1.75f, 0.5f, armColour[7].x, armColour[7].y, armColour[7].z,
		-0.5f, 1.75f, 0.5f, armColour[7].x, armColour[7].y, armColour[7].z,

		// top
		-0.5f, 1.75f, -0.5f, armColour[8].x, armColour[8].y, armColour[8].z,
		-0.5f, 1.75f, 0.5f, armColour[8].x, armColour[8].y, armColour[8].z,
		0.5f, 1.75f, -0.5f, armColour[8].x, armColour[8].y, armColour[8].z,

		0.5f, 1.75f, 0.5f, armColour[9].x, armColour[9].y, armColour[9].z,
		-0.5f, 1.75f, 0.5f, armColour[9].x, armColour[9].y, armColour[9].z,
		0.5f, 1.75f, -0.5f, armColour[9].x, armColour[9].y, armColour[9].z,

		-0.5f, -2.75f, -0.5f, handColour[0].x, handColour[0].y, handColour[0].z,
		-0.5f, -2.75f, 0.5f, handColour[0].x, handColour[0].y, handColour[0].z,
		0.5f, -2.75f, -0.5f, handColour[0].x, handColour[0].y, handColour[0].z,

		0.5f, -2.75f, 0.5f, handColour[1].x, handColour[1].y, handColour[1].z,
		-0.5f, -2.75f, 0.5f, handColour[1].x, handColour[1].y, handColour[1].z,
		0.5f, -2.75f, -0.5f, handColour[1].x, handColour[1].y, handColour[1].z,

		// hand
		0.5f, -1.75f, 0.5f, handColour[2].x, handColour[2].y, handColour[2].z,
		0.5f, -2.75f, 0.5f, handColour[2].x, handColour[2].y, handColour[2].z,
		0.5f, -2.75f, -0.5f, handColour[2].x, handColour[2].y, handColour[2].z,

		0.5f, -2.75f, -0.5f, handColour[3].x, handColour[3].y, handColour[3].z,
		0.5f, -1.75f, -0.5f, handColour[3].x, handColour[3].y, handColour[3].z,
		0.5f, -1.75f, 0.5f, handColour[3].x, handColour[3].y, handColour[3].z,

		-0.5f, -1.75f, 0.5f, handColour[4].x, handColour[4].y, handColour[4].z,
		-0.5f, -2.75f, 0.5f, handColour[4].x, handColour[4].y, handColour[4].z,
		-0.5f, -2.75f, -0.5f, handColour[4].x, handColour[4].y, handColour[4].z,

		-0.5f, -2.75f, -0.5f, handColour[5].x, handColour[5].y, handColour[5].z,
		-0.5f, -1.75f, -0.5f, handColour[5].x, handColour[5].y, handColour[5].z,
		-0.5f, -1.75f, 0.5f, handColour[5].x, handColour[5].y, handColour[5].z,

		0.5f, -1.75f, 0.5f, handColour[6].x, handColour[6].y, handColour[6].z,
		0.5f, -2.75f, 0.5f, handColour[6].x, handColour[6].y, handColour[6].z,
		-0.5f, -2.75f, 0.5f, handColour[6].x, handColour[6].y, handColour[6].z,

		-0.5f, -2.75f, 0.5f, handColour[7].x, handColour[7].y, handColour[7].z,
		-0.5f, -1.75f, 0.5f, handColour[7].x, handColour[7].y, handColour[7].z,
		0.5f, -1.75f, 0.5f, handColour[7].x, handColour[7].y, handColour[7].z,

		0.5f, -1.75f, -0.5f, handColour[8].x, handColour[8].y, handColour[8].z,
		0.5f, -2.75f, -0.5f, handColour[8].x, handColour[8].y, handColour[8].z,
		-0.5f, -2.75f, -0.5f, handColour[8].x, handColour[8].y, handColour[8].z,

		-0.5f, -2.75f, -0.5f, handColour[9].x, handColour[9].y, handColour[9].z,
		-0.5f, -1.75f, -0.5f, handColour[9].x, handColour[9].y, handColour[9].z,
		0.5f, -1.75f, -0.5f, handColour[9].x, handColour[9].y, handColour[9].z,
	};
	// start ints
	glCreateVertexArrays(1, &legVAO);
	glCreateBuffers(1, &legVBO);
	glBindVertexArray(legVAO);
	glBindBuffer(GL_ARRAY_BUFFER, legVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(legVertices), legVertices, GL_STATIC_DRAW);
	// pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void player::runAnimation(float multiplier){
	if (multiplier == 0.0f) {
		int upDown = 0;
		if (legRotationTwo.x > 0) {
			upDown = 1;
			if (!aiming) {
				armRotation.x -= (float)deltaTime * 3 * 40.0f;
				armRotationTwo.x += (float)deltaTime * 3 * 40.0f;
			}
			legRotation.x += (float) deltaTime * 3 * 40.0f;
			legRotationTwo.x -= (float) deltaTime * 3 * 40.0f;
		}
		if (legRotationTwo.x < 0) {
			upDown = -1;
			if (!aiming) {
				armRotation.x += (float)deltaTime * 3 * 40.0f;
				armRotationTwo.x -= (float)deltaTime * 3 * 40.0f;
			}
			legRotation.x -= (float) deltaTime * 3 * 40.0f;
			legRotationTwo.x += (float) deltaTime * 3 * 40.0f;
		}
		if (upDown == -1) {
			if (legRotationTwo.x > 0) {
				if (!aiming) {
					armRotation.x = 0.0f;
					armRotationTwo.x = 0.0f;
				}
				legRotation.x = 0.0f;
				legRotationTwo.x = 0.0f;
			}
		}
		if (upDown == 1) {
			if (legRotationTwo.x < 0) {
				if (!aiming) {
					armRotation.x = 0.0f;
					armRotationTwo.x = 0.0f;
				}
				legRotation.x = 0.0f;
				legRotationTwo.x = 0.0f;
			}
		}
		return;
	}
	if (!finishedFirst) {
		if (!aiming) {
			armRotation.x += (float)deltaTime * 3 * multiplier;
			armRotationTwo.x -= (float)deltaTime * 3 * multiplier;
		}
		legRotation.x -= (float) deltaTime * 2.5f * multiplier;
		legRotationTwo.x += (float) deltaTime * 2.5f * multiplier;
		
		if (legRotationTwo.x >= 30.0f) {
			finishedFirst = true;
		}
	}
	if (!finishedSecond && finishedFirst) {
		if (!aiming) {
			armRotation.x -= (float)deltaTime * 3 * multiplier;
			armRotationTwo.x += (float)deltaTime * 3 * multiplier;
		}
		legRotation.x += (float) deltaTime * 2.5f * multiplier;
		legRotationTwo.x -= (float) deltaTime * 2.5f * multiplier;

		if (legRotationTwo.x <= -30.0f) {
			finishedSecond = true;
		}
	}
	if (finishedFirst && finishedSecond) {
		if (!aiming) {
			armRotation.x += (float)deltaTime * 3 * multiplier;
			armRotationTwo.x -= (float)deltaTime * 3 * multiplier;
		}
		legRotation.x -= (float) deltaTime * 2.5f * multiplier;
		legRotationTwo.x += (float) deltaTime * 2.5f * multiplier;

		if (legRotationTwo.x >= 0) {
			if (!aiming) {
				armRotation.x = 0.0f;
				armRotationTwo.x = 0.0f;
			}
			legRotation.x = 0.0f;
			legRotationTwo.x = 0.0f;

			finishedFirst = false;
			finishedSecond = false;
		}
	}
}

void player::startTorso() {
	vector<vec3> torsoColour = colourVector(12, vec3(0.27f, 0.87f, 0.92f));
	float torsoVertices[] = {
		// top
		-1.0f, 1.75f, 0.5f, torsoColour[0].x, torsoColour[0].y, torsoColour[0].z,
		-1.0f, 1.75f, -0.5f, torsoColour[0].x, torsoColour[0].y, torsoColour[0].z,
		1.0f, 1.75f, 0.5f, torsoColour[0].x, torsoColour[0].y, torsoColour[0].z,

		1.0f, 1.75f, 0.5f, torsoColour[1].x, torsoColour[1].y, torsoColour[1].z,
		1.0f, 1.75f, -0.5f, torsoColour[1].x, torsoColour[1].y, torsoColour[1].z,
		-1.0f, 1.75f, -0.5f, torsoColour[1].x, torsoColour[1].y, torsoColour[1].z,

		// bottom
		-1.0f, -1.75f, 0.5f, torsoColour[2].x, torsoColour[2].y, torsoColour[2].z,
		-1.0f, -1.75f, -0.5f, torsoColour[2].x, torsoColour[2].y, torsoColour[2].z,
		1.0f, -1.75f, 0.5f, torsoColour[2].x, torsoColour[2].y, torsoColour[2].z,

		1.0f, -1.75f, 0.5f, torsoColour[3].x, torsoColour[3].y, torsoColour[3].z,
		1.0f, -1.75f, -0.5f, torsoColour[3].x, torsoColour[3].y, torsoColour[3].z,
		-1.0f, -1.75f, -0.5f, torsoColour[3].x, torsoColour[3].y, torsoColour[3].z,

		//left
		-1.0f, 1.75f, 0.5f, torsoColour[4].x, torsoColour[4].y, torsoColour[4].z,
		-1.0f, -1.75f, 0.5f, torsoColour[4].x, torsoColour[4].y, torsoColour[4].z,
		-1.0f, -1.75f, -0.5f, torsoColour[4].x, torsoColour[4].y, torsoColour[4].z,

		-1.0f, 1.75f, 0.5f, torsoColour[5].x, torsoColour[5].y, torsoColour[5].z,
		-1.0f, 1.75f, -0.5f, torsoColour[5].x, torsoColour[5].y, torsoColour[5].z,
		-1.0f, -1.75f, -0.5f, torsoColour[5].x, torsoColour[5].y, torsoColour[5].z,

		// right
		1.0f, 1.75f, 0.5f, torsoColour[6].x, torsoColour[6].y, torsoColour[6].z,
		1.0f, -1.75f, 0.5f, torsoColour[6].x, torsoColour[6].y, torsoColour[6].z,
		1.0f, -1.75f, -0.5f, torsoColour[6].x, torsoColour[6].y, torsoColour[6].z,

		1.0f, 1.75f, 0.5f, torsoColour[7].x, torsoColour[7].y, torsoColour[7].z,
		1.0f, 1.75f, -0.5f, torsoColour[7].x, torsoColour[7].y, torsoColour[7].z,
		1.0f, -1.75f, -0.5f, torsoColour[7].x, torsoColour[7].y, torsoColour[7].z,

		// front
		-1.0f, 1.75f, 0.5f, torsoColour[8].x, torsoColour[8].y, torsoColour[8].z,
		-1.0f, -1.75f, 0.5f, torsoColour[8].x, torsoColour[8].y, torsoColour[8].z,
		1.0f, -1.75f, 0.5f, torsoColour[8].x, torsoColour[8].y, torsoColour[8].z,

		1.0f, 1.75f, 0.5f, torsoColour[9].x, torsoColour[9].y, torsoColour[9].z,
		-1.0f, 1.75f, 0.5f, torsoColour[9].x, torsoColour[9].y, torsoColour[9].z,
		1.0f, -1.75f, 0.5f, torsoColour[9].x, torsoColour[9].y, torsoColour[9].z,

		// back
		-1.0f, 1.75f, -0.5f, torsoColour[10].x, torsoColour[10].y, torsoColour[10].z,
		-1.0f, -1.75f, -0.5f, torsoColour[10].x, torsoColour[10].y, torsoColour[10].z,
		1.0f, -1.75f, -0.5f, torsoColour[10].x, torsoColour[10].y, torsoColour[10].z,

		1.0f, 1.75f, -0.5f, torsoColour[11].x, torsoColour[11].y, torsoColour[11].z,
		-1.0f, 1.75f, -0.5f, torsoColour[11].x, torsoColour[11].y, torsoColour[11].z,
		1.0f, -1.75f, -0.5f, torsoColour[11].x, torsoColour[11].y, torsoColour[11].z,

	};
	// start ints
	glCreateVertexArrays(1, &torsoVAO);
	glCreateBuffers(1, &torsoVBO);
	glBindVertexArray(torsoVAO);
	glBindBuffer(GL_ARRAY_BUFFER, torsoVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(torsoVertices), torsoVertices, GL_STATIC_DRAW);
	// pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void startPlayerShader(){
	int vertShad = createShader("assets/shaders/terrainVert.txt", GL_VERTEX_SHADER);
	int fragShad = createShader("assets/shaders/terrainFrag.txt", GL_FRAGMENT_SHADER);
	playerShader = createProgram({ vertShad, fragShad });
}

float shotDelays[] = { 0.25f, 0.1f };
float automaticGun[] = { false, true };

void player::shoot() {
	if (allWeapons[currentWeapon].currentAmmo < 1) {
		canShoot = false;
	}
	int shootButton = stoi(inputLines[5]);
	if (!canShoot) {
		laserColour = vec3(1.0f, 0.0f, 0.0f);
		if (!equippingReloading && !allWeapons[currentWeapon].currentAmmo < 1) {
			currentDelay -= (float)deltaTime;
			if (currentDelay < 0) {
				currentDelay = 0.0f;
				canShoot = true;
			}
		}
	}
	if (canShoot) {
		laserColour = vec3(0.0f, 0.0f, 1.0f);
	}
	// check if auto key down
	bool keyDown = false;
	if (!automaticGun[currentWeapon]) {
		if (checkKeyDown(shootButton)) {
			keyDown = true;
		}
	}
	if (automaticGun[currentWeapon]) {
		if (checkKey(shootButton)) {
			keyDown = true;
		}
	}
	// shoot
	if (keyDown && canShoot && aiming) {
		allWeapons[currentWeapon].currentAmmo -= 1;

		vec3 bulletRot = vec3(0.0f);
		bulletRot.y = rotation.y + 90.0f;
		bulletRot.z = playerPitch;

		vec3 bulletPos = position;
		bulletPos.z += 0.7f * sin(radians(rotation.y));
		bulletPos.y += 0.25f;
		bulletPos.x += 0.7f * -cos(radians(rotation.y));

		createBullet(bulletPos, bulletRot);

		canShoot = false;
		currentDelay = shotDelays[currentWeapon];
	}
}

void player::equipReloadAnimation(float multiplier) {
	canShoot = false;
	if (!finishedEquipFirst) {
		armRotationTwo.x += (float) deltaTime * multiplier;
		totalGoneUpEquipping += (float) deltaTime * multiplier;
		if (totalGoneUpEquipping > 15.0f) {
			finishedEquipFirst = true;
		}
	}
	if (finishedEquipFirst) {
		armRotationTwo.x -= (float) deltaTime * multiplier;
		totalGoneUpEquipping -= (float) deltaTime * multiplier;
		if (totalGoneUpEquipping < 0.0f) {
			finishedEquipFirst = false;
			equippingReloading = false;
			resetArms();
			armRotationTwo.x -= totalGoneUpEquipping;
			totalGoneUpEquipping = 0.0f;
		}
	}
}

void player::resetArms() {
	if (!equippingReloading) {
		armRotationTwo.x = armRotation.x;
	}
}

void player::reload() {
	if (allWeapons[currentWeapon].currentAmmo < 1) {
		if (allWeapons[currentWeapon].shotDelayCurrent == allWeapons[currentWeapon].shotDelay) {
			equippingReloading = true;
			reloadSpeed = 30.0f / allWeapons[currentWeapon].shotDelay;
		}
		allWeapons[currentWeapon].shotDelayCurrent -= deltaTime;
		if (allWeapons[currentWeapon].shotDelayCurrent < 0) {
			allWeapons[currentWeapon].currentAmmo = allWeapons[currentWeapon].maxAmmo;
			allWeapons[currentWeapon].shotDelayCurrent = allWeapons[currentWeapon].shotDelay;
		}
	}
}