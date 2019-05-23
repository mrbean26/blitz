#include "structures.h"
#include "player.h"
#include "shader.h"
#include "display.h"
#include "interface.h"
#include "worldGeneration.h"

int interactKey;
void StructuresBegin(){
	interactKey = stoi(inputLines[6]);
	startBuildings();
	startBuildingSelectUI();
}

void StructuresMainloop(){
	if (!WorldGeneration.active) { return; }
	renderBuildings();
	buildingInteractions();
}

int standardHouseButton;
void startBuildingSelectUI() {
	standardHouseButton = createButton();
	allButtons[standardHouseButton].texture = loadTexture("assets/images/standardHouseImage.png");
	allButtons[standardHouseButton].scale = vec2(0.5f);
	allButtons[standardHouseButton].position = vec3(-8.0, 3.75f, 0.0f);
}

void startIrregularColorBuilding(vector<float> vertices, GLuint &VAO, GLuint &VBO, GLuint &size) {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // position attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // colour attribute
	size = vertices.size() / 6;
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
	usedBuilding->size = vertices.size() / 6;
}

buildingColour mainBench, mainBlueprint;
vec2 currentBuildingScale = vec2(0.5f), currentBuildingPosition;
bool benchInUse = false;

GLuint benchUIVAO, benchUIVBO, benchUIShader, benchUITotal;
GLuint currentBuildingVAO, currentBuildingVBO, currentBuildingTotal;
void startBuildBenchUI() {
	float mountainDisplaySizeScale = 0.02f;

	vec2 area = currentPlanetScale / vec2(2.0f);
	vec3 areaColourOne = WorldGeneration.currentAreaColour;
	vec3 areaColourTwo = areaColourOne + vec3(0.03f);

	float blueprintX = (aspect_x / 10.0f) * 0.95f;
	float blueprintY = (aspect_y / 10.0f) * 0.95f;

	vector<float> vertices = {
		// blueprint
		-blueprintX, -blueprintY, 0.0f, 0.16f, 0.42f, 0.83f,
		-blueprintX, blueprintY, 0.0f, 0.16f, 0.42f, 0.83f,
		blueprintX, -blueprintY, 0.0f, 0.16f, 0.42f, 0.83f,

		-blueprintX, blueprintY, 0.0f, 0.19f, 0.46f, 0.89f,
		blueprintX, blueprintY, 0.0f, 0.19f, 0.46f, 0.89f,
		blueprintX, -blueprintY, 0.0f, 0.19f, 0.46f, 0.89f,
		// area
		-area.x / 65.0f, -area.y / 65.0f, 0.1f,
		areaColourOne.x, areaColourOne.y, areaColourOne.z,
		-area.x / 65.0f, area.y / 65.0f, 0.1f,
		areaColourOne.x, areaColourOne.y, areaColourOne.z,
		area.x / 65.0f, -area.y / 65.0f, 0.1f,
		areaColourOne.x, areaColourOne.y, areaColourOne.z,

		area.x / 65.0f, area.y / 65.0f, 0.1f,
		areaColourTwo.x, areaColourTwo.y, areaColourTwo.z,
		-area.x / 65.0f, area.y / 65.0f, 0.1f,
		areaColourTwo.x, areaColourTwo.y, areaColourTwo.z,
		area.x / 65.0f, -area.y / 65.0f, 0.1f,
		areaColourTwo.x, areaColourTwo.y, areaColourTwo.z,
	};
	// add to mountains to vertices
	int mCount = currentAllMountainPositions.size();
	for (int m = 0; m < mCount; m++) {
		vec2 pos = currentAllMountainPositions[m];
		pos = pos / vec2(65.0f);
		vec3 scale = currentAllMountainScales[m];

		float scaleX = scale.x * mountainDisplaySizeScale;
		float scaleY = scale.x * mountainDisplaySizeScale;

		vec3 one = vec3(-scaleX + pos.x, -scaleY + pos.y, 0.2f);
		vec3 two = vec3(-scaleX + pos.x, scaleY + pos.y, 0.2f);
		vec3 three = vec3(scaleX + pos.x, scaleY + pos.y, 0.2f);
		vec3 four = vec3(scaleX + pos.x, -scaleY + pos.y, 0.2f);
		vec3 whichPoint[] = { one, three };
		
		vec3 color = vec3(1.0f) - WorldGeneration.currentAreaColour;
		vec3 colourTwo = color + vec3(0.2f);
		vec3 whichColour[] = { color, colourTwo };

		for (int t = 0; t < 2; t++) {
			vec3 points[] = { whichPoint[t], two, four };
			vec3 usedColour = whichColour[t];
			for (int v = 0; v < 3; v++) {
				points[v].x = points[v].x - area.x / 65.0f;
				points[v].y = points[v].y - area.y / 65.0f;
				for (int p = 0; p < 3; p++) {
					vertices[newVectorPosFloat(&vertices)] = points[v][p];
				}
				for (int p = 0; p < 3; p++) {
					vertices[newVectorPosFloat(&vertices)] = usedColour[p];
				}
			}
		}
	}
	// memory
	startIrregularColorBuilding(vertices, benchUIVAO, benchUIVBO, benchUITotal);
	// shader
	int vertex = createShader("assets/shaders/2DUIVert.txt", GL_VERTEX_SHADER);
	int fragment = createShader("assets/shaders/2DUIFrag.txt", GL_FRAGMENT_SHADER);
	benchUIShader = createProgram({ vertex, fragment });
	// selected building square
	vec3 buildingColor = (WorldGeneration.currentAreaColour / vec3(2.0f));
	vec3 buildingColourTwo = buildingColor + vec3(0.1f);
	vector<float> vertices2 = {
		-0.1f, -0.1f, -0.5f, buildingColor.x, buildingColor.y, buildingColor.z,
		-0.1f, 0.1f, -0.5f, buildingColor.x, buildingColor.y, buildingColor.z,
		0.1f, -0.1f, -0.5f, buildingColor.x, buildingColor.y, buildingColor.z,

		-0.1f, 0.1f, -0.5f, buildingColourTwo.x, buildingColourTwo.y, buildingColourTwo.z,
		0.1f, 0.1f, -0.5f, buildingColourTwo.x, buildingColourTwo.y, buildingColourTwo.z,
		0.1f, -0.1f, -0.5f, buildingColourTwo.x, buildingColourTwo.y, buildingColourTwo.z,
	};
	startIrregularColorBuilding(vertices2, currentBuildingVAO,
		currentBuildingVBO, currentBuildingTotal);
}

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
	startBuildBenchUI();
} 

