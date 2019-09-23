#pragma once

#ifndef ROCKET_H
#define ROCKET_H

#include <glm.hpp>
using namespace glm;

vec4 rocketColliders(vec3 original, bool player = false);
vec3 rocketCameraColliders(vec3 original, vec3 playerPos);
void renderRocket();

void openDoorAnimation(float multiplier);
void openDoor();

void rocketBegin();
void rocketMainloop();

#endif