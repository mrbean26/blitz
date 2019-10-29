#pragma once

#ifndef ROCKET_H
#define ROCKET_H

#include "structures.h"
#include <glm.hpp>
using namespace glm;

#define ROCKET_COLLIDER_DISTANCE 4.25f
#define ROCKET_OPEN_DISTANCE 20.0f
#define ROCKET_OPEN_SPEED 30.0f
#define ROCKET_DOOR_OPEN_ROT 140.0f
#define ROCKET_DOOR_Z_COLLIDER_ADDITION 0.05f
#define ROCKET_DOOR_COLLIDER_LENGTH 13.25f
#define ROCKET_DOOR_WIDTH 1.8f
#define HOLDER_ROTATION_MULTIPLIER 22.5f
#define ROCKET_CANNOT_DRIVE_TIME 1.0f

#define ROCKET_EXPLODE_TIME 2.5f
#define ROCKET_EXPLODE_QUALITY 36

#define SLIDE_END_POS_FOR_COLLIDER -3.85f
#define ROCKET_CAMERA_WORLD_DISTANCE 17.5f
#define SLIDE_TOP_HEIGHT 5.5f
#define FIRE_SPAWN_RATE 3
#define FIRE_ROT_RANDOMNESS 50.0f
#define PARTICLE_SCALE 0.4f

#define ROCKET_MAX_ROTATE_SPEED 60.0f
#define ROCKET_ROTATE_SPEED_INCREASE 50.0f
#define ROTATION_DECREASE_MULTIPLIER 6.0f
#define ROCKET_SPEED_INCREASE 20.0f
#define ROCKET_SPEED_MAX 60.0f
#define ROCKET_ROTATION_CUTOFF 5.0f
#define PARTICLE_DYING_SPEED 2.5f
#define PARTICLE_LIFETIME 2.0f

#define WEAPON_LAUNCH_MULTIPLIER 1.0f

#define WEAPON_GUN_MIN_X 0.0f
#define WEAPON_GUN_MAX_X 3.0f

#define WEAPON_ROCKET_LAUNCHER_MAX_X -1.5f
#define WEAPON_ROCKET_LAUNCHER_MIN_X -4.0f

#define ROCKET_LAUNCHER_BULLET_SPEED 15.0f
#define ROCKET_LAUNCHER_SHOOT_DELAY 2.0f
#define ROCKET_LAUNCHER_BULLET_QUALITY 45
#define ROCKET_LAUNCHER_BULLET_EXPLODE_DELAY 0.25f
#define ROCKET_LAUNCHER_BULLET_LIFETIME 3.0f

#define ROCKET_GUN_BULLET_DELLAY 0.15f
#define ROCKET_GUN_BULLET_SPEED 45.0f

int createParticle(vec3 position, vec3 rotation, bool rocket = true);
void fireBegin();
void fireMainloop();
void spawnParticles();

class fireParticle {
public:
	void move();
	void render();

	vec3 position = vec3(0.0f);
	vec3 rotation = vec3(0.0f);
	vec3 scale = vec3(PARTICLE_SCALE);

	vec3 colour = vec3(1.0f, 0.0f, 0.0f);
	float lifetimeLeft = PARTICLE_LIFETIME;
	float dyingSpeed = PARTICLE_DYING_SPEED;
	float speed = 6.0f;

	bool active = true;
};
extern vector<fireParticle> allParticles;

extern float rocketShootDelay, rocketBulletShootDelay;
extern float rocketDriveDelay;
int createRocketLauncherBullet();

class rocketLauncherBulletIndividual {
public:
	bool active = true;
	void mainloop();

	void render();
	void begin();

	void move();
	void explode();

	float explodeTime = ROCKET_LAUNCHER_BULLET_EXPLODE_DELAY;
	float lifetime = ROCKET_LAUNCHER_BULLET_LIFETIME;

	vec3 position = vec3(0.0f);
	vec3 rotation = vec3(0.0f);
};

int createRocketGunBullet();
class rocketGunBulletIndividual {
public:
	bool active = true;
	void mainloop();

	void render();
	void begin();

	void move();

	vec3 position = vec3(0.0f);
	vec3 rotation = vec3(0.0f);
};

extern buildingColour rocket;
extern buildingColour rocketHolder;

extern float rocketLauncherExplodeTime, rocketGunExplodeTime, rocketExplodeTime;
extern bool rocketLauncherExploding, rocketGunExploding, rocketExploding;
extern vec3 rocketStatusScale;
void rocketExplodingMethod();

extern buildingColour rocketGun, rocketGunBullet;
extern buildingColour rocketLauncherGun, rocketLauncherBullet;
extern vector<rocketLauncherBulletIndividual> allRocketLauncherBullets; // position
extern vector<rocketGunBulletIndividual> allRocketGunBullets;

extern float doorRot;
vec4 rocketColliders(vec3 original, bool player = false, int monsterType = -1);
vec3 rocketCameraColliders(vec3 original, vec3 playerPos);
void renderRocket();

void openDoorAnimation(float multiplier);
void openDoor();

void startRocket();
void flyRocket();
vector<string> saveRocket(vector<string> current);

extern bool rocketFlying;
extern float rocketSpeed;
extern vec3 originalRocketPos;
extern float holderRot;

void rocketBegin();
void rocketMainloop();

#endif