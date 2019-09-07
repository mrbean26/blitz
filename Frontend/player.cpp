#include "model.h"
#include "player.h"
#include "shader.h"
#include "display.h"
#include "worldGeneration.h"
#include "backend.h"
#include "frontend.h"
#include "structures.h"
#include "weapons.h"
#include "monsters.h"
#include "inventory.h"
#include "people.h"

#include <glm/gtc/type_ptr.hpp>
using namespace glm;

float CROUCH_HEIGHT_TAKEAWAY = 2.125f;

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

int diedText, respawnButton;
int pauseKey;
void player::begin(){
	playerTexture = loadTexture("assets/models/player/skin.png");
	startCharacterVertices();
	playerView = true;
	inputLines = readLines("assets/saves/inputs.save");
	pauseKey = stoi(inputLines[10]);
	startPlayerUI();

	diedText = createText();
	allTexts[diedText].fontCharacters = getFont("assets/fonts/zekton.ttf", 40);
	allTexts[diedText].displayedText = "You Died!";
	allTexts[diedText].position = vec2(display_x / 2, display_y / 2 + display_y / 10);
	allTexts[diedText].centered = true;
	allTexts[diedText].active = false;
	allTexts[diedText].fontSize = display_x / 40;
	respawnButton = createButton();
	allButtons[respawnButton].texture = loadTexture("assets/images/respawnButton.png");
	allButtons[respawnButton].scale = vec2(0.5f);
	allButtons[respawnButton].position = vec3(0.0f, -1.0f, 0.0f);
	allButtons[respawnButton].active = false;
}

void player::mainloop(){
	if (!WorldGeneration.startedBegin) { return; }
	renderPlayer();
	movement();
	researchAnimation(120.0f);
	cameraMovement();
	monsterColliders();
	collisions();
	pauseUIInteraction();
	shoot();
	reload();
	respawn();
	if(allButtons[respawnButton].clickUp){ respawning=true; }
	if(redDelay < 0.0f && !respawning){ multiplyColour=vec3(1.0f); }
	if(health < 1){ multiplyColour = vec3(2.5f, 0.2f, 0.2f); }
	redDelay -= deltaTime;
}

void player::respawn(){
	if(!respawning){
		return;
	}
	multiplyColour = vec3(0.2f, 2.5f, 0.2f);
	health = 100;
	dead = false;
	allTexts[diedText].active = false;
	allButtons[respawnButton].active = false;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	rotation.x -= deltaTime * DYING_ROTATE_SPEED / 4.0f;
	if(rotation.x < 0.0f){
		rotation.x = 0.0f;
		respawning = false;
		
		// kill monstyers within radius
		int monsterCount = allMonsters.size();
		for(int m = 0; m < monsterCount; m++){
			vec3 pos = allMonsters[m].position;
			float distance = glm::distance(pos, position);
			if(distance < PLAYER_RESPAWN_KILL_RADIUS){
				allMonsters[m].health = -10.0f;
			}
		}
	}
}

int continueButton, exitButton;
int ammoIcon, healthIcon;
int ammoText, healthText;
bool paused = false;
void startPlayerUI() {
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
	ammoIcon = createButton();
	allButtons[ammoIcon].texture = loadTexture("assets/images/ammoIcon.png");
	allButtons[ammoIcon].scale = vec2(0.3f);
	allButtons[ammoIcon].interactive = false;
	allButtons[ammoIcon].position = vec3(-11.0f, -8.3f, 0.0f);
	healthIcon = createButton();
	allButtons[healthIcon].texture = loadTexture("assets/images/healthIcon.png");
	allButtons[healthIcon].scale = vec2(0.25f);
	allButtons[healthIcon].interactive = false;
	allButtons[healthIcon].position = vec3(-18.5f, -10.0f, 0.0f);
	ammoText = createText();
	allTexts[ammoText].fontCharacters = getFont("assets/fonts/zekton.ttf", display_x / 30);
	healthText = createText();
	allTexts[healthText].fontCharacters = getFont("assets/fonts/zekton.ttf", display_x / 30);
}

void exitToMenus() {
	// exit to main menu
	// world generation
	glDeleteVertexArrays(1, &WorldGeneration.terrainVAO);
	glDeleteVertexArrays(1, &WorldGeneration.areaLimitVAO);
	glDeleteVertexArrays(1, &WorldGeneration.waterVAO);
	glDeleteBuffers(1, &WorldGeneration.terrainVBO);
	glDeleteBuffers(1, &WorldGeneration.areaLimitVBO);
	glDeleteBuffers(1, &WorldGeneration.waterVBO);
	WorldGeneration.areaLimitCount = 0;
	// bools
	mainPlayer.active = false;
	allButtons.clear();
	allTexts.clear();
	StartScreen.active = true;
	allButtons = previousAllButtons;
	allTexts = previousAllTexts;
	debugText = debugTextPlaceholder;
    alertText = alertTextPlaceholder;
    alertBackground = alertBackgroundPlaceholder;
	loading = false;
	mainPlayer.canMove = true;
	paused = false;
	// worldGeneration.h
	currentAllMountainPositions.clear();
	currentAllMountainScales.clear();
	currentAllMountainWaters.clear();
	flatTerrainTriangles.clear();
	mountainTriangles.clear();
	waterTriangles.clear();
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
	// player values
	string posLine = to_string(mainPlayer.position.x) + " " + to_string(mainPlayer.position.y) + " " + to_string(mainPlayer.position.z);
	currentAllLines = rewriteLine(currentAllLines, "playerPos", posLine);
	currentAllLines = rewriteLine(currentAllLines, "playerHealth", to_string(mainPlayer.health));
	currentAllLines = rewriteLine(currentAllLines, "inventory", inventorySaveLine()[0]);
	currentAllLines = rewriteLine(currentAllLines, "inventoryQuantity", inventorySaveLine()[1]);
	currentAllLines = rewriteLine(currentAllLines, WorldGeneration.currentAreaPrefix + "DataPoints", to_string(WorldGeneration.dataPoints));
	mainPlayer.deleteMemory();
	allSlots.clear();
	allItems.clear();
	allSlotTexts.clear();
	currentSwappingIndex = -1;
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
	// people
	iterator = currentAllLines.begin();
	while (iterator != currentAllLines.end()) {
		if (contains(*iterator, WorldGeneration.currentAreaPrefix + "Person")) { // delete values
			iterator = currentAllLines.erase(iterator);
		}
		else {
			++iterator;
		}
	}
	// write
	int personCount = allPeople.size();
	for (int p = 0; p < personCount; p++) {
		player person = allPeople[p];
		currentAllLines[newVectorPos(&currentAllLines)] = WorldGeneration.currentAreaPrefix + "Person " + to_string(person.position.x) +
			" " + to_string(person.rotation.y) + " " + to_string(person.position.z) + " " +
			to_string(person.personType);
	}
	allPeople.clear();
	// all entities
	// erase
	iterator = currentAllLines.begin();
	while (iterator != currentAllLines.end()) {
		if (contains(*iterator, WorldGeneration.currentAreaPrefix + "Entity")) { // delete values
			iterator = currentAllLines.erase(iterator);
		}
		else {
			++iterator;
		}
	}
	// write
	vector<string> newEntityLines = entitySaveLines();
	int eLines = newEntityLines.size();
	for(int e = 0; e < eLines; e++){
		currentAllLines[newVectorPos(&currentAllLines)] = newEntityLines[e];
	}
	allEntities.clear();
	// write
	writeLines(WorldGeneration.worldLinesPath, currentAllLines);
	// structures.h
	mountainLimits.clear();
	allMiniBuildings.clear();
	newBuildingLines.clear();
}

