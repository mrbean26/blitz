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
void startPlayerUI();
void pauseUIInteraction();

vec3 cameraBuildingCollisions(vec3 original);
void exitToMenus();

class player {
public:
	bool active = false, canMove = true, canMoveCamera = true;
	void begin();
	void mainloop();

	vec3 position, rotation, scale = vec3(1.0f, 1.0f, 1.0f);
	vec3 velocity = vec3(0.0f);

	float headLookAtY = 0.0f;

	void deleteMemory();

	int currentWeapon = 0;
	bool canShoot = true;
	bool aiming = false;
	void shoot();

	float health = 100.0f;
	bool dead = false;
	bool respawning = false;
	vec3 multiplyColour = vec3(1.0f);
	float redDelay = 0.0f;

	bool equippingReloading = false;
	float reloadSpeed;
private:
	void respawn();
	void movement();
	void resetArms();
	void reload();

	void collisions();
	void monsterColliders();

	bool jumping = false;
	float jumpVelocity;

	void cameraMovement();
	float sensitivity = 0.3f;
	
	
	void startLaserForBullets();
	GLuint laserVAO, laserVBO;
	vec3 laserColour = vec3(1.0f, 0.0f, 0.0f);
	float currentDelay = 0.0f;

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

	bool finishedEquipFirst = false;
	float totalGoneUpEquipping = 0.0f;

	void equipReloadAnimation(float multiplier);
};

#endif