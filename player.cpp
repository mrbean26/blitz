#include "player.h"
#include "shader.h"
#include "display.h"
#include "worldGeneration.h"
#include "backend.h"

#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <iostream>
#include <vector>
using namespace std;

vector<vec3> colourVector(int size, vec3 colour) {
	vector<vec3> newVector;
	for (int i = 0; i < size; i++) {
		int currentSize = newVector.size();
		newVector.resize(currentSize + 1);
		newVector[currentSize] = colour + colourDifference(0.2f);
	}
	return newVector;
}

GLuint playerShader;
vector<string> inputLines;
void player::begin(){
	startCharacterVertices();
	playerView = true;
	inputLines = readLines("assets/saves/inputs.save");
}

void player::mainloop(){
	if (!active) { return; }
	renderPlayer();
	movement();
	cameraMovement();
	collisions();
}

float startedLowestY = -999.0f;
void player::movement(){
	if (!canMove) { return; }
	int forwardKey = stoi(inputLines[0]);
	int leftKey = stoi(inputLines[1]);
	int backKey = stoi(inputLines[2]);
	int rightKey = stoi(inputLines[3]);
	
	int sprintKey = stoi(inputLines[7]);
	int jumpKey = stoi(inputLines[9]);

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
		if (position.y < lowestY + 0.05f && jumpVelocity < 0) {
			jumpVelocity = 0.0f;
			jumping = false;
		}
		if (lowestY > startedLowestY) {
			if (position.y < lowestY + 0.05f) {
				jumpVelocity = 0.0f;
				jumping = false;
			}
			
		}
		if (lowestY < startedLowestY) {
			if (position.y < lowestY + 0.05f) {
				jumpVelocity = 0.0f;
				jumping = false;
			}
		}
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
	// flat terrain collisions
	float legPos = position.y - 2.3f;
	if (legPos < 0 && !inMountain && !jumping) { position.y += -legPos; }
	if (!inMountain) { lowestY = 2.2f; }
	if (!cameraInMountain) { lowestCameraY = 0.5f; }
	// distance outside of area scale
	position.x = clamp(position.x, 0.0f, currentPlanetScale.x);
	position.z = clamp(position.z, -currentPlanetScale.y, 0.0f);
}

bool lastFrameMove = true;
void player::cameraMovement(){
	if (!canMove) { 
		lastFrameMove = false;
		return; 
	}
	vec2 mouseDiffer = mouseDifferences();
	if (lastFrameMove) {
		playerYaw -= mouseDiffer.x * sensitivity;
		playerPitch += mouseDiffer.y * sensitivity;
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
	
	glUseProgram(playerShader);
	for (int i = 0; i < 6; i++) {
		
		vec3 combinedScale = scale * scales[i];

		setMat4(playerShader, "model", modelMatrix(positions[i], rotations[i], combinedScale, 
			true, parentPositions[i], rotation));
		setMat4(playerShader, "view", viewMatrix());
		setMat4(playerShader, "projection", projectionMatrix());

		glBindVertexArray(vaos[i]);
		glDrawArrays(GL_TRIANGLES, 0, vertCounts[i]);
	}
}

void player::startCharacterVertices(){
	startHead();
	startArm();
	startTorso();
	startLeg();
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
		if (armRotation.x > 0) {
			upDown = 1;
			armRotation.x -= (float)deltaTime * 3 * 40.0f;
			armRotationTwo.x += (float) deltaTime * 3 * 40.0f;
			legRotation.x += (float) deltaTime * 3 * 40.0f;
			legRotationTwo.x -= (float) deltaTime * 3 * 40.0f;
		}
		if (armRotation.x < 0) {
			upDown = -1;
			armRotation.x += (float) deltaTime * 3 * 40.0f;
			armRotationTwo.x -= (float) deltaTime * 3 * 40.0f;
			legRotation.x -= (float) deltaTime * 3 * 40.0f;
			legRotationTwo.x += (float) deltaTime * 3 * 40.0f;
		}
		if (upDown == -1) {
			if (armRotation.x > 0) {
				armRotation.x = 0.0f;
				armRotationTwo.x = 0.0f;
				legRotation.x = 0.0f;
				legRotationTwo.x = 0.0f;
			}
		}
		if (upDown == 1) {
			if (armRotation.x < 0) {
				armRotation.x = 0.0f;
				armRotationTwo.x = 0.0f;
				legRotation.x = 0.0f;
				legRotationTwo.x = 0.0f;
			}
		}
		return;
	}
	if (!finishedFirst) {
		armRotation.x += (float) deltaTime * 3 * multiplier;
		armRotationTwo.x -= (float) deltaTime * 3 * multiplier;
		legRotation.x -= (float) deltaTime * 2.5f * multiplier;
		legRotationTwo.x += (float) deltaTime * 2.5f * multiplier;
		
		if (armRotation.x >= 30.0f) {
			finishedFirst = true;
		}
	}
	if (!finishedSecond && finishedFirst) {
		armRotation.x -= (float) deltaTime * 3 * multiplier;
		armRotationTwo.x += (float) deltaTime * 3 * multiplier;
		legRotation.x += (float) deltaTime * 2.5f * multiplier;
		legRotationTwo.x -= (float) deltaTime * 2.5f * multiplier;

		if (armRotation.x <= -30.0f) {
			finishedSecond = true;
		}
	}
	if (finishedFirst && finishedSecond) {
		armRotation.x += (float) deltaTime * 3 * multiplier;
		armRotationTwo.x -= (float) deltaTime * 3 * multiplier;
		legRotation.x -= (float) deltaTime * 2.5f * multiplier;
		legRotationTwo.x += (float) deltaTime * 2.5f * multiplier;

		if (armRotation.x >= 0) {
			armRotation.x = 0.0f;
			armRotationTwo.x = 0.0f;
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
