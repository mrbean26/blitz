#pragma once

#ifndef  MONSTERS_H
#define MONSTERS_H

#define EARTH_ONE 1
#define EARTH_TWO 2
#define EARTH_ONE_SCALE 1.25f
#define EARTH_TWO_SCALE 2.5f
#define MONSTER_TO_MONSTER_COLLIDER_DISTANCE 1.75f
#define MONSTER_LIGHT_SPAWN_MAX 0.7f
#define MONSTER_MAX_COUNT 11
#define MONSTER_SPAWN_RATE 1000
#define DAMAGE_SHOW_RED_TIME 0.75f
#define EARTH_ONE_BULLET_DISTANCE 0.5f
#define EARTH_TWO_BULLET_DISTANCE_CENTER 0.65f
#define EARTH_TWO_ARM_LENGTH 1.0f
#define EARTH_TWO_ARM_WIDTH 8.0f
#define MONSTER_ROTATE_DISTANCE_MAX 12.0f
#define MONSTER_MOVE_DISTANCE_MAX 8.0f
#define MONSTER_MOVE_ROTATE_DISTANCE_MIN 3.0f
#define MONSTER_CAMERA_ANGLE 17.5f
#define MONSTER_CAMERA_DISTANCE 1.0f
#define DYING_X_COLOUR 2.5f
#define DYING_Y_COLOUR 0.2f
#define DYING_Z_COLOUR 0.2f
#define DYING_ROTATE_SPEED 240.0f
#define MONSTER_ATTACK_CLOSEST_DISTANCE 1.0f
#define MONSTER_ATTACK_SPEED 10.0f
#define EARTH_ONE_DROP_CHANCE 1
#define EARTH_TWO_DROP_CHANCE 3
#define EARTH_ONE_DROP_INDEX 0
#define EARTH_TWO_DROP_INDEX 1

#include "interface.h"

extern int earthOne, earthTwo;

class monster {
public:
	monster(int name = 1, vector<float> vertices = {});
	int monsterName = 1;
	float health = 100.0f;

	vec3 position, rotation = vec3(0.0f), 
		scale = vec3(1.0f);
	float speed = 3.0f;

	bool attacking = false;
	bool finishedAttackOne = false;
	float damage = 0.0f;
	float defaultAttackDelay = 3.0f;
	float attackChanceDelay = 3.0f;

	bool droppedItem = false;
	int itemIndex = 0;
	int itemDropChance = 1;

	void render();
	GLuint VAO, VBO, size;

	vec3 multiplyColour = vec3(1.0f);
	float changeBackDelay = -1.0f;
};

void monstersBegin();
void monstersMainloop();

extern vector<monster> allMonsterTemplates;
extern vector<monster> allMonsters; // need to be written when change planet

float degreesClamp(float degrees);
float bearingTwo(vec2 xyOne, vec2 xyTwo);

void getMonsters();
void monsterInteractions();

float monsterCameraColliders(vec3 cameraPos, vec3 lookAt, float defaultDistance);
void monsterMonsterColliders();
void monsterBuildCollisions();
void monsterTerrainCollisions();

void monsterDamage();

int createMonster(int monsterType);
int createMonsterTemplate(int name, vector<float> vertices, float health, float damage, float attackDelayChance, int dropIndex, int dropChance,
	vec3 scale = vec3(1.0f));

void monsterSpawning();
void renderMonsters();

#endif 

