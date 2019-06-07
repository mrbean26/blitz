#include "weapons.h"
#include "structures.h"

int basicPistol;

void weaponsBegin(){
	startWeapons();
	startWeaponUI();
}

void weaponsMainloop(){
	if (!WorldGeneration.startedBegin) {
		return;
	}
	moveBullets();
	weaponUI();
}

void startWeapons() {
	// basic pistol
	vector<vec3> pistolColours = colourVector(14, vec3(0.8f, 0.8f, 0.8f), 0.2f);
	vector<float> basicPistolVertices{
		0.0f, 0.0f, 0.0f, pistolColours[0].x, pistolColours[0].y, pistolColours[0].z,
		0.0f, 1.1f, 0.0f, pistolColours[0].x, pistolColours[0].y, pistolColours[0].z,
		0.4f, 0.0f, 0.0f, pistolColours[0].x, pistolColours[0].y, pistolColours[0].z,

		0.4f, 1.1f, 0.0f, pistolColours[0].x, pistolColours[0].y, pistolColours[0].z,
		0.0f, 1.1f, 0.0f, pistolColours[0].x, pistolColours[0].y, pistolColours[0].z,
		0.4f, 0.0f, 0.0f, pistolColours[0].x, pistolColours[0].y, pistolColours[0].z,

		0.0f, 0.0f, 0.5f, pistolColours[1].x, pistolColours[1].y, pistolColours[1].z,
		0.0f, 1.1f, 0.5f, pistolColours[1].x, pistolColours[1].y, pistolColours[1].z,
		0.4f, 0.0f, 0.5f, pistolColours[1].x, pistolColours[1].y, pistolColours[1].z,

		0.4f, 1.1f, 0.5f, pistolColours[1].x, pistolColours[1].y, pistolColours[1].z,
		0.0f, 1.1f, 0.5f, pistolColours[1].x, pistolColours[1].y, pistolColours[1].z,
		0.4f, 0.0f, 0.5f, pistolColours[1].x, pistolColours[1].y, pistolColours[1].z,

		0.4f, 0.6f, 0.0f, pistolColours[2].x, pistolColours[2].y, pistolColours[2].z,
		1.4f, 0.6f, 0.0f, pistolColours[2].x, pistolColours[2].y, pistolColours[2].z,
		0.4f, 1.1f, 0.0f, pistolColours[2].x, pistolColours[2].y, pistolColours[2].z,

		1.4f, 1.1f, 0.0f, pistolColours[2].x, pistolColours[2].y, pistolColours[2].z,
		1.4f, 0.6f, 0.0f, pistolColours[2].x, pistolColours[2].y, pistolColours[2].z,
		0.4f, 1.1f, 0.0f, pistolColours[2].x, pistolColours[2].y, pistolColours[2].z,

		0.4f, 0.6f, 0.5f, pistolColours[3].x, pistolColours[3].y, pistolColours[3].z,
		1.4f, 0.6f, 0.5f, pistolColours[3].x, pistolColours[3].y, pistolColours[3].z,
		0.4f, 1.1f, 0.5f, pistolColours[3].x, pistolColours[3].y, pistolColours[3].z,

		1.4f, 1.1f, 0.5f, pistolColours[3].x, pistolColours[3].y, pistolColours[3].z,
		1.4f, 0.6f, 0.5f, pistolColours[3].x, pistolColours[3].y, pistolColours[3].z,
		0.4f, 1.1f, 0.5f, pistolColours[3].x, pistolColours[3].y, pistolColours[3].z,

		0.0f, 1.1f, 0.0f, pistolColours[4].x, pistolColours[4].y, pistolColours[4].z,
		1.4f, 1.1f, 0.0f, pistolColours[4].x, pistolColours[4].y, pistolColours[4].z,
		0.0f, 1.1f, 0.5f, pistolColours[4].x, pistolColours[4].y, pistolColours[4].z,

		1.4f, 1.1f, 0.5f, pistolColours[4].x, pistolColours[4].y, pistolColours[4].z,
		1.4f, 1.1f, 0.0f, pistolColours[4].x, pistolColours[4].y, pistolColours[4].z,
		0.0f, 1.1f, 0.5f, pistolColours[4].x, pistolColours[4].y, pistolColours[4].z,

		0.0f, 0.6f, 0.0f, pistolColours[5].x, pistolColours[5].y, pistolColours[5].z,
		1.4f, 0.6f, 0.0f, pistolColours[5].x, pistolColours[5].y, pistolColours[5].z,
		0.4f, 0.6f, 0.5f, pistolColours[5].x, pistolColours[5].y, pistolColours[5].z,

		1.4f, 0.6f, 0.5f, pistolColours[5].x, pistolColours[5].y, pistolColours[5].z,
		1.4f, 0.6f, 0.0f, pistolColours[5].x, pistolColours[5].y, pistolColours[5].z,
		0.4f, 0.6f, 0.5f, pistolColours[5].x, pistolColours[5].y, pistolColours[5].z,

		1.4f, 0.6f, 0.0f, pistolColours[6].x, pistolColours[6].y, pistolColours[6].z,
		1.4f, 1.1f, 0.0f, pistolColours[6].x, pistolColours[6].y, pistolColours[6].z,
		1.4f, 0.6f, 0.5f, pistolColours[6].x, pistolColours[6].y, pistolColours[6].z,

		1.4f, 1.1f, 0.5f, pistolColours[6].x, pistolColours[6].y, pistolColours[6].z,
		1.4f, 1.1f, 0.0f, pistolColours[6].x, pistolColours[6].y, pistolColours[6].z,
		1.4f, 0.6f, 0.5f, pistolColours[6].x, pistolColours[6].y, pistolColours[6].z,

		0.0f, 0.0f, 0.0f, pistolColours[7].x, pistolColours[7].y, pistolColours[7].z,
		0.0f, 1.1f, 0.0f, pistolColours[7].x, pistolColours[7].y, pistolColours[7].z,
		0.0f, 0.0f, 0.5f, pistolColours[7].x, pistolColours[7].y, pistolColours[7].z,

		0.0f, 1.1f, 0.5f, pistolColours[7].x, pistolColours[7].y, pistolColours[7].z,
		0.0f, 1.1f, 0.0f, pistolColours[7].x, pistolColours[7].y, pistolColours[7].z,
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

int weaponSelectImage, weaponSelectCornerOne, weaponSelectCornerTwo, weaponSelectCornerThree, weaponSelectCornerFour;
void startWeaponUI(){
	// select buttons
	button defaultSelect;
	defaultSelect.texture = loadTexture("assets/images/weaponSelectCorner.png");
	defaultSelect.position = vec3(-1.4f, 1.4f, 0.0f);
	defaultSelect.colour = vec3(0.05f);
	defaultSelect.alpha = 0.6f;

	weaponSelectCornerOne = createButton();
	allButtons[weaponSelectCornerOne] = defaultSelect;
	allButtons[weaponSelectCornerOne].rotation = 90.0f;
	allButtons[weaponSelectCornerOne].position = vec3(1.4f, -1.4f, 0.0f);

	weaponSelectCornerTwo = createButton();
	allButtons[weaponSelectCornerTwo] = defaultSelect;
	allButtons[weaponSelectCornerTwo].position = vec3(-1.4f, -1.4f, 0.0f);

	weaponSelectCornerThree = createButton();
	allButtons[weaponSelectCornerThree] = defaultSelect;
	allButtons[weaponSelectCornerThree].rotation = 180.0f;
	allButtons[weaponSelectCornerThree].position = vec3(1.4f, 1.4f, 0.0f);

	weaponSelectCornerFour = createButton();
	allButtons[weaponSelectCornerFour] = defaultSelect;
	allButtons[weaponSelectCornerFour].rotation = 270.0f;

	// middle icon
	weaponSelectImage = createButton();
	allButtons[weaponSelectImage].interactive = false;
	allButtons[weaponSelectImage].scale = vec2(0.51f, 0.267f);
	allButtons[weaponSelectImage].texture = loadTexture("assets/images/weaponSelectImage.png");
	allButtons[weaponSelectImage].colour = vec3(0.05f);
	allButtons[weaponSelectImage].alpha = 0.8f;
}

bool weaponSelectOpen = false;
void weaponUI() {
	int buttons [] = { weaponSelectCornerOne, weaponSelectCornerTwo, 
		weaponSelectCornerThree, weaponSelectCornerFour, weaponSelectImage };
	if (!checkKey(interactKey)) {
		if (weaponSelectOpen) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		weaponSelectOpen = false;
		mainPlayer.canMoveCamera = true;
	}
	if (checkKey(interactKey)) {
		if (!weaponSelectOpen) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		weaponSelectOpen = true;
		mainPlayer.canMoveCamera = false;
	}
	for (int b = 0; b < 5; b++) {
		int index = buttons[b];
		allButtons[index].active = false;
		if (!weaponSelectOpen) {
			continue;
		}
		if (!paused && !benchInUse) {
			allButtons[index].active = true;
		}
		if (buttons[b] == weaponSelectImage) {
			continue;
		}
		// interaction
		allButtons[index].alpha = 0.6f;
		if (allButtons[index].mouseOver) {
			allButtons[index].alpha = 0.8f;
		}
	}
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
	float bulletSpeed = 1.1f;
	for (int i = 0; i < count; i++) {
		vec3 bulletRot = allBullets[i].rotation;

		vec3 added = vec3(0.0f);
		added.x = cos(radians(bulletRot.y)) * cos(radians(bulletRot.z)) * bulletSpeed;
		added.y = sin(radians(bulletRot.z)) * bulletSpeed;
		added.z = -sin(radians(bulletRot.y)) * cos(radians(bulletRot.z)) * bulletSpeed;

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
