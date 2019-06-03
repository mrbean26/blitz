#include "weapons.h"
#include "structures.h"

int basicPistol;

void weaponsBegin(){
	startWeapons();
}

void weaponsMainloop(){
	if (!WorldGeneration.startedBegin) {
		return;
	}
	moveBullets();
}

void startWeapons() {
	// basic pistol
	vector<vec3> pistolColours = colourVector(14, vec3(0.8f, 0.8f, 0.8f), 0.2f);
	vector<float> basicPistolVertices{
		0.0f, 0.0f, 0.0f, pistolColours[0].x, pistolColours[0].y, pistolColours[0].z,
		0.0f, 1.0f, 0.0f, pistolColours[0].x, pistolColours[0].y, pistolColours[0].z,
		0.4f, 0.0f, 0.0f, pistolColours[0].x, pistolColours[0].y, pistolColours[0].z,

		0.4f, 1.0f, 0.0f, pistolColours[0].x, pistolColours[0].y, pistolColours[0].z,
		0.0f, 1.0f, 0.0f, pistolColours[0].x, pistolColours[0].y, pistolColours[0].z,
		0.4f, 0.0f, 0.0f, pistolColours[0].x, pistolColours[0].y, pistolColours[0].z,

		0.0f, 0.0f, 0.5f, pistolColours[1].x, pistolColours[1].y, pistolColours[1].z,
		0.0f, 1.0f, 0.5f, pistolColours[1].x, pistolColours[1].y, pistolColours[1].z,
		0.4f, 0.0f, 0.5f, pistolColours[1].x, pistolColours[1].y, pistolColours[1].z,

		0.4f, 1.0f, 0.5f, pistolColours[1].x, pistolColours[1].y, pistolColours[1].z,
		0.0f, 1.0f, 0.5f, pistolColours[1].x, pistolColours[1].y, pistolColours[1].z,
		0.4f, 0.0f, 0.5f, pistolColours[1].x, pistolColours[1].y, pistolColours[1].z,

		0.4f, 0.6f, 0.0f, pistolColours[2].x, pistolColours[2].y, pistolColours[2].z,
		1.4f, 0.6f, 0.0f, pistolColours[2].x, pistolColours[2].y, pistolColours[2].z,
		0.4f, 1.0f, 0.0f, pistolColours[2].x, pistolColours[2].y, pistolColours[2].z,

		1.4f, 1.0f, 0.0f, pistolColours[2].x, pistolColours[2].y, pistolColours[2].z,
		1.4f, 0.6f, 0.0f, pistolColours[2].x, pistolColours[2].y, pistolColours[2].z,
		0.4f, 1.0f, 0.0f, pistolColours[2].x, pistolColours[2].y, pistolColours[2].z,

		0.4f, 0.6f, 0.5f, pistolColours[3].x, pistolColours[3].y, pistolColours[3].z,
		1.4f, 0.6f, 0.5f, pistolColours[3].x, pistolColours[3].y, pistolColours[3].z,
		0.4f, 1.0f, 0.5f, pistolColours[3].x, pistolColours[3].y, pistolColours[3].z,

		1.4f, 1.0f, 0.5f, pistolColours[3].x, pistolColours[3].y, pistolColours[3].z,
		1.4f, 0.6f, 0.5f, pistolColours[3].x, pistolColours[3].y, pistolColours[3].z,
		0.4f, 1.0f, 0.5f, pistolColours[3].x, pistolColours[3].y, pistolColours[3].z,

		0.0f, 1.0f, 0.0f, pistolColours[4].x, pistolColours[4].y, pistolColours[4].z,
		1.4f, 1.0f, 0.0f, pistolColours[4].x, pistolColours[4].y, pistolColours[4].z,
		0.0f, 1.0f, 0.5f, pistolColours[4].x, pistolColours[4].y, pistolColours[4].z,

		1.4f, 1.0f, 0.5f, pistolColours[4].x, pistolColours[4].y, pistolColours[4].z,
		1.4f, 1.0f, 0.0f, pistolColours[4].x, pistolColours[4].y, pistolColours[4].z,
		0.0f, 1.0f, 0.5f, pistolColours[4].x, pistolColours[4].y, pistolColours[4].z,

		0.0f, 0.6f, 0.0f, pistolColours[5].x, pistolColours[5].y, pistolColours[5].z,
		1.4f, 0.6f, 0.0f, pistolColours[5].x, pistolColours[5].y, pistolColours[5].z,
		0.4f, 0.6f, 0.5f, pistolColours[5].x, pistolColours[5].y, pistolColours[5].z,

		1.4f, 0.6f, 0.5f, pistolColours[5].x, pistolColours[5].y, pistolColours[5].z,
		1.4f, 0.6f, 0.0f, pistolColours[5].x, pistolColours[5].y, pistolColours[5].z,
		0.4f, 0.6f, 0.5f, pistolColours[5].x, pistolColours[5].y, pistolColours[5].z,

		1.4f, 0.6f, 0.0f, pistolColours[6].x, pistolColours[6].y, pistolColours[6].z,
		1.4f, 1.0f, 0.0f, pistolColours[6].x, pistolColours[6].y, pistolColours[6].z,
		1.4f, 0.6f, 0.5f, pistolColours[6].x, pistolColours[6].y, pistolColours[6].z,

		1.4f, 1.0f, 0.5f, pistolColours[6].x, pistolColours[6].y, pistolColours[6].z,
		1.4f, 1.0f, 0.0f, pistolColours[6].x, pistolColours[6].y, pistolColours[6].z,
		1.4f, 0.6f, 0.5f, pistolColours[6].x, pistolColours[6].y, pistolColours[6].z,

		0.0f, 0.0f, 0.0f, pistolColours[7].x, pistolColours[7].y, pistolColours[7].z,
		0.0f, 1.0f, 0.0f, pistolColours[7].x, pistolColours[7].y, pistolColours[7].z,
		0.0f, 0.0f, 0.5f, pistolColours[7].x, pistolColours[7].y, pistolColours[7].z,

		0.0f, 1.0f, 0.5f, pistolColours[7].x, pistolColours[7].y, pistolColours[7].z,
		0.0f, 1.0f, 0.0f, pistolColours[7].x, pistolColours[7].y, pistolColours[7].z,
		0.0f, 0.0f, 0.5f, pistolColours[7].x, pistolColours[7].y, pistolColours[7].z,

		0.4f, 0.0f, 0.0f, pistolColours[8].x, pistolColours[8].y, pistolColours[8].z,
		0.4f, 0.6f, 0.0f, pistolColours[8].x, pistolColours[8].y, pistolColours[8].z,
		0.4f, 0.0f, 0.5f, pistolColours[8].x, pistolColours[8].y, pistolColours[8].z,

		0.4f, 0.6f, 0.5f, pistolColours[8].x, pistolColours[8].y, pistolColours[8].z,
		0.4f, 0.6f, 0.0f, pistolColours[8].x, pistolColours[8].y, pistolColours[8].z,
		0.4f, 0.0f, 0.5f, pistolColours[8].x, pistolColours[8].y, pistolColours[8].z,

		0.0f, 0.0f, 0.0f, pistolColours[9].x, pistolColours[9].y, pistolColours[9].z,
		0.0f, 0.0f, 0.5f, pistolColours[9].x, pistolColours[9].y, pistolColours[9].z,
		0.4f, 0.0f, 0.0f, pistolColours[9].x, pistolColours[9].y, pistolColours[9].z,

		0.4f, 0.0f, 0.5f, pistolColours[9].x, pistolColours[9].y, pistolColours[9].z,
		0.0f, 0.0f, 0.5f, pistolColours[9].x, pistolColours[9].y, pistolColours[9].z,
		0.4f, 0.0f, 0.0f, pistolColours[9].x, pistolColours[9].y, pistolColours[9].z,

		// handle

		0.4f, 0.2f, 0.15f, pistolColours[10].x, pistolColours[10].y, pistolColours[10].z,
		0.4f, 0.4f, 0.15f, pistolColours[10].x, pistolColours[10].y, pistolColours[10].z,
		0.8f, 0.6f, 0.15f, pistolColours[10].x, pistolColours[10].y, pistolColours[10].z,

		0.6f, 0.6f, 0.15f, pistolColours[10].x, pistolColours[10].y, pistolColours[10].z,
		0.4f, 0.4f, 0.15f, pistolColours[10].x, pistolColours[10].y, pistolColours[10].z,
		0.8f, 0.6f, 0.15f, pistolColours[10].x, pistolColours[10].y, pistolColours[10].z,

		0.4f, 0.2f, 0.35f, pistolColours[11].x, pistolColours[11].y, pistolColours[11].z,
		0.4f, 0.4f, 0.35f, pistolColours[11].x, pistolColours[11].y, pistolColours[11].z,
		0.8f, 0.6f, 0.35f, pistolColours[11].x, pistolColours[11].y, pistolColours[11].z,

		0.6f, 0.6f, 0.35f, pistolColours[11].x, pistolColours[11].y, pistolColours[11].z,
		0.4f, 0.4f, 0.35f, pistolColours[11].x, pistolColours[11].y, pistolColours[11].z,
		0.8f, 0.6f, 0.35f, pistolColours[11].x, pistolColours[11].y, pistolColours[11].z,

		0.4f, 0.4f, 0.35f, pistolColours[12].x, pistolColours[12].y, pistolColours[12].z,
		0.4f, 0.4f, 0.15f, pistolColours[12].x, pistolColours[12].y, pistolColours[12].z,
		0.6f, 0.6f, 0.35f, pistolColours[12].x, pistolColours[12].y, pistolColours[12].z,
		
		0.6f, 0.6f, 0.15f, pistolColours[12].x, pistolColours[12].y, pistolColours[12].z,
		0.6f, 0.6f, 0.35f, pistolColours[12].x, pistolColours[12].y, pistolColours[12].z,
		0.4f, 0.4f, 0.15f, pistolColours[12].x, pistolColours[12].y, pistolColours[12].z,

		0.4f, 0.2f, 0.35f, pistolColours[13].x, pistolColours[13].y, pistolColours[13].z,
		0.4f, 0.2f, 0.15f, pistolColours[13].x, pistolColours[13].y, pistolColours[13].z,
		0.8f, 0.6f, 0.35f, pistolColours[13].x, pistolColours[13].y, pistolColours[13].z,

		0.8f, 0.6f, 0.15f, pistolColours[13].x, pistolColours[13].y, pistolColours[13].z,
		0.8f, 0.6f, 0.35f, pistolColours[13].x, pistolColours[13].y, pistolColours[13].z,
		0.4f, 0.2f, 0.15f, pistolColours[13].x, pistolColours[13].y, pistolColours[13].z,
	};
	basicPistol = createWeapon(basicPistolVertices);
}

