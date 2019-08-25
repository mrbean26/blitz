#pragma once

#ifndef  PEOPLE_H
#define PEOPLE_H

#define	TYPE_COUNT 2
#define CIVILIAN 0
#define ROBOT 1

#define PEOPLE_MOVEMENT_DELAY 4.0f
#define PLAYER_COLLIDE_DISTANCE 2.0f
#define PEOPLE_ROTATE_SPEED 100.0f

#define PEOPLE_MOVEMENT_DISTANCE 3.0f
#define PEOPLE_MOVEMENT_TIMEOUT 4.0f
#define PEOPLE_MOVEMENT_SPEED 3.0f

void peopleBegin();
void peopleMainloop();

vec3 peoplePlayerCollider(vec3 original, float radius);
int createPlayer(int type, vec3 position, vec3 scale = vec3(0.8f));
void getPeople();
void peopleMovement(player * person);

extern vector<player> allPeople;

#endif
