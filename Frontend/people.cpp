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
        peopleMovement(&allPeople[p]);
        
		allPeople[p].position = terrainColliders(allPeople[p].position, 2.16f);
		allPeople[p].monsterColliders();
        allPeople[p].position = peoplePeopleCollider(allPeople[p].position, PEOPLE_PEOPLE_COLLIDE_DISTANCE);
        buildCollisions(allPeople[p].position, allPeople[p].personBuildingInside, emptyFloat, emptyBool, emptyBoolTwo, 1.5f);
        allPeople[p].position = peoplePlayerCollider(allPeople[p], PLAYER_COLLIDE_DISTANCE);
        
        allPeople[p].position.x = clamp(allPeople[p].position.x, 0.0f, currentPlanetScale.x); 
		allPeople[p].position.z = clamp(allPeople[p].position.z, -currentPlanetScale.y, 0.0f);

        allPeople[p].renderPlayer();
	}
}

vec3 peoplePlayerCollider(player person, float radius){
    vec3 original = person.position;
    
	vec3 returned = original;
	vec2 peopleFloor = vec2(returned.x, returned.z);

	vec3 playerPos = mainPlayer.position;
	vec2 playerFloor = vec2(playerPos.x, playerPos.z);

	float bearing = bearingTwo(playerFloor, peopleFloor);
	float distance = glm::distance(peopleFloor, playerFloor);

	if (distance < PLAYER_COLLIDE_DISTANCE) {
		returned.x = playerPos.x + sin(radians(bearing)) * radius;
		returned.z = playerPos.z + cos(radians(bearing)) * radius;
        
        int newIndex = 0;
        buildCollisions(returned, newIndex, emptyFloat, emptyBool, emptyBoolTwo);
        
        if(newIndex != person.personBuildingInside){
            returned.x = playerPos.x - sin(radians(bearing)) * radius;
            returned.z = playerPos.z - cos(radians(bearing)) * radius;
        }
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

void peopleMovement(player * person){
	if (!person->personMoving) {
		person->runAnimation(0.0f);
	}
	if (person->movementDelay < 0.0f && !person->personMoving && !person->personRotating) {
		retry:
		person->personRotation = (float) randomInt(0, 6) * 60.0f;
		if (degreesDistance(person->personRotation, person->rotation.y) < 65.0f) {
			goto retry;
		}
		person->personRotating = true;
	}
	if (person->personRotating) {
		float distance = glm::distance(degreesClamp(person->rotation.y), person->personRotation);
		if (distance > 180.0f) {
			bool changed = false;
			if (degreesClamp(person->rotation.y) > person->personRotation) {
				person->rotation.y += deltaTime * PEOPLE_ROTATE_SPEED;
				changed = true;
			}
			if (degreesClamp(person->rotation.y) < person->personRotation && !changed) {
				person->rotation.y -= deltaTime * PEOPLE_ROTATE_SPEED;
			}
		}
		if (distance <= 180.0f) {
			bool changed = false;
			if (degreesClamp(person->rotation.y) > person->personRotation) {
				person->rotation.y -= deltaTime * PEOPLE_ROTATE_SPEED;
				changed = true;
			}
			if (degreesClamp(person->rotation.y) < person->personRotation && !changed) {
				person->rotation.y += deltaTime * PEOPLE_ROTATE_SPEED;
			}
		}
		if (degreesDistance(person->personRotation, degreesClamp(person->rotation.y)) < 3.0f) {
			person->rotation.y = person->personRotation;
			person->personMoving = true;
			person->personRotating = false;
			person->movingStart = vec2(person->position.x, person->position.z);
		}
	}
	if (person->personMoving) {
		person->movingTime += deltaTime;

		person->position.x = person->position.x + -sin(radians(person->rotation.y)) * deltaTime * PEOPLE_MOVEMENT_SPEED;
		person->position.z = person->position.z + -cos(radians(person->rotation.y)) * deltaTime * PEOPLE_MOVEMENT_SPEED;
		person->runAnimation(PEOPLE_MOVEMENT_SPEED * 12.0f);
		vec2 floor = vec2(person->position.x, person->position.z);
		float distance = glm::distance(floor, person->movingStart);
		if (person->movingTime > PEOPLE_MOVEMENT_TIMEOUT || distance > PEOPLE_MOVEMENT_DISTANCE) {
			person->personMoving = false;
			person->personRotating = false;
			person->movementDelay = PEOPLE_MOVEMENT_DELAY;
			person->movingTime = 0.0f;
			return;
		}
		
	}
	person->movementDelay -= deltaTime;
}

vec3 peoplePeopleCollider(vec3 position, float radius){
    vec3 returned = position;
    vec2 returnedFloor = vec2(returned.x, returned.z);
    
    int personCount = allPeople.size();
    for(int p = 0; p < personCount; p++){
        vec3 personPosition = allPeople[p].position;
        vec2 personFloor = vec2(personPosition.x, personPosition.z);
        
        if(personPosition == position){
            continue;
        }
        
        float distance = glm::distance(returnedFloor, personFloor);
        if(distance < radius){
            float bearing = bearingTwo(personFloor, returnedFloor);
            returned.x = personFloor.x + sin(radians(bearing)) * PEOPLE_PEOPLE_COLLIDE_DISTANCE;
            returned.z = personFloor.y + cos(radians(bearing)) * PEOPLE_PEOPLE_COLLIDE_DISTANCE;
            
        }
        
        returnedFloor = vec2(returned.x, returned.z);
    }
    
    return returned;
}
