#pragma once

#ifndef PLAYER_H
#define PLAYER_H

#include <glew.h>
#include <glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

extern GLuint playerShader;
void startPlayerShader();

class player {
public:
	bool active = false;
	void begin();
	void mainloop();

	vec3 playerPosition, playerRotation, playerScale = vec3(1.0f, 1.0f, 1.0f);

	float headLookAtY = 0.0f;
private:
	void movement();

	void cameraMovement();
	float sensitivity = 0.3f;

	void renderPlayer();

	void startCharacterVertices();

	void startHead();
	GLuint headVAO, headVBO;
	vec3 headPosition = vec3(0.0f, 1.2f, 0.0f), headRotation, headScale = vec3(0.9f, 0.9f, 0.9f);

	void startArm(); // includes hand
	GLuint armVAO, armVBO;
	vec3 armPosition=vec3(-0.7f, 0.0f, 0.0f), armRotation, armScale = vec3(0.4f, 0.4f, 0.4f);
	vec3 armPositionTwo=vec3(0.7f, 0.0f, 0.0f), armRotationTwo, armScaleTwo = vec3(0.4f, 0.4f, 0.4f);

	void startTorso();
	GLuint torsoVAO, torsoVBO;
	vec3 torsoPosition, torsoRotation, torsoScale = vec3(0.6f, 0.6f, 0.6f);

	void startLeg(); // includes foot
	GLuint legVAO, legVBO;
	vec3 legPosition = vec3(-0.25f, -1.55f, 0.0f), legRotation, legScale = vec3(0.3f, 0.3f, 0.3f);
	vec3 legPositionTwo = vec3(0.25f, -1.55f, 0.0f), legRotationTwo, legScaleTwo = vec3(0.3f, 0.3f, 0.3f);
};

#endif