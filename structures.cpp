#include "structures.h"
#include "player.h"
#include "shader.h"
#include "display.h"

void StructuresBegin(){
	startBuildings();
}

void StructuresMainloop(){
	if (!WorldGeneration.active) { return; }
	renderBuildings();
}

void startColorBuilding(vector<float> vertices, buildingColour * usedBuilding) {
	glGenVertexArrays(1, &usedBuilding->VAO);
	glBindVertexArray(usedBuilding->VAO);
	glGenBuffers(1, &usedBuilding->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, usedBuilding->VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // position attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // colour attribute
	cout << vertices.size() << endl;
	usedBuilding->size = vertices.size() / 6;
}

buildingColour mainBench;
buildingColour mainBlueprint;
void startBuildBench(){
	// data
	vector<vec3> benchColours = colourVector(20, vec3(0.43f, 0.24f, 0.11f));
	vector<float> benchVertices = {
		// bottom half
		// sides
		0.0f, 0.0f, -1.0f, benchColours[0].x, benchColours[0].y, benchColours[0].z,
		1.0f, 0.0f, -1.0f, benchColours[0].x, benchColours[0].y, benchColours[0].z,
		1.0f, 2.0f, -1.0f, benchColours[0].x, benchColours[0].y, benchColours[0].z,

		0.0f, 0.0f, -1.0f, benchColours[1].x, benchColours[1].y, benchColours[1].z,
		0.0f, 2.0f, -1.0f, benchColours[1].x, benchColours[1].y, benchColours[1].z,
		1.0f, 2.0f, -1.0f, benchColours[1].x, benchColours[1].y, benchColours[1].z,

		0.0f, 0.0f, 2.0f, benchColours[2].x, benchColours[2].y, benchColours[2].z,
		1.0f, 0.0f, 2.0f, benchColours[2].x, benchColours[2].y, benchColours[2].z,
		1.0f, 2.0f, 2.0f, benchColours[2].x, benchColours[2].y, benchColours[2].z,

		0.0f, 0.0f, 2.0f, benchColours[3].x, benchColours[3].y, benchColours[3].z,
		0.0f, 2.0f, 2.0f, benchColours[3].x, benchColours[3].y, benchColours[3].z,
		1.0f, 2.0f, 2.0f, benchColours[3].x, benchColours[3].y, benchColours[3].z,
		// other sides
		0.0f, 0.0f, -1.0f, benchColours[4].x, benchColours[4].y, benchColours[4].z,
		0.0f, 2.0f, -1.0f, benchColours[4].x, benchColours[4].y, benchColours[4].z,
		0.0f, 0.0f, 2.0f, benchColours[4].x, benchColours[4].y, benchColours[4].z,

		0.0f, 2.0f, -1.0f, benchColours[5].x, benchColours[5].y, benchColours[5].z,
		0.0f, 2.0f, 2.0f, benchColours[5].x, benchColours[5].y, benchColours[5].z,
		0.0f, 0.0f, 2.0f, benchColours[5].x, benchColours[5].y, benchColours[5].z,

		1.0f, 0.0f, -1.0f, benchColours[6].x, benchColours[6].y, benchColours[6].z,
		1.0f, 2.0f, -1.0f, benchColours[6].x, benchColours[6].y, benchColours[6].z,
		1.0f, 0.0f, 2.0f, benchColours[6].x, benchColours[6].y, benchColours[6].z,

		1.0f, 2.0f, -1.0f, benchColours[7].x, benchColours[7].y, benchColours[7].z,
		1.0f, 2.0f, 2.0f, benchColours[7].x, benchColours[7].y, benchColours[7].z,
		1.0f, 0.0f, 2.0f, benchColours[7].x, benchColours[7].y, benchColours[7].z,
		// top
		-0.5f, 2.0f, -2.0f, benchColours[8].x, benchColours[8].y, benchColours[8].z,
		-0.5f, 2.0f, 3.0f, benchColours[8].x, benchColours[8].y, benchColours[8].z,
		1.5f, 2.0f, -2.0f, benchColours[8].x, benchColours[8].y, benchColours[8].z,

		-0.5f, 2.0f, 3.0f, benchColours[9].x, benchColours[9].y, benchColours[9].z,
		1.5f, 2.0f, -2.0f, benchColours[9].x, benchColours[9].y, benchColours[9].z,
		1.5f, 2.0f, 3.0f, benchColours[9].x, benchColours[9].y, benchColours[9].z,
		// top half
		-0.5f, 2.25f, -2.0f, benchColours[10].x, benchColours[10].y, benchColours[10].z,
		-0.5f, 2.25f, 3.0f, benchColours[10].x, benchColours[10].y, benchColours[10].z,
		1.5f, 2.25f, -2.0f, benchColours[10].x, benchColours[10].y, benchColours[10].z,

		-0.5f, 2.25f, 3.0f, benchColours[11].x, benchColours[11].y, benchColours[11].z,
		1.5f, 2.25f, -2.0f, benchColours[11].x, benchColours[11].y, benchColours[11].z,
		1.5f, 2.25f, 3.0f, benchColours[11].x, benchColours[11].y, benchColours[11].z,
		// sides
		-0.5f, 2.0f, -2.0f, benchColours[12].x, benchColours[12].y, benchColours[12].z,
		-0.5f, 2.25f, -2.0f, benchColours[12].x, benchColours[12].y, benchColours[12].z,
		1.5f, 2.0f, -2.0f, benchColours[12].x, benchColours[12].y, benchColours[12].z,

		1.5f, 2.0f, -2.0f, benchColours[13].x, benchColours[13].y, benchColours[13].z,
		1.5f, 2.25f, -2.0f, benchColours[13].x, benchColours[13].y, benchColours[13].z,
		-0.5f, 2.25f, -2.0f, benchColours[13].x, benchColours[13].y, benchColours[13].z,

		-0.5f, 2.0f, 3.0f, benchColours[14].x, benchColours[14].y, benchColours[14].z,
		-0.5f, 2.25f, 3.0f, benchColours[14].x, benchColours[14].y, benchColours[14].z,
		1.5f, 2.0f, 3.0f, benchColours[14].x, benchColours[14].y, benchColours[14].z,

		1.5f, 2.0f, 3.0f, benchColours[15].x, benchColours[15].y, benchColours[15].z,
		1.5f, 2.25f, 3.0f, benchColours[15].x, benchColours[15].y, benchColours[15].z,
		-0.5f, 2.25f, 3.0f, benchColours[15].x, benchColours[15].y, benchColours[15].z,
		// other sides
		-0.5f, 2.0f, -2.0f, benchColours[16].x, benchColours[16].y, benchColours[16].z,
		-0.5f, 2.25f, 3.0f, benchColours[16].x, benchColours[16].y, benchColours[16].z,
		-0.5f, 2.0f, 3.0f, benchColours[16].x, benchColours[16].y, benchColours[16].z,

		-0.5f, 2.25f, 3.0f, benchColours[17].x, benchColours[17].y, benchColours[17].z,
		-0.5f, 2.25f, -2.0f, benchColours[17].x, benchColours[17].y, benchColours[17].z,
		-0.5f, 2.0f, -2.0f, benchColours[17].x, benchColours[17].y, benchColours[17].z,

		1.5f, 2.0f, -2.0f, benchColours[18].x, benchColours[18].y, benchColours[18].z,
		1.5f, 2.25f, 3.0f, benchColours[18].x, benchColours[18].y, benchColours[18].z,
		1.5f, 2.0f, 3.0f, benchColours[18].x, benchColours[18].y, benchColours[18].z,

		1.5f, 2.25f, 3.0f, benchColours[19].x, benchColours[19].y, benchColours[19].z,
		1.5f, 2.25f, -2.0f, benchColours[19].x, benchColours[19].y, benchColours[19].z,
		1.5f, 2.0f, -2.0f, benchColours[19].x, benchColours[19].y, benchColours[19].z,
	};
	// start
	startColorBuilding(benchVertices, &mainBench);
	mainBench.position = vec3(currentPlanetScale.x - 1.0f, 0.0f, -(currentPlanetScale.y / 2));
	// blueprint
	vector<vec3> blueprintColours = colourVector(2, vec3(0.26f, 0.53f, 0.96f));
	vector<float> blueprintVertices = {
		-0.84375f, 0.0f, -1.5f, blueprintColours[0].x, blueprintColours[0].y, blueprintColours[0].z,
		0.84375f, 0.0f, -1.5f, blueprintColours[0].x, blueprintColours[0].y, blueprintColours[0].z,
		0.84375f, 0.0f, 1.5f, blueprintColours[0].x, blueprintColours[0].y, blueprintColours[0].z,

		-0.84375f, 0.0f, -1.5f, blueprintColours[1].x, blueprintColours[1].y, blueprintColours[1].z,
		-0.84375f, 0.0f, 1.5f, blueprintColours[1].x, blueprintColours[1].y, blueprintColours[1].z,
		0.84375f, 0.0f, 1.5f, blueprintColours[1].x, blueprintColours[1].y, blueprintColours[1].z,
	};
	startColorBuilding(blueprintVertices, &mainBlueprint);
	mainBlueprint.position = vec3(mainBench.position.x + 0.5f, 
		mainBench.position.y + 2.255f, mainBench.position.z + 0.5f);
}

void startBuildings() {
	startBuildBench();
}

void renderBuildings() {
	vector<buildingColour> allColourBuildings = { mainBench, mainBlueprint };
	int count = allColourBuildings.size();
	for (int i = 0; i < count; i++) {
		glUseProgram(playerShader);
		setMat4(playerShader, "projection", projectionMatrix());
		setMat4(playerShader, "view", viewMatrix());
		setMat4(playerShader, "model", modelMatrix(allColourBuildings[i].position,
			allColourBuildings[i].rotation, allColourBuildings[i].scale));
		glBindVertexArray(allColourBuildings[i].VAO);
		glDrawArrays(GL_TRIANGLES, 0, allColourBuildings[i].size);
	}
}
