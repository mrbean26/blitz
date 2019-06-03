#pragma once

#ifndef WEAPONS_H
#define WEAPONS_H

#include "frontend.h"

#include <glew.h>
#include <glfw3.h>
#include <vector>
using namespace std;

#include <glm.hpp>
using namespace glm;

void weaponsBegin();
void weaponsMainloop();

void startWeapons();

class weapon {
public:
	weapon(vector<float> vertices = {});
	void render(mat4 model);

	GLuint VAO, VBO;
	GLuint vertCount;
};

struct bullet {
	vec3 position;
	vec3 rotation;
};

extern vector<weapon> allWeapons;
int createWeapon(vector<float> vertices);

extern vector<bullet> allBullets;
int createBullet(vec3 position, vec3 rotation);

void moveBullets();

extern int basicPistol;

#endif