vector<weapon> allWeapons;
int createWeapon(vector<float> vertices){
	weapon newWeapon(vertices);
	allWeapons[newVectorPos(&allWeapons)] = newWeapon;
	int size = allWeapons.size();
	return size - 1;
}

weapon::weapon(vector<float> vertices){
	startIrregularColorBuilding(vertices, VAO, VBO, vertCount);
}

void weapon::render(mat4 model){
	glUseProgram(playerShader);
	setMat4(playerShader, "model", model);
	setMat4(playerShader, "projection", projectionMatrix());
	setMat4(playerShader, "view", viewMatrix());
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertCount);
}

vector<bullet> allBullets;

void moveBullets() {
	int count = allBullets.size();
	float bulletSpeed = 0.5f;
	for (int i = 0; i < count; i++) {
		vec3 bulletRot = allBullets[i].rotation;

		vec3 added = vec3(0.0f);
		added.x = -sin(radians(bulletRot.y)) * bulletSpeed;
		added.y = -cos(radians(bulletRot.x)) * bulletSpeed;
		added.z = -cos(radians(bulletRot.y)) * bulletSpeed;

		allBullets[i].position += added;
	}
}

int createBullet(vec3 position, vec3 rotation) {
	bullet newBullet;
	newBullet.position = position;
	newBullet.rotation = rotation;
	allBullets[newVectorPos(&allBullets)] = newBullet;
	return allBullets.size() - 1;
}