void pauseUIInteraction() {
	button ammoButtonVar = allButtons[ammoIcon];
	allTexts[ammoText].position = vec2(ammoButtonVar.maxX * 1.03f, display_y - (ammoButtonVar.maxY * 0.965f));
	weapon playerCurrentWeapon = allWeapons[mainPlayer.currentWeapon];
	allTexts[ammoText].displayedText = to_string(playerCurrentWeapon.currentAmmo) + "/" + to_string(playerCurrentWeapon.maxAmmo);

	button healthButtonVar = allButtons[healthIcon];
	allTexts[healthText].position = vec2(healthButtonVar.maxX * 1.04f, display_y - (healthButtonVar.maxY * 0.97f));
	float displayedHealth = glm::clamp(mainPlayer.health, 0.0f, 100000.0f);
	string healthTextDisplay = removeAfterCharacter(to_string(displayedHealth), ".");
	allTexts[healthText].displayedText = healthTextDisplay;

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
	if ((checkKeyDown(pauseKey) && !benchInUse) || changePaused) {
		if (!paused) {
			paused = true;
			allTexts[diedText].active = false;
			allButtons[respawnButton].active = false;
			allButtons[exitButton].active = true;
			allButtons[continueButton].active = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			mainPlayer.canMove = false;
			return;
		}
		if (paused) {
			paused = false;
			if(mainPlayer.health < 1){
				allTexts[diedText].active = true;
				allButtons[respawnButton].active = true;
			}
			allButtons[exitButton].active = false;
			allButtons[continueButton].active = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			mainPlayer.canMove = true;
		}
	}
}

float startedLowestY = -999.0f;
void player::deleteMemory(){
	position = vec3(0.0f);
	rotation = vec3(0.0f);
	health = 100;
}

void player::crouchMoveAnimation(float multiplier){
	if(!finishedFirst){
		armRotation.z -= deltaTime * multiplier;
		legRotation.z -= deltaTime * multiplier;

		armRotationTwo.z += deltaTime * multiplier;
		legRotationTwo.z += deltaTime * multiplier;
		
		float compare = degreesClamp(legRotationTwo.z);
		if(compare > 30.0f){
			finishedFirst = true;
		}
	}
	if(finishedFirst){
		armRotation.z += deltaTime * multiplier;
		legRotation.z += deltaTime * multiplier;

		armRotationTwo.z -= deltaTime * multiplier;
		legRotationTwo.z -= deltaTime * multiplier;

		float compare = degreesClamp(legRotationTwo.z);
		if(compare < 360.0f && compare > 180.0f){
			finishedFirst = false;
		}
	}
}

float degreesDistance(float v1, float v2){
	float one = degreesClamp(v1);
	float two = degreesClamp(v2);
	float distance = glm::distance(one, two);
	if(distance > 180.0f){
        distance = glm::distance(glm::max(one, two) - 360.0f, glm::min(one, two));
	}
	return distance;
}

