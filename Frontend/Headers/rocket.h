#pragma once

#ifndef ROCKET_H
#define ROCKET_H

#include <glm.hpp>
using namespace glm;

#define ROCKET_COLLIDER_DISTANCE 4.25f
#define ROCKET_OPEN_DISTANCE 20.0f
#define ROCKET_OPEN_SPEED 30.0f
#define ROCKET_DOOR_OPEN_ROT 140.0f
#define ROCKET_DOOR_Z_COLLIDER_ADDITION 0.05f
#define ROCKET_DOOR_COLLIDER_LENGTH 13.25f
#define ROCKET_DOOR_WIDTH 1.8f

#define SLIDE_END_POS_FOR_COLLIDER -3.85f
#define ROCKET_CAMERA_WORLD_DISTANCE 17.5f
#define SLIDE_TOP_HEIGHT 5.5f
#define FIRE_SPAWN_RATE 8
#define FIRE_ROT_RANDOMNESS 10.0f

void createParticle(vec3 position, vec3 rotation);
void fireBegin();
void fireMainloop();
void spawnParticles();

class fireParticle {
public:
	void move();
	void render();

	vec3 position = vec3(0.0f);
	vec3 rotation = vec3(0.0f);
	vec3 scale = vec3(0.15f);

	vec3 colour = vec3(1.0f, 0.0f, 0.0f);
	float lifetimeLeft = 1.0f;
	float dyingSpeed = 0.33f;
	float speed = 6.0f;

	bool active = true;
};

extern float doorRot;
vec4 rocketColliders(vec3 original, bool player = false);
vec3 rocketCameraColliders(vec3 original, vec3 playerPos);
void renderRocket();

void openDoorAnimation(float multiplier);
void openDoor();

void rocketBegin();
void rocketMainloop();

#endif