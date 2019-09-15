#pragma once

#ifndef ROCKET_H
#define ROCKET_H

#include <glm.hpp>
using namespace glm;

vec3 rocketColliders(vec3 original);
void renderRocket();

void openDoorAnimation(float multiplier);
void openDoor();

void rocketBegin();
void rocketMainloop();

#endif