bool lastOnBench = false;
float lastPitch;
bool crouchingInCrater = false;
void player::movement(){
	if(health < 1){
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		rotation.x += deltaTime * DYING_ROTATE_SPEED;
		allTexts[diedText].active = true;
		allButtons[respawnButton].active = true;
		if(rotation.x > 90.0f){
			rotation.x = glm::clamp(rotation.x, 0.0f, 90.0f);
			dead = true;
			// drop items
			for(int s = 0; s < SLOT_COUNT; s++){
				if(allSlots[s].itemType != -1){
					// drop
					item newEntity = allItems[allSlots[s].itemType];
					newEntity.itemType = allSlots[s].itemType; 
					newEntity.modelPosition = position;
					newEntity.quantity = allSlots[s].itemQuantity;
					allItems[0].removeButton(allSlots[s].buttonIconIndex);
					// reset
					allSlots[s].itemType = -1;
					allSlots[s].itemQuantity = 0;

					allEntities[newVectorPos(&allEntities)] = newEntity;

				}
			}
			// middle pos
			position.x = currentPlanetScale.x / 2.0f;
			position.z = -currentPlanetScale.y / 2.0f;
		}
	}
	if (!canMove || health < 1) { return; }
	int forwardKey = stoi(inputLines[0]);
	int leftKey = stoi(inputLines[1]);
	int backKey = stoi(inputLines[2]);
	int rightKey = stoi(inputLines[3]);
	
	int sprintKey = stoi(inputLines[7]);
	int jumpKey = stoi(inputLines[9]);
	int aimButton = stoi(inputLines[4]);
	int crouchKey = stoi(inputLines[8]);

	if (!checkKey(aimButton)) {
		if (checkKey(shootButton)) {
			researching = true;
		}
	}

	if(checkKeyDown(crouchKey) && !researching){
		if(!changingCrouch){
			changingCrouch = true;
		}
	}

	if(changingCrouch){
		if(!crouching && !researching){
			rotation.x -= deltaTime * PLAYER_CROUCH_SPEED;
			float clampedRotation = degreesClamp(rotation.x);
			if(clampedRotation < 270.0f && clampedRotation > 90.0f){
				rotation.x = 270.0f;
				changingCrouch = false;
				crouching = true;

				armRotation.x = 0.0f;
				armRotationTwo.x = 0.0f;
				legRotation.x = 0.0f;
				legRotationTwo.x = 0.0f;
				finishedFirst = false; 
				finishedSecond = false;
				return;
			}
		}
		if(crouching){
			rotation.x += deltaTime * PLAYER_CROUCH_SPEED;
			float clampedRotation = degreesClamp(rotation.x);
			if(clampedRotation > 0.0f && clampedRotation < 270.0f){
				rotation.x = 0.0f;
				changingCrouch = false;
				crouching = false;

				armRotation.z = 0.0f;
				armRotationTwo.z = 0.0f;
				legRotation.z = 0.0f;
				legRotationTwo.z = 0.0f;
				finishedFirst = false; 
				finishedSecond = false;
			}
		}
		return;
	}
	if(crouching){
		vec2 playerFloor = vec2(position.x, position.z);

		int mountainIndex = -1;
		int mCount = currentAllMountainPositions.size();
		for(int m = 0; m < mCount; m++){
			vec2 pos = currentAllMountainPositions[m]; pos.y *= -1.0f;
			vec3 sca = currentAllMountainScales[m];
			float currentRad = (sca.x * 100.0f) * 0.025f;
			float distance = glm::distance(pos, playerFloor);
			if(currentRad - distance > 0){
				mountainIndex = m;
			}
		}
		crouchingInCrater = false;
		if(mountainIndex > -1){
			vec2 floorPos = vec2(position.x, position.z);
			vec2 mPosition = currentAllMountainPositions[mountainIndex];
			mPosition.y = -mPosition.y;
			vec3 scale = currentAllMountainScales[mountainIndex];
			float radius = (scale.x * 100.0f) * 0.025f;

			vec2 leftEdge = vec2(mPosition.x - radius, 0.0f);
			vec2 center = vec2(mPosition.x, scale.y);

			float upAngle = bearingTwo(leftEdge, center);
			float downAngle = 180.0f - upAngle;

			float bearing = bearingTwo(mPosition, floorPos);
			float main = degreesDistance(rotation.y, bearing);
			main = -((main - 90.0f) / 90.0f);

			float angle = -upAngle + 90.0f;
			if(main < 0){
				angle = -downAngle + 90.0f;
				main = -main;
			}
			if(scale.z < 0){ 
				angle = -angle;
				crouchingInCrater = true;
			}
			rotation.x = angle * main - 90.0f;
			position.y += 0.5f;
		}
		if(mountainIndex ==  -1){
			rotation.x = 270.0f;
			rotation.z = 0.0f;
		}
	}
	movingMultiplier = 0.0f;
	velocity = vec3(0.0f);
	if (checkKey(backKey) && !checkKey(forwardKey)) {
		movingMultiplier = 25.0f;
		velocity = vec3(BACKWARD_SPEED);
		if(crouching){
			velocity = vec3(BACKWARD_SPEED / 3.0f);
		}
	}
	if (checkKey(forwardKey) && !checkKey(backKey)) {
		velocity = vec3(WALK_SPEED);
		if(crouching){
			velocity = vec3(WALK_SPEED / 3.0f);
		}
		movingMultiplier = 30.0f;
		if (checkKey(sprintKey)) {
			movingMultiplier = 50.0f;
			velocity = vec3(RUN_SPEED);
			if(crouching){
				velocity = vec3(RUN_SPEED / 3.0f);
			}
		}
	}
	if (checkKeyDown(jumpKey) && !jumping && !crouching) {
		jumping = true;
		startedLowestY = lowestY;
		jumpVelocity = JUMP_HEIGHT;
	}
	if (jumping) {
		jumpVelocity -= deltaTime * JUMP_SPEED;
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
	if (checkKey(rightKey)) { 
		rotation.y -= deltaTime * PLAYER_ROTATE_SPEED;
	}
	if (checkKey(leftKey)) {
		rotation.y += deltaTime * PLAYER_ROTATE_SPEED;
	}
	if(!crouching){
		runAnimation(movingMultiplier);
	}
	if(crouching){
		crouchMoveAnimation(movingMultiplier);
	}
	if (checkKey(aimButton) && !crouching && !researching) {
		if (!aiming) {
			armRotation.x = 90.0f;
			armRotationTwo.x = 90.0f;
			armRotationTwo.z = -45.0f;
			lastPitch = playerPitch;
			equippingReloading = true;
			reloadSpeed = allWeapons[currentWeapon].equipTime;
		}
		aiming = true;
		armScale = vec3(0.1f, 0.4f, 0.1f);
		armScaleTwo = vec3(0.1f, 0.4f, 0.1f);
		if (!equippingReloading) {
			armRotationTwo.z = -45.0f;
		}
        armRotation.x = playerPitch + 90.0f;
        if(!equippingReloading){
            armRotationTwo.x = playerPitch + 90.0f;
        }
		if(crouching){
			armRotation.x += 135.0f;
			armRotationTwo.x += 135.0f;
			armRotation.x = glm::clamp(armRotation.x, 250.0f, 360.0f);
			armRotationTwo.x = glm::clamp(armRotationTwo.x, 250.0f, 360.0f);
		}
		if(!crouching){
			armRotation.x = glm::clamp(armRotation.x, 10.0f, 170.0f);
			armRotationTwo.x = glm::clamp(armRotationTwo.x, 10.0f, 170.0f);
		}
		armPositionTwo = vec3(0.55f, -0.425f, 0.0f);
		aimingView = true;

		lastPitch = playerPitch;
	}
	if (!checkKey(aimButton)) {
		if (aiming) {
			armRotation.x = 0.0f;
			armPositionTwo = vec3(0.9f, -0.55f, 0.0f);
			armRotationTwo = vec3(0.0f);
			aimingView = false;

			equippingReloading = false;
			totalGoneUpEquipping = 0.0f;
			armRotationTwo.z = 0.0f;
			equippingStep = 0;
		}
		armPositionTwo = vec3(0.72f, -0.55f, 0.0f);
		aiming = false;
		armScale = vec3(0.3f);
		armScaleTwo = vec3(0.3f);
	}

	if (equippingReloading) {
        equipReloadAnimation(allWeapons[currentWeapon].equipTime);
	}

	// physical movement

	float xGap = -sin(radians(rotation.y));
	float zGap = -cos(radians(rotation.y));
	vec3 rotationMovement = vec3(xGap, 1.0f, zGap) * vec3(deltaTime);
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
	vec4 cameraCollide = terrainColliders(cameraThirdPos, 0.0f);
	cameraMountainIndex = (int) cameraCollide.w - 1;
	float usedY = LEG_LENGTH;
	if(crouching){ 
		usedY = usedY - CROUCH_HEIGHT_TAKEAWAY;
		if(crouchingInCrater){
			usedY += CROUCH_CRATER_ADD;
		}
	}
	vec4 playerCollide = terrainColliders(position, usedY);
	bool inMountain = (int) playerCollide.w > 0;
	bool cameraInMountain = (int) cameraCollide.w > 0;
	if (position.y < playerCollide.y && jumping) { position.y = playerCollide.y; }
	if (!jumping) { position.y = playerCollide.y; }
	lowestY = playerCollide.y;
	lowestCameraY = cameraCollide.y + 0.5f;
	bool useless = false;
	buildCollisions(position, insideBuildingIndex, jumpVelocity, lastOnBench, useless);
	// flat terrain collisions
	float legPos = position.y - LEG_LENGTH;
	if(crouching){
		legPos += CROUCH_HEIGHT_TAKEAWAY;
	}
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
	if (insideBuildingIndex > -1) {
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
	if (lastFrameMove && health > 0) {
		playerYaw -= mouseDiffer.x * sensitivity;
		playerPitch += mouseDiffer.y * sensitivity;
		if (aiming) {
			rotation.y -= mouseDiffer.x * sensitivity;
		}
	}
	playerPitch = clamp(playerPitch, -80.0f, 80.0f);
	lastFrameMove = true;
}

void player::renderPlayer(){
	vector<vec3> positions = { headPosition, torsoPosition, armPosition, legPosition, armPositionTwo, legPositionTwo };
	vector<vec3> rotations = { headRotation, torsoRotation, armRotation, legRotation, armRotationTwo, legRotationTwo };
	vector<vec3> scales = { headScale, torsoScale, armScale, legScale, armScaleTwo, legScaleTwo };

	vector<readyTextureModel> models = { headModel, torsoModel, armModel, legModel, armModel, legModel };
	vec3 headParentPos = position - vec3(
		sin(radians(rotation.y)) * sin(radians(rotation.x)) * -1.2f, 
		-1.2f * cos(radians(rotation.x)),
		cos(radians(rotation.y)) * sin(radians(rotation.x)) * -1.2f);
	vec3 legParentPos = position - vec3(
		sin(radians(rotation.y)) * sin(radians(rotation.x)) * 0.72f, 
		0.72f * cos(radians(rotation.x)),
		cos(radians(rotation.y)) * sin(radians(rotation.x)) * 0.72f);
	vec3 armParentPos = position + vec3(
		0.55f * sin(radians(rotation.x)) * sin(radians(rotation.y)), 
		0.55f * cos(radians(rotation.x)), 
		0.55f * sin(radians(rotation.x)) * cos(radians(rotation.y)));
	vector<vec3> parentPositions = { headParentPos, position, armParentPos, legParentPos, armParentPos, legParentPos };
	for (int i = 0; i < 6; i++) {
		if(i < 2 && aiming){
			continue;
		}

		bool inWater = false;
		if (cameraThirdPos.y <= 0.05f) {
			int index = (int)terrainColliders(cameraThirdPos, 0.0f).w;
			index = index - 1;
			if (index > -1) {
				inWater = currentAllMountainWaters[index];
			}
		}

		vec3 combinedScale = scale * scales[i];
		models[i].render(modelMatrix(positions[i], rotations[i], combinedScale, true, parentPositions[i], rotation), true, 
			vec3(1.0f), inWater, WorldGeneration.waterMultiplyColour);
	}
	// laser
	if (aiming) {
		glUseProgram(playerShader);
		setShaderVecThree(playerShader, "multiplyColour", laserColour);
		setShaderInt(playerShader, "useLight", 0);
		glBindVertexArray(laserVAO);
		setShaderFloat(playerShader, "alpha", 0.5f);
		float xRot = playerPitch;
		if(crouching){
			xRot += 135.0f;
		}
		setMat4(playerShader, "model", modelMatrix(vec3(2.0f, 0.0f, -0.65f), vec3(armRotation.x - 90.0f, 90.0f, 0.0f), vec3(250.0f, 0.1f, 0.1f),
			true, armParentPos, rotation));
		glDrawArrays(GL_TRIANGLES, 0, 24);
		glLinkProgram(playerShader);
		// weapon
		if (currentWeapon == 0) {
			allWeapons[currentWeapon].render(modelMatrix(vec3(1.2f, -0.3f, -0.7f), 
				vec3(armRotation.x - 90.0f, 90.0f, 0.0f), vec3(0.4f), true, 
					armParentPos, rotation));
		}
		if (currentWeapon == 1) {
			allWeapons[currentWeapon].render(modelMatrix(vec3(0.6f, 0.0f, 1.2f),
				vec3(armRotation.x - 90.0f, 180.0f, 0.0f), vec3(0.8f), true,
				armParentPos, rotation));
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
	glGenVertexArrays(1, &laserVAO);
	glGenBuffers(1, &laserVBO);
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
	vector<float> headVerts = { -2.000000f, -2.000000f, 2.000000f, 0.214239f, 0.703683f, 2.000000f, -2.000000f, 2.000000f, 0.355532f, 0.703684f, 2.000000f, 2.000000f, 2.000000f, 0.355533f, 0.844979f, -2.000000f, -2.000000f, 2.000000f, 0.214239f, 0.703683f, 2.000000f, 2.000000f, 2.000000f, 0.355533f, 0.844979f, -2.000000f, 2.000000f, 2.000000f, 0.214239f, 0.844978f, -2.000000f, 2.000000f, 2.000000f, 0.214239f, 0.844978f, 2.000000f, 2.000000f, 2.000000f, 0.355533f, 0.844979f, 2.000000f, 2.000000f, -2.000000f, 0.355532f, 0.986273f, -2.000000f, 2.000000f, 2.000000f, 0.214239f, 0.844978f, 2.000000f, 2.000000f, -2.000000f, 0.355532f, 0.986273f, -2.000000f, 2.000000f, -2.000000f, 0.214239f, 0.986273f, -2.000000f, 2.000000f, -2.000000f, 0.638122f, 0.844978f, 2.000000f, 2.000000f, -2.000000f, 0.496827f, 0.844978f, 2.000000f, -2.000000f, -2.000000f, 0.496827f, 0.703683f, -2.000000f, 2.000000f, -2.000000f, 0.638122f, 0.844978f, 2.000000f, -2.000000f, -2.000000f, 0.496827f, 0.703683f, -2.000000f, -2.000000f, -2.000000f, 0.638122f, 0.703684f, -2.000000f, -2.000000f, -2.000000f, 0.214239f, 0.562390f, 2.000000f, -2.000000f, -2.000000f, 0.355533f, 0.562389f, 2.000000f, -2.000000f, 2.000000f, 0.355532f, 0.703684f, -2.000000f, -2.000000f, -2.000000f, 0.214239f, 0.562390f, 2.000000f, -2.000000f, 2.000000f, 0.355532f, 0.703684f, -2.000000f, -2.000000f, 2.000000f, 0.214239f, 0.703683f, 2.000000f, -2.000000f, 2.000000f, 0.355532f, 0.703684f, 2.000000f, -2.000000f, -2.000000f, 0.496827f, 0.703683f, 2.000000f, 2.000000f, -2.000000f, 0.496827f, 0.844978f, 2.000000f, -2.000000f, 2.000000f, 0.355532f, 0.703684f, 2.000000f, 2.000000f, -2.000000f, 0.496827f, 0.844978f, 2.000000f, 2.000000f, 2.000000f, 0.355533f, 0.844979f, -2.000000f, -2.000000f, -2.000000f, 0.072944f, 0.703683f, -2.000000f, -2.000000f, 2.000000f, 0.214239f, 0.703683f, -2.000000f, 2.000000f, 2.000000f, 0.214239f, 0.844978f, -2.000000f, -2.000000f, -2.000000f, 0.072944f, 0.703683f, -2.000000f, 2.000000f, 2.000000f, 0.214239f, 0.844978f, -2.000000f, 2.000000f, -2.000000f, 0.072944f, 0.844979f, };
	readyTextureModel newHead(headVerts, playerTexture, true);
	headModel = newHead;
}

void player::startArm() {
	vector<float> armVerts = { -1.000000f, -0.000000f, 1.000000f, 0.110906f, 0.425123f, 1.000000f, -0.000000f, 1.000000f, 0.180302f, 0.425123f, 1.000000f, 3.000000f, 1.000000f, 0.180302f, 0.529216f, -1.000000f, -0.000000f, 1.000000f, 0.110906f, 0.425123f, 1.000000f, 3.000000f, 1.000000f, 0.180302f, 0.529216f, -1.000000f, 3.000000f, 1.000000f, 0.110906f, 0.529216f, -1.000000f, -3.000000f, 1.000000f, 0.110906f, 0.321030f, 1.000000f, -0.000000f, 1.000000f, 0.180302f, 0.425123f, -1.000000f, -0.000000f, 1.000000f, 0.110906f, 0.425123f, -1.000000f, -3.000000f, 1.000000f, 0.110906f, 0.321030f, 1.000000f, -3.000000f, 1.000000f, 0.180302f, 0.321030f, 1.000000f, -0.000000f, 1.000000f, 0.180302f, 0.425123f, -1.000000f, 3.000000f, 1.000000f, 0.110906f, 0.529216f, 1.000000f, 3.000000f, 1.000000f, 0.180302f, 0.529216f, 1.000000f, 3.000000f, -1.000000f, 0.180302f, 0.598611f, -1.000000f, 3.000000f, 1.000000f, 0.110906f, 0.529216f, 1.000000f, 3.000000f, -1.000000f, 0.180302f, 0.598611f, -1.000000f, 3.000000f, -1.000000f, 0.110906f, 0.598611f, -1.000000f, 3.000000f, -1.000000f, 0.319092f, 0.529216f, 1.000000f, 3.000000f, -1.000000f, 0.249697f, 0.529216f, 1.000000f, 0.000000f, -1.000000f, 0.249697f, 0.425123f, -1.000000f, 3.000000f, -1.000000f, 0.319092f, 0.529216f, 1.000000f, 0.000000f, -1.000000f, 0.249697f, 0.425123f, -1.000000f, 0.000000f, -1.000000f, 0.319092f, 0.425123f, -1.000000f, 0.000000f, -1.000000f, 0.319092f, 0.425123f, 1.000000f, 0.000000f, -1.000000f, 0.249697f, 0.425123f, 1.000000f, -3.000000f, -1.000000f, 0.249697f, 0.321030f, -1.000000f, 0.000000f, -1.000000f, 0.319092f, 0.425123f, 1.000000f, -3.000000f, -1.000000f, 0.249697f, 0.321030f, -1.000000f, -3.000000f, -1.000000f, 0.319092f, 0.321030f, 1.000000f, -0.000000f, 1.000000f, 0.180302f, 0.425123f, 1.000000f, 0.000000f, -1.000000f, 0.249697f, 0.425123f, 1.000000f, 3.000000f, -1.000000f, 0.249697f, 0.529216f, 1.000000f, -0.000000f, 1.000000f, 0.180302f, 0.425123f, 1.000000f, 3.000000f, -1.000000f, 0.249697f, 0.529216f, 1.000000f, 3.000000f, 1.000000f, 0.180302f, 0.529216f, 1.000000f, -3.000000f, 1.000000f, 0.180302f, 0.321030f, 1.000000f, 0.000000f, -1.000000f, 0.249697f, 0.425123f, 1.000000f, -0.000000f, 1.000000f, 0.180302f, 0.425123f, 1.000000f, -3.000000f, 1.000000f, 0.180302f, 0.321030f, 1.000000f, -3.000000f, -1.000000f, 0.249697f, 0.321030f, 1.000000f, 0.000000f, -1.000000f, 0.249697f, 0.425123f, -1.000000f, 0.000000f, -1.000000f, 0.041511f, 0.425123f, -1.000000f, -0.000000f, 1.000000f, 0.110906f, 0.425123f, -1.000000f, 3.000000f, 1.000000f, 0.110906f, 0.529216f, -1.000000f, 0.000000f, -1.000000f, 0.041511f, 0.425123f, -1.000000f, 3.000000f, 1.000000f, 0.110906f, 0.529216f, -1.000000f, 3.000000f, -1.000000f, 0.041511f, 0.529216f, -1.000000f, -3.000000f, -1.000000f, 0.041511f, 0.321030f, -1.000000f, -0.000000f, 1.000000f, 0.110906f, 0.425123f, -1.000000f, 0.000000f, -1.000000f, 0.041511f, 0.425123f, -1.000000f, -3.000000f, -1.000000f, 0.041511f, 0.321030f, -1.000000f, -3.000000f, 1.000000f, 0.110906f, 0.321030f, -1.000000f, -0.000000f, 1.000000f, 0.110906f, 0.425123f, 1.000000f, -3.000000f, -1.000000f, 0.180302f, 0.251635f, 1.000000f, -3.000000f, 1.000000f, 0.180302f, 0.321030f, -1.000000f, -3.000000f, 1.000000f, 0.110906f, 0.321030f, 1.000000f, -3.000000f, -1.000000f, 0.180302f, 0.251635f, -1.000000f, -3.000000f, 1.000000f, 0.110906f, 0.321030f, -1.000000f, -3.000000f, -1.000000f, 0.110906f, 0.251635f, };
	readyTextureModel newArm(armVerts, playerTexture, true);
	armModel = newArm;
}

void player::startLeg() {
	vector<float> legVerts = { -1.000000f, -0.000000f, 1.000000f, 0.839335f, 0.203811f, 1.000000f, -0.000000f, 1.000000f, 0.908730f, 0.203811f, 1.000000f, 3.000000f, 1.000000f, 0.908730f, 0.307904f, -1.000000f, -0.000000f, 1.000000f, 0.839335f, 0.203811f, 1.000000f, 3.000000f, 1.000000f, 0.908730f, 0.307904f, -1.000000f, 3.000000f, 1.000000f, 0.839335f, 0.307904f, -1.000000f, -3.000000f, 1.000000f, 0.839335f, 0.099718f, 1.000000f, -0.000000f, 1.000000f, 0.908730f, 0.203811f, -1.000000f, -0.000000f, 1.000000f, 0.839335f, 0.203811f, -1.000000f, -3.000000f, 1.000000f, 0.839335f, 0.099718f, 1.000000f, -3.000000f, 1.000000f, 0.908731f, 0.099718f, 1.000000f, -0.000000f, 1.000000f, 0.908730f, 0.203811f, -1.000000f, 3.000000f, 1.000000f, 0.839335f, 0.307904f, 1.000000f, 3.000000f, 1.000000f, 0.908730f, 0.307904f, 1.000000f, 3.000000f, -1.000000f, 0.908730f, 0.377299f, -1.000000f, 3.000000f, 1.000000f, 0.839335f, 0.307904f, 1.000000f, 3.000000f, -1.000000f, 0.908730f, 0.377299f, -1.000000f, 3.000000f, -1.000000f, 0.839335f, 0.377299f, -1.000000f, 3.000000f, -1.000000f, 0.769940f, 0.307904f, 1.000000f, 3.000000f, -1.000000f, 0.700545f, 0.307904f, 1.000000f, 0.000000f, -1.000000f, 0.700545f, 0.203811f, -1.000000f, 3.000000f, -1.000000f, 0.769940f, 0.307904f, 1.000000f, 0.000000f, -1.000000f, 0.700545f, 0.203811f, -1.000000f, 0.000000f, -1.000000f, 0.769940f, 0.203811f, -1.000000f, 0.000000f, -1.000000f, 0.769940f, 0.203811f, 1.000000f, 0.000000f, -1.000000f, 0.700545f, 0.203811f, 1.000000f, -3.000000f, -1.000000f, 0.700545f, 0.099718f, -1.000000f, 0.000000f, -1.000000f, 0.769940f, 0.203811f, 1.000000f, -3.000000f, -1.000000f, 0.700545f, 0.099718f, -1.000000f, -3.000000f, -1.000000f, 0.769940f, 0.099718f, 1.000000f, -0.000000f, 1.000000f, 0.908730f, 0.203811f, 1.000000f, 0.000000f, -1.000000f, 0.978126f, 0.203811f, 1.000000f, 3.000000f, -1.000000f, 0.978126f, 0.307904f, 1.000000f, -0.000000f, 1.000000f, 0.908730f, 0.203811f, 1.000000f, 3.000000f, -1.000000f, 0.978126f, 0.307904f, 1.000000f, 3.000000f, 1.000000f, 0.908730f, 0.307904f, 1.000000f, -3.000000f, 1.000000f, 0.908731f, 0.099718f, 1.000000f, 0.000000f, -1.000000f, 0.978126f, 0.203811f, 1.000000f, -0.000000f, 1.000000f, 0.908730f, 0.203811f, 1.000000f, -3.000000f, 1.000000f, 0.908731f, 0.099718f, 1.000000f, -3.000000f, -1.000000f, 0.978126f, 0.099718f, 1.000000f, 0.000000f, -1.000000f, 0.978126f, 0.203811f, -1.000000f, 0.000000f, -1.000000f, 0.769940f, 0.203811f, -1.000000f, -0.000000f, 1.000000f, 0.839335f, 0.203811f, -1.000000f, 3.000000f, 1.000000f, 0.839335f, 0.307904f, -1.000000f, 0.000000f, -1.000000f, 0.769940f, 0.203811f, -1.000000f, 3.000000f, 1.000000f, 0.839335f, 0.307904f, -1.000000f, 3.000000f, -1.000000f, 0.769940f, 0.307904f, -1.000000f, -3.000000f, -1.000000f, 0.769940f, 0.099718f, -1.000000f, -0.000000f, 1.000000f, 0.839335f, 0.203811f, -1.000000f, 0.000000f, -1.000000f, 0.769940f, 0.203811f, -1.000000f, -3.000000f, -1.000000f, 0.769940f, 0.099718f, -1.000000f, -3.000000f, 1.000000f, 0.839335f, 0.099718f, -1.000000f, -0.000000f, 1.000000f, 0.839335f, 0.203811f, 1.000000f, -3.000000f, 1.000000f, 0.908731f, 0.099718f, -1.000000f, -3.000000f, 1.000000f, 0.839335f, 0.099718f, -1.000000f, -3.000000f, -1.000000f, 0.839335f, 0.030323f, 1.000000f, -3.000000f, 1.000000f, 0.908731f, 0.099718f, -1.000000f, -3.000000f, -1.000000f, 0.839335f, 0.030323f, 1.000000f, -3.000000f, -1.000000f, 0.908730f, 0.030323f, };
	readyTextureModel newLeg(legVerts, playerTexture, true);
	legModel = newLeg;	
}

void player::runAnimation(float multiplier){
	if (multiplier == 0.0f) {
		int upDown = 0;
		if (legRotationTwo.x > 0) {
			upDown = 1;
			if (!aiming && !researching) {
				armRotation.x -= (float)deltaTime * 3 * 40.0f;
				armRotationTwo.x += (float)deltaTime * 3 * 40.0f;
			}
			legRotation.x += deltaTime * 3 * 40.0f;
			legRotationTwo.x -= deltaTime * 3 * 40.0f;
		}
		if (legRotationTwo.x < 0) {
			upDown = -1;
			if (!aiming && !researching) {
				armRotation.x += (float)deltaTime * 3 * 40.0f;
				armRotationTwo.x -= (float)deltaTime * 3 * 40.0f;
			}
			legRotation.x -= deltaTime * 3 * 40.0f;
			legRotationTwo.x += deltaTime * 3 * 40.0f;
		}
		if (upDown == -1) {
			if (legRotationTwo.x > 0) {
				if (!aiming && !researching) {
					armRotation.x = 0.0f;
					armRotationTwo.x = 0.0f;
				}
				legRotation.x = 0.0f;
				legRotationTwo.x = 0.0f;
			}
		}
		if (upDown == 1) {
			if (legRotationTwo.x < 0) {
				if (!aiming && !researching) {
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
		if (!aiming && !researching) {
			armRotation.x += (float)deltaTime * 3 * multiplier;
			armRotationTwo.x -= (float)deltaTime * 3 * multiplier;
		}
		legRotation.x -= deltaTime * 2.5f * multiplier;
		legRotationTwo.x += deltaTime * 2.5f * multiplier;
		
		if (legRotationTwo.x >= 30.0f) {
			finishedFirst = true;
		}
	}
	if (!finishedSecond && finishedFirst) {
		if (!aiming && !researching) {
			armRotation.x -= (float)deltaTime * 3 * multiplier;
			armRotationTwo.x += (float)deltaTime * 3 * multiplier;
		}
		legRotation.x += deltaTime * 2.5f * multiplier;
		legRotationTwo.x -= deltaTime * 2.5f * multiplier;

		if (legRotationTwo.x <= -30.0f) {
			finishedSecond = true;
		}
	}
	if (finishedFirst && finishedSecond) {
		if (!aiming && !researching) {
			armRotation.x += (float)deltaTime * 3 * multiplier;
			armRotationTwo.x -= (float)deltaTime * 3 * multiplier;
		}
		legRotation.x -= deltaTime * 2.5f * multiplier;
		legRotationTwo.x += deltaTime * 2.5f * multiplier;

		if (legRotationTwo.x >= 0) {
			if (!aiming && !researching) {
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
	vector<float> torsoVerts = { -2.002170f, -3.000000f, 1.000000f, 0.465780f, 0.391188f, 1.997840f, -3.000000f, 1.000000f, 0.607623f, 0.391188f, 1.997840f, 3.000000f, 1.000000f, 0.607623f, 0.603950f, -2.002170f, -3.000000f, 1.000000f, 0.465780f, 0.391188f, 1.997840f, 3.000000f, 1.000000f, 0.607623f, 0.603950f, -2.002170f, 3.000000f, 1.000000f, 0.465781f, 0.603950f, -2.002170f, 3.000000f, 1.000000f, 0.465781f, 0.603950f, 1.997840f, 3.000000f, 1.000000f, 0.607623f, 0.603950f, 2.002170f, 3.000000f, -1.000000f, 0.607776f, 0.674871f, -2.002170f, 3.000000f, 1.000000f, 0.465781f, 0.603950f, 2.002170f, 3.000000f, -1.000000f, 0.607776f, 0.674871f, -1.997840f, 3.000000f, -1.000000f, 0.465934f, 0.674871f, -1.997840f, 3.000000f, -1.000000f, 0.820385f, 0.603950f, 2.002170f, 3.000000f, -1.000000f, 0.678543f, 0.603950f, 2.002170f, -3.000000f, -1.000000f, 0.678543f, 0.391188f, -1.997840f, 3.000000f, -1.000000f, 0.820385f, 0.603950f, 2.002170f, -3.000000f, -1.000000f, 0.678543f, 0.391188f, -1.997840f, -3.000000f, -1.000000f, 0.820385f, 0.391188f, -1.997840f, -3.000000f, -1.000000f, 0.465934f, 0.320267f, 2.002170f, -3.000000f, -1.000000f, 0.607776f, 0.320268f, 1.997840f, -3.000000f, 1.000000f, 0.607623f, 0.391188f, -1.997840f, -3.000000f, -1.000000f, 0.465934f, 0.320267f, 1.997840f, -3.000000f, 1.000000f, 0.607623f, 0.391188f, -2.002170f, -3.000000f, 1.000000f, 0.465780f, 0.391188f, 1.997840f, -3.000000f, 1.000000f, 0.607623f, 0.391188f, 2.002170f, -3.000000f, -1.000000f, 0.678543f, 0.391188f, 2.002170f, 3.000000f, -1.000000f, 0.678543f, 0.603950f, 1.997840f, -3.000000f, 1.000000f, 0.607623f, 0.391188f, 2.002170f, 3.000000f, -1.000000f, 0.678543f, 0.603950f, 1.997840f, 3.000000f, 1.000000f, 0.607623f, 0.603950f, -1.997840f, -3.000000f, -1.000000f, 0.394860f, 0.391188f, -2.002170f, -3.000000f, 1.000000f, 0.465780f, 0.391188f, -2.002170f, 3.000000f, 1.000000f, 0.465781f, 0.603950f, -1.997840f, -3.000000f, -1.000000f, 0.394860f, 0.391188f, -2.002170f, 3.000000f, 1.000000f, 0.465781f, 0.603950f, -1.997840f, 3.000000f, -1.000000f, 0.394860f, 0.603950f, };
	readyTextureModel newTorso(torsoVerts, playerTexture, true);
	torsoModel = newTorso;	
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
		if (!equippingReloading && allWeapons[currentWeapon].currentAmmo > 0) {
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

		if(crouching){
			bulletRot.z += 45.0f;
		}

		vec3 bulletPos = position;
		bulletPos.z += 0.7f * sin(radians(rotation.y));
		bulletPos.y += 0.25f;
		bulletPos.x += 0.7f * -cos(radians(rotation.y));

		createBullet(bulletPos, bulletRot, allWeapons[currentWeapon].bulletDamage);

		canShoot = false;
		currentDelay = shotDelays[currentWeapon];
	}
}

void player::equipReloadAnimation(float multiplier) {
	canShoot = false;
    float maxSpeed = (WEAPON_RELOAD_MAX_ROT * 6.0f) / 0.5f;
    int maxStep = 5;
    if(multiplier > maxSpeed){
        multiplier = (WEAPON_RELOAD_MAX_ROT * 4.0f) / 0.5f;
        maxStep = 3;
    }
	if (equippingStep == 0) {
		totalGoneUpEquipping += deltaTime * multiplier;
        armRotationTwo.x = playerPitch + 90.0f;
        armRotationTwo.x += totalGoneUpEquipping;
		if (totalGoneUpEquipping > WEAPON_RELOAD_MAX_ROT) {
            equippingStep = 1;
		}
	}
	if (equippingStep == 1) {
		totalGoneUpEquipping -= deltaTime * multiplier;
        armRotationTwo.x = playerPitch + 90.0f;
        armRotationTwo.x += totalGoneUpEquipping;
		if (totalGoneUpEquipping < 0.0f) {
            equippingStep = 2;
			resetArms();
			totalGoneUpEquipping = 0.0f;
		}
	}
    if(equippingStep == 2){
		armRotationTwo.x = playerPitch + 90.0f;
        totalGoneUpEquipping += deltaTime * multiplier;
        armRotationTwo.z = -45.0f;
        armRotationTwo.z += totalGoneUpEquipping;
        if(totalGoneUpEquipping > WEAPON_RELOAD_MAX_ROT){
            equippingStep = 3;
            armRotationTwo.z = WEAPON_RELOAD_MAX_ROT;
        }
    }
    if(equippingStep == 3){
		armRotationTwo.x = playerPitch + 90.0f;
        totalGoneUpEquipping -= deltaTime * multiplier;
        armRotationTwo.z = -45.0f;
        armRotationTwo.z += totalGoneUpEquipping;
        if(totalGoneUpEquipping < 0.0f){
            totalGoneUpEquipping = 0.0f;
            equippingStep = 4;
            if(maxStep == 3){
                equippingReloading = false;
                totalGoneUpEquipping = 0.0f;
                equippingStep = 0;
                return;
            }
        }
    }
    if(equippingStep == 4){
        totalGoneUpEquipping += deltaTime * multiplier;
        armRotationTwo.x = playerPitch + 90.0f;
        armRotationTwo.x -= totalGoneUpEquipping;
        if (totalGoneUpEquipping > WEAPON_RELOAD_MAX_ROT) {
            equippingStep = 5;
        }
    }
    if(equippingStep == 5){
        totalGoneUpEquipping -= deltaTime * multiplier;
        armRotationTwo.x = playerPitch + 90.0f;
        armRotationTwo.x -= totalGoneUpEquipping;
        if(totalGoneUpEquipping < 0.0f){
            equippingReloading = false;
            totalGoneUpEquipping = 0.0f;
            equippingStep = 0;
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

void player::monsterColliders(){
	vec2 playerFloorPos = vec2(position.x, position.z);
	float colliderDistance = 3.0f;

	int mCount = allMonsters.size();
	for(int m = 0; m < mCount; m++){
		vec3 pos = allMonsters[m].position;
		vec2 floorPos = vec2(pos.x, pos.z);

		float bearing = bearingTwo(playerFloorPos, floorPos);
		float distance = glm::distance(playerFloorPos, floorPos);
		
		if(!allMonsters[m].attacking){
			if(distance < colliderDistance){
				pos.x = position.x + sin(radians(bearing)) * colliderDistance;
				pos.z = position.z + cos(radians(bearing)) * colliderDistance;
				allMonsters[m].position = pos;
			}
		}
	}
}

void player::researchAnimation(float multiplier) {
	researcherModel.modelTexture = researcherTextures[researchType];
	vec3 researcherPosition = position;
	researcherPosition.x = position.x + sin(radians(armRotation.x)) * -sin(radians(rotation.y)) * 1.2f;
	researcherPosition.y = (position.y - 0.6f) + ((degreesClamp(armRotation.x) / 90.0f) * 1.2f);
	researcherPosition.z = position.z + sin(radians(armRotation.x)) * -cos(radians(rotation.y)) * 1.2f;

	if (!researching) {
		return;
	}

	float stickScale = ((degreesClamp(armRotation.x) / 90.0f)) * ((glm::clamp(researchStep - 1, 0, 20))) * 0.5f;
	if (researchStep == researchTicks + 1) {
		stickScale = 0.0f;
	}

	if (multiplier == 0.0f || !researching) {
		armRotation.x -= deltaTime * 120.0f;
		armRotationTwo.x -= deltaTime * 120.0f;
		if (armRotation.x < 0.0f) {
			armRotation.x = 0.0f;
			armRotationTwo.x = 0.0f;
			researchStep = 0;
			researching = false;
		}
		researcherModel.render(modelMatrix(researcherPosition, vec3(0.0f, rotation.y - 90.0f, -armRotation.x + 90.0f), vec3(1.0f)),
			true, vec3(1.0f), cameraThirdPos.y < 0.0f, WorldGeneration.waterMultiplyColour);
		researcherStick.render(modelMatrix(vec3(0.0f, -stickScale, 0.0f), vec3(0.0f, rotation.y - 90.0f, 0.0f), vec3(0.05f, stickScale, 0.05f), true, 
			researcherPosition, vec3(armRotation.x - 90.0f, 0.0f, 0.0f)), true, vec3(1.0f), cameraThirdPos.y < 0.0f, WorldGeneration.waterMultiplyColour);
		return;
	}
	if (researchStep == 0) {
		armRotation.x += deltaTime * multiplier;
		armRotationTwo.x += deltaTime * multiplier;
		if (degreesClamp(armRotation.x) > 90.0f) {
			armRotation.x = 90.0f;
			armRotationTwo.x = 90.0f;
			researchStep = 1;
		}
	}
	if (researchStep > 0 && researchStep < researchTicks + 1) {
		if (!finishedResearchTick) { // odd
			armRotationTwo.x += deltaTime * multiplier;
			if (armRotationTwo.x > 120.0f) {
				finishedResearchTick = true;
			}
		}
		if (finishedResearchTick) { // even
			armRotationTwo.x -= deltaTime * multiplier;
			if (armRotationTwo.x < 90.0f) {
				finishedResearchTick = false;
				researchStep++;
			}
		}
	}
	if (researchStep == researchTicks + 1) {
		armRotation.x -= deltaTime * multiplier;
		armRotationTwo.x -= deltaTime * multiplier;
		if (armRotation.x < 0.0f) {
			armRotation.x = 0.0f;
			armRotationTwo.x = 0.0f;

			researching = false;
			researchStep = 0;

			bool foundData = false;
			string dataType = "";
			if (researchType == WATER_RESEARCH) {
				dataType = "water";
				if (randomInt(1, 2) == 1) {
					foundData = true;
				}
			}
			if (researchType == TERRAIN_RESEARCH || researchType == AIR_GRAVITY_RESEARCH) {
				if (researchType == TERRAIN_RESEARCH) {
					dataType = "terrain";
				}
				if (researchType == AIR_GRAVITY_RESEARCH) {
					dataType = "air";
				}
				if (randomInt(1, 3) == 1) {
					foundData = true;
				}
			}

			if (foundData) {
				alert(name + "found: " + dataType + " data");
				WorldGeneration.dataPoints++;
				allTexts[researchStatusText].displayedText = "Data Points: " + to_string(WorldGeneration.dataPoints);
			}
		}
	}
	// render
	researcherModel.render(modelMatrix(researcherPosition, vec3(0.0f, rotation.y - 90.0f, -armRotation.x + 90.0f), vec3(1.0f)), 
		true, vec3(1.0f), cameraThirdPos.y < 0.0f, WorldGeneration.waterMultiplyColour);
	researcherStick.render(modelMatrix(vec3(0.0f, -stickScale, 0.0f), vec3(0.0f, rotation.y - 90.0f, 0.0f), vec3(0.05f, stickScale, 0.05f), true,
		researcherPosition, vec3(armRotation.x - 90.0f, 0.0f, 0.0f)), true, vec3(1.0f), cameraThirdPos.y < 0.0f, WorldGeneration.waterMultiplyColour);
}
