#pragma once

#ifndef  MONSTERS_H
#define MONSTERS_H

#define EARTH_ONE 1
#define EARTH_TWO 2

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

	void render();
	GLuint VAO, VBO, size;
};

void monstersBegin();
void monstersMainloop();

extern vector<monster> allMonsterTemplates;
extern vector<monster> allMonsters; // need to be written when change planet

void getMonsters();
void monsterInteractions();
float monsterCameraColliders(vec3 cameraPos, vec3 lookAt);
void monsterMonsterColliders();

void monsterDamage();

int createMonster(int monsterType);
int createMonsterTemplate(int name, vector<float> vertices, float health,
	vec3 scale = vec3(1.0f));

void monsterSpawning();
void renderMonsters();

#endif 

