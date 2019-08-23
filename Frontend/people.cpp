#include "player.h"
#include "people.h"
#include "interface.h"
#include "frontend.h"
#include "structures.h"
#include "monsters.h"

vector<player> allPeople;
vector<texture> skinTextures;

void peopleBegin(){
	string paths[] = { "assets/models/player/civilian.png", "assets/models/player/robot.png" };
	for (int i = 0; i < TYPE_COUNT; i++) {
		skinTextures[newVectorPos(&skinTextures)] = loadTexture(paths[i].data());
	}
	getPeople();
}

void peopleMainloop(){
	if (!WorldGeneration.startedBegin) {
		return;
	}
	int pCount = allPeople.size();
	for (int p = 0; p < pCount; p++) {
		allPeople[p].position = peoplePlayerCollider(allPeople[p].position, PLAYER_COLLIDE_DISTANCE);
		allPeople[p].position = terrainColliders(allPeople[p].position, 2.16f);
		buildCollisions(allPeople[p].position, emptyInt, emptyFloat, emptyBool, emptyBoolTwo);
		allPeople[p].monsterColliders();

		allPeople[p].position.x = clamp(allPeople[p].position.x, 0.0f, currentPlanetScale.x);
		allPeople[p].position.z = clamp(allPeople[p].position.z, -currentPlanetScale.y, 0.0f);

		allPeople[p].renderPlayer();
	}
}

vec3 peoplePlayerCollider(vec3 original, float radius){
	vec3 returned = original;
	vec2 peopleFloor = vec2(returned.x, returned.z);

	vec3 playerPos = mainPlayer.position;
	vec2 playerFloor = vec2(playerPos.x, playerPos.z);

	float bearing = bearingTwo(playerFloor, peopleFloor);
	float distance = glm::distance(peopleFloor, playerFloor);

	if (distance < PLAYER_COLLIDE_DISTANCE) {
		returned.x = playerPos.x + sin(radians(bearing)) * radius;
		returned.z = playerPos.z + cos(radians(bearing)) * radius;
	}

	return returned;
}

int createPlayer(int type, vec3 position, vec3 scale){
	int size = allPeople.size();
	allPeople.resize(size + 1);

	player newPlayer;
	newPlayer.position = position;
	newPlayer.scale = scale;
	newPlayer.playerTexture = skinTextures[type];
	newPlayer.personType = type;
	newPlayer.startCharacterVertices();
	allPeople[size] = newPlayer;

	return size;
}

void getPeople(){
	vector<string> lines = readLines(WorldGeneration.worldLinesPath);
	int lCount = lines.size();
	for (int l = 0; l < lCount; l++) {
		if (contains(lines[l], WorldGeneration.currentAreaPrefix + "Person")) {
			vec4 personData = getVec4File(WorldGeneration.worldLinesPath, WorldGeneration.currentAreaPrefix + "Person", l);
			int newPlayer = createPlayer((int) personData.w, vec3(personData));
			allPeople[newPlayer].rotation.y = personData.y;
		}
	}
}
