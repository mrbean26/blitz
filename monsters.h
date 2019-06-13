#pragma once

#ifndef  MONSTERS_H
#define MONSTERS_H

#define EARTH_ONE 1
#define EARTH_TWO 2

class monster {
	monster(int name = 1, vector<float> vertices = {});

	int name = 1;

	vec3 position, rotation, 
		scale = vec3(1.0f);

	void render();
};

void monstersBegin();
void monstersMainloop();

#endif 

