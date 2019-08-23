#pragma once

#ifndef  PEOPLE_H
#define PEOPLE_h

#define	TYPE_COUNT 2
#define CIVILIAN 0
#define ROBOT 1

#define PLAYER_COLLIDE_DISTANCE 2.0f

void peopleBegin();
void peopleMainloop();

vec3 peoplePlayerCollider(vec3 original, float radius);
int createPlayer(int type, vec3 position, vec3 scale = vec3(0.8f));
void getPeople();

extern vector<player> allPeople;

#endif
