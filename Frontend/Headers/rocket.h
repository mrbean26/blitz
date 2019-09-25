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

extern float doorRot;
vec4 rocketColliders(vec3 original, bool player = false);
vec3 rocketCameraColliders(vec3 original, vec3 playerPos);
void renderRocket();

void openDoorAnimation(float multiplier);
void openDoor();

void rocketBegin();
void rocketMainloop();

#endif