vec2 lastMouse;
void buildBenchInteraction(){
	float distanceNeeded = 10.0f;
	float distance = glm::distance(mainPlayer.position, mainBench.position);
	if (distance < distanceNeeded) {
		if (checkKeyDown(interactKey)) {
			benchInUse = !benchInUse;
			mainPlayer.canMove = !benchInUse;
			if (benchInUse) {
				currentBuildingPosition = vec2(0.0f);
				glfwSetCursorPos(window, display_x / 2.0, display_y / 2.0);
				double newX, newY;
				glfwGetCursorPos(window, &newX, &newY);
				lastMouse = vec2(newX, newY);
			}
		}
	}
	if (benchInUse) {
		double newX, newY;
		glfwGetCursorPos(window, &newX, &newY);
		
		currentBuildingPosition.x = ((-display_x / 2.0) + newX) / (display_x / 2.0);
		currentBuildingPosition.y = ((display_y / 2.0) - newY) / (display_y / (aspect_y / 5));

		currentBuildingPosition.x = glm::clamp(currentBuildingPosition.x, -1.0f, 1.0f);
		currentBuildingPosition.y = glm::clamp(currentBuildingPosition.y, (float) -aspect_y / 10.0f, (float) aspect_y / 10.0f);

		lastMouse = vec2(newX, newY);
	}
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
	if (benchInUse) {
		glUseProgram(benchUIShader);
		setMat4(benchUIShader, "model", 
			ortho(-aspect_x / 10, aspect_x / 10, -aspect_y / 10, aspect_y / 10));
		glBindVertexArray(benchUIVAO);
		glDrawArrays(GL_TRIANGLES, 0, benchUITotal);
		glLinkProgram(benchUIShader);

		mat4 model = mat4(1.0f);
		model = ortho(-aspect_x / 10, aspect_x / 10, -aspect_y / 10, aspect_y / 10);
		model = translate(model, vec3(currentBuildingPosition, 1.0f));
		model = scale(model, vec3(currentBuildingScale, 1.0f));
		setMat4(benchUIShader, "model", model);

		glBindVertexArray(currentBuildingVAO);
		glDrawArrays(GL_TRIANGLES, 0, currentBuildingTotal);
	}
}

void buildingInteractions(){
	buildBenchInteraction();
}
