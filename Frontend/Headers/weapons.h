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

#define PISTOL_DELAY 0.25f
#define PISTOL_SHOT_DELAY 0.5f
#define PISTOL_DAMAGE 22.0f
#define PISTOL_AMMO 8

#define RIFLE_DELAY 0.75f
#define RIFLE_SHOT_DELAY 0.25f
#define RIFLE_DAMAGE 16.0f
#define RIFLE_AMMO 25

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

	float equipTime, shotDelay = 0.0f;

	float shotDelayCurrent = 0.0f;

	int currentAmmo = 0, maxAmmo = 0;
	float bulletDamage = 0.0f;
};

struct bullet {
	vec3 position;
	vec3 rotation;
	float damage;
};

void bulletColliders();
void removeBullet(int index);

extern vector<weapon> allWeapons;
int createWeapon(vector<float> vertices, float activateDelay,
	int maxAmmo, float shotDelay, float damage);

extern vector<bullet> allBullets;
int createBullet(vec3 position, vec3 rotation, float damage);

void moveBullets();

extern int basicPistol, basicRifle;

#endif
