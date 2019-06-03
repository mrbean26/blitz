#pragma once

#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <vector>
#include <glew.h>
#include <glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

extern float lowestY;
extern GLuint playerShader;
void startPlayerShader();

vector<vec3> colourVector(int size, vec3 colour, float multiplier = 0.2f);
extern vector<string> inputLines;

extern int continueButton, exitButton;
extern bool paused;
void startPauseUI();
void pauseUIInteraction();

vec3 cameraBuildingCollisions(vec3 original);
void exitToMenus();

class player {
public:
	bool active = false, canMove = true;
	void begin();
	void mainloop();

	vec3 position, rotation, scale = vec3(1.0f, 1.0f, 1.0f);
	vec3 velocity = vec3(0.0f);

	float headLookAtY = 0.0f;

	void deleteMemory();

	int currentWeapon = 0;
	bool canShoot = true;
	void shoot();
private:
	void movement();

	void collisions();

	bool jumping = false;
	float jumpVelocity;

	void cameraMovement();
	float sensitivity = 0.3f;
	
	bool aiming = false;
	void startLaserForBullets();
	GLuint laserVAO, laserVBO;

	void renderPlayer();

	void startCharacterVertices();

	void startHead();
	GLuint headVAO, headVBO;
	vec3 headPosition = vec3(0.0f, 1.2f, 0.0f), headRotation, headScale = vec3(0.9f, 0.9f, 0.9f);

	void startArm(); // includes hand
	GLuint armVAO, armVBO;
	vec3 armPosition=vec3(-0.7f, -0.55f, 0.0f), armRotation, armScale = vec3(0.4f, 0.4f, 0.4f);
	vec3 armPositionTwo=vec3(0.7f, -0.55f, 0.0f), armRotationTwo, armScaleTwo = vec3(0.4f, 0.4f, 0.4f);

	void startTorso();
	GLuint torsoVAO, torsoVBO;
	vec3 torsoPosition, torsoRotation, torsoScale = vec3(0.6f, 0.6f, 0.6f);

	void startLeg(); // includes foot
	GLuint legVAO, legVBO;
	vec3 legPosition = vec3(-0.25f, -0.775f, 0.0f), legRotation, legScale = vec3(0.3f, 0.3f, 0.3f);
	vec3 legPositionTwo = vec3(0.25f, -0.775f, 0.0f), legRotationTwo, legScaleTwo = vec3(0.3f, 0.3f, 0.3f);

	// animations
	bool walking = false, running = false;
	bool finishedFirst = false, finishedSecond = false;
	float movingMultiplier = 0.0f; // speed at which to move limbs in runAnimation()
	void runAnimation(float multiplier);
};

#endif