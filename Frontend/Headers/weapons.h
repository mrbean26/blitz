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

extern vector<int> currentWeapons;
void startWeapons();
void getCurrentWeapons();

void startWeaponUI();
void weaponUI();
extern bool weaponSelectOpen;
extern int weaponSelectImage, weaponSelectCornerOne, weaponSelectCornerTwo, weaponSelectCornerThree, weaponSelectCornerFour,
	weaponIconOne, wepaonIconTwo, weaponIconThree, weaponIconFour;

class weapon {
public:
	weapon(vector<float> vertices = {}, float openTime = 0.0f, float damage = 0.0f);
	void render(mat4 model);

	GLuint VAO, VBO;
	GLuint vertCount;

	float equipTime, shotDelay;

	float shotDelayCurrent;

	int currentAmmo, maxAmmo;
	float bulletDamage = 0.0f;
};

struct bullet {
	vec3 position;
	vec3 rotation;
	float damage;
};

void removeBullet(int index);

extern vector<weapon> allWeapons;
int createWeapon(vector<float> vertices, float activateDelay,
	int maxAmmo, float shotDelay, float damage);

extern vector<bullet> allBullets;
int createBullet(vec3 position, vec3 rotation, float damage);

void moveBullets();

extern int basicPistol, basicRifle;

#endif
