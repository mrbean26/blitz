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
private:
	void movement();
	void cameraMovement();
	void renderPlayer();

	void startCharacterVertices();
	vec3 playerPosition, playerRotation, playerScale = vec3(1.0f, 1.0f, 1.0f);

	void startHead();
	GLuint headVAO, headVBO;
	vec3 headPosition, headRotation, headScale = vec3(1.0f, 1.0f, 1.0f);

	void startArm(); // includes hand
	GLuint armVAO, armVBO;
	vec3 armPosition, armRotation, armScale = vec3(1.0f, 1.0f, 1.0f);
	vec3 armPositionTwo, armRotationTwo, armScaleTwo = vec3(1.0f, 1.0f, 1.0f);

	void startTorso();
	GLuint torsoVAO, torsoVBO;
	vec3 torsoPosition, torsoRotation, torsoScale = vec3(1.0f, 1.0f, 1.0f);

	void startLeg(); // includes foot
	GLuint legVAO, legVBO;
	vec3 legPosition, legRotation, legScale = vec3(1.0f, 1.0f, 1.0f);
	vec3 legPositionTwo, legRotationTwo, legScaleTwo = vec3(1.0f, 1.0f, 1.0f);
};

#endif