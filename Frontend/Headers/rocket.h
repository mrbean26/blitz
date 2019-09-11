#pragma once

#ifndef ROCKET_H
#define ROCKET_H

#include <glm.hpp>
using namespace glm;

vec3 rocketColliders(vec3 original);
void renderRocket();

void rocketBegin();
void rocketMainloop();

#endif