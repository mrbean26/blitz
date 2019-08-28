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

#include "model.h"
#include "texture.h"

#define WALK_SPEED 6.0f
#define RUN_SPEED 10.0f
#define BACKWARD_SPEED -4.0f
#define JUMP_SPEED 12.0f
#define JUMP_HEIGHT 8.0f
#define PLAYER_ROTATE_SPEED 140.0f
#define LEG_LENGTH 2.16f
#define CROUCH_CRATER_ADD 0.5f
#define PITCH_MIN -80.0f
#define PITCH_MAX 80.0f
#define DYING_ROTATE_SPEED 240.0f
#define PLAYER_CROUCH_SPEED 240.0f
#define PLAYER_RESPAWN_KILL_RADIUS 10.0f
#define WEAPON_RELOAD_MAX_ROT 7.5f
#define WEAPON_RELOAD_SPEED 0.5f

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

float degreesDistance(float v1, float v2);

class player {
public:
	bool active = false, canMove = true, canMoveCamera = true;
	void begin();
	void mainloop();

	vec3 position, rotation = vec3(0.0f), scale = vec3(0.8f);
	vec3 velocity = vec3(0.0f);

	void deleteMemory();

	int currentWeapon = 0;
	bool canShoot = true;
	bool aiming = false;
	bool changingCrouch = false;
	bool crouching = false;
	void shoot();

	float health = 100.0f;
	bool dead = false;
	bool respawning = false;
	vec3 multiplyColour = vec3(1.0f);
	float redDelay = 0.0f;

	float movementDelay = 4.0f;

	bool personRotating = false;
	bool personMoving = false;
	float personRotation = 0.0f;
	float movingTime = 0.0f;
	vec2 movingStart = vec2(0.0f);

	bool equippingReloading = false;
	float reloadSpeed;
	int personType = 0;
    int personBuildingInside = 0;

	vec3 armPosition=vec3(-0.72f, -0.55f, 0.0f), armRotation = vec3(0.0f), armScale = vec3(0.3f);
	vec3 armPositionTwo=vec3(0.72f, -0.55f, 0.0f), armRotationTwo = vec3(0.0f), armScaleTwo = vec3(0.3f);

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
	texture playerTexture;

	void startCharacterVertices();

	void startHead();
	readyTextureModel headModel;
	vec3 headPosition = vec3(0.0f, 0.0f, 0.0f), headRotation = vec3(0.0f, 180.0f, 0.0f), headScale = vec3(0.3f, 0.3f, 0.3f);

	void startArm(); // includes hand
	readyTextureModel armModel;

	void startTorso();
	readyTextureModel torsoModel;
	vec3 torsoPosition = vec3(0.0f), torsoRotation = vec3(0.0f, 180.0f, 0.0f), torsoScale = vec3(0.3f);

	void startLeg(); // includes foot
	readyTextureModel legModel;

	vec3 legPosition = vec3(-0.2f, -0.72f, 0.0f), legRotation = vec3(0.0f), legScale = vec3(0.298f);
	vec3 legPositionTwo = vec3(0.2f, -0.72f, 0.0f), legRotationTwo = vec3(0.0f), legScaleTwo = vec3(0.298f);

	// animations
	bool walking = false, running = false;
	bool finishedFirst = false, finishedSecond = false;
	float movingMultiplier = 0.0f; // speed at which to move limbs in runAnimation()
	void runAnimation(float multiplier);
	void crouchMoveAnimation(float multiplier);

    int equippingStep = 0;
	float totalGoneUpEquipping = 0.0f;

	void equipReloadAnimation(float multiplier);
};

#endif
