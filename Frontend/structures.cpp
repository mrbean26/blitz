#include "structures.h"
#include "player.h"
#include "shader.h"
#include "display.h"
#include "interface.h"
#include "worldGeneration.h"
#include "inventory.h"
#include "people.h"

int interactKey, shootButton, aimButton;
vector<buildingColour> allColourBuildings = { mainBench, mainBlueprint };
void StructuresBegin(){
	interactKey = stoi(inputLines[6]);
	shootButton = stoi(inputLines[5]);
	aimButton = stoi(inputLines[4]);
	startBuildings();
	startBuildingSelectUI();
	startPhysicalBuildings();
	getAllBuildingPositions();
}

void StructuresMainloop(){
	if (!WorldGeneration.startedBegin) { return; }
	renderBuildings();
	buildingInteractions();
	changeBuildings();
}

vector<vec2> buildingScales = {
	vec2(0.55f, 0.55f), // standard house
	vec2(0.95f, 0.95f), // pointy house
	vec2(0.1f, 0.3f), // bench
};

vector<buildingColour> physicalBuildings;
void startPhysicalBuildings() {
	buildingColour standardHouseBuilding;
	vector<vec3> standardHouseColour = colourVector(8, vec3(0.65f, 0.35f, 0.2f));
	vector<float> standardHouseVertices = {
		// sides
		-4.0f, 0.0f, -4.0f, standardHouseColour[0].x, standardHouseColour[0].y, standardHouseColour[0].z,
		4.0f, 0.0f, -4.0f, standardHouseColour[0].x, standardHouseColour[0].y, standardHouseColour[0].z,
		4.0f, 8.0f, -4.0f, standardHouseColour[0].x, standardHouseColour[0].y, standardHouseColour[0].z,

		-4.0f, 0.0f, -4.0f, standardHouseColour[1].x, standardHouseColour[1].y, standardHouseColour[1].z,
		-4.0f, 8.0f, -4.0f, standardHouseColour[1].x, standardHouseColour[1].y, standardHouseColour[1].z,
		4.0f, 8.0f, -4.0f, standardHouseColour[1].x, standardHouseColour[1].y, standardHouseColour[1].z,

		-4.0f, 0.0f, 4.0f, standardHouseColour[2].x, standardHouseColour[2].y, standardHouseColour[2].z,
		4.0f, 0.0f, 4.0f, standardHouseColour[2].x, standardHouseColour[2].y, standardHouseColour[2].z,
		4.0f, 8.0f, 4.0f, standardHouseColour[2].x, standardHouseColour[2].y, standardHouseColour[2].z,

		-4.0f, 0.0f, 4.0f, standardHouseColour[3].x, standardHouseColour[3].y, standardHouseColour[3].z,
		-4.0f, 8.0f, 4.0f, standardHouseColour[3].x, standardHouseColour[3].y, standardHouseColour[3].z,
		4.0f, 8.0f, 4.0f, standardHouseColour[3].x, standardHouseColour[3].y, standardHouseColour[3].z,
		// back
		4.0f, 0.0f, -4.0f, standardHouseColour[4].x, standardHouseColour[4].y, standardHouseColour[4].z,
		4.0f, 8.0f, -4.0f, standardHouseColour[4].x, standardHouseColour[4].y, standardHouseColour[4].z,
		4.0f, 0.0f, 4.0f, standardHouseColour[4].x, standardHouseColour[4].y, standardHouseColour[4].z,

		4.0f, 8.0f, -4.0f, standardHouseColour[5].x, standardHouseColour[5].y, standardHouseColour[5].z,
		4.0f, 8.0f, 4.0f, standardHouseColour[5].x, standardHouseColour[5].y, standardHouseColour[5].z,
		4.0f, 0.0f, 4.0f, standardHouseColour[5].x, standardHouseColour[5].y, standardHouseColour[5].z,
		// front w door & window
		-4.0f, 0.0f, -4.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 8.0f, -4.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 0.0f, -3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,

		-4.0f, 8.0f, -4.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 8.0f, -3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 0.0f, -3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,

		-4.0f, 0.0f, 1.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 8.0f, 1.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 0.0f, 0.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,

		-4.0f, 8.0f, 1.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 8.0f, 0.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 0.0f, 0.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,

		-4.0f, 0.0f, 4.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 8.0f, 4.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 0.0f, 3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,

		-4.0f, 8.0f, 4.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 8.0f, 3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 0.0f, 3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		// top & bottom of window
		-4.0f, 4.5f, 1.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 8.0f, 1.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 4.5f, 3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,

		-4.0f, 8.0f, 1.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 8.0f, 3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 4.5f, 3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,

		-4.0f, 0.0f, 1.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 2.0f, 1.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 0.0f, 3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,

		-4.0f, 2.0f, 1.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 2.0f, 3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 0.0f, 3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		// top of door		
		-4.0f, 5.0f, -3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 8.0f, -3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 5.0f, 0.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,

		-4.0f, 8.0f, -3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 8.0f, 0.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 5.0f, 0.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		// roof
		-4.0f, 8.0f, -4.0f, standardHouseColour[7].x, standardHouseColour[7].y, standardHouseColour[7].z,
		-4.0f, 8.0f, 4.0f, standardHouseColour[7].x, standardHouseColour[7].y, standardHouseColour[7].z,
		4.0f, 8.0f, -4.0f, standardHouseColour[7].x, standardHouseColour[7].y, standardHouseColour[7].z,

		4.0f, 8.0f, -4.0f, standardHouseColour[7].x, standardHouseColour[7].y, standardHouseColour[7].z,
		-4.0f, 8.0f, 4.0f, standardHouseColour[7].x, standardHouseColour[7].y, standardHouseColour[7].z,
		4.0f, 8.0f, 4.0f, standardHouseColour[7].x, standardHouseColour[7].y, standardHouseColour[7].z,
		// window
		-4.0f, 3.5f, 1.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 3.5f, 3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 3.0f, 1.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,

		-4.0f, 3.0f, 1.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 3.0f, 3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 3.5f, 3.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,

		-4.0f, 4.5f, 2.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 4.5f, 2.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 2.0f, 2.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,

		-4.0f, 4.5f, 2.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 2.0f, 2.5f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
		-4.0f, 2.0f, 2.0f, standardHouseColour[6].x, standardHouseColour[6].y, standardHouseColour[6].z,
	};
	startColorBuilding(standardHouseVertices, &standardHouseBuilding);
	standardHouseBuilding.scale = vec3(0.88f);
	standardHouseBuilding.buildingType = 0;
	physicalBuildings[newVectorPos(&physicalBuildings)] = standardHouseBuilding;
	// point house
	buildingColour pointHouseBuilding;
	vector<vec3> pointHouseColour = colourVector(11, vec3(0.42f, 0.22f, 0.10f));
	vector<float> pointHouseVertices = {
		// sides
		-4.0f, 0.0f, -4.0f, pointHouseColour[0].x, pointHouseColour[0].y, pointHouseColour[0].z,
		4.0f, 0.0f, -4.0f, pointHouseColour[0].x, pointHouseColour[0].y, pointHouseColour[0].z,
		4.0f, 8.0f, -4.0f, pointHouseColour[0].x, pointHouseColour[0].y, pointHouseColour[0].z,

		-4.0f, 0.0f, -4.0f, pointHouseColour[1].x, pointHouseColour[1].y, pointHouseColour[1].z,
		-4.0f, 8.0f, -4.0f, pointHouseColour[1].x, pointHouseColour[1].y, pointHouseColour[1].z,
		4.0f, 8.0f, -4.0f, pointHouseColour[1].x, pointHouseColour[1].y, pointHouseColour[1].z,

		-4.0f, 0.0f, 4.0f, pointHouseColour[2].x, pointHouseColour[2].y, pointHouseColour[2].z,
		4.0f, 0.0f, 4.0f, pointHouseColour[2].x, pointHouseColour[2].y, pointHouseColour[2].z,
		4.0f, 8.0f, 4.0f, pointHouseColour[2].x, pointHouseColour[2].y, pointHouseColour[2].z,

		-4.0f, 0.0f, 4.0f, pointHouseColour[3].x, pointHouseColour[3].y, pointHouseColour[3].z,
		-4.0f, 8.0f, 4.0f, pointHouseColour[3].x, pointHouseColour[3].y, pointHouseColour[3].z,
		4.0f, 8.0f, 4.0f, pointHouseColour[3].x, pointHouseColour[3].y, pointHouseColour[3].z,
		// back
		4.0f, 0.0f, -4.0f, pointHouseColour[4].x, pointHouseColour[4].y, pointHouseColour[4].z,
		4.0f, 8.0f, -4.0f, pointHouseColour[4].x, pointHouseColour[4].y, pointHouseColour[4].z,
		4.0f, 0.0f, 4.0f, pointHouseColour[4].x, pointHouseColour[4].y, pointHouseColour[4].z,

		4.0f, 8.0f, -4.0f, pointHouseColour[5].x, pointHouseColour[5].y, pointHouseColour[5].z,
		4.0f, 8.0f, 4.0f, pointHouseColour[5].x, pointHouseColour[5].y, pointHouseColour[5].z,
		4.0f, 0.0f, 4.0f, pointHouseColour[5].x, pointHouseColour[5].y, pointHouseColour[5].z,
		// front w door
		-4.0f, 0.0f, -4.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,
		-4.0f, 8.0f, -4.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,
		-4.0f, 0.0f, -2.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,

		-4.0f, 8.0f, -4.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,
		-4.0f, 8.0f, -2.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,
		-4.0f, 0.0f, -2.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,

		-4.0f, 0.0f, 4.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,
		-4.0f, 8.0f, 4.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,
		-4.0f, 0.0f, 2.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,

		-4.0f, 8.0f, 4.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,
		-4.0f, 8.0f, 2.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,
		-4.0f, 0.0f, 2.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,

		-4.0f, 5.0f, -2.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,
		-4.0f, 8.0f, -2.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,
		-4.0f, 5.0f, 2.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,

		-4.0f, 8.0f, -2.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,
		-4.0f, 8.0f, 2.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,
		-4.0f, 5.0f, 2.0f, pointHouseColour[6].x, pointHouseColour[6].y, pointHouseColour[6].z,
		// roof
		6.0f, 6.0f, -6.0f, pointHouseColour[7].x, pointHouseColour[7].y, pointHouseColour[7].z,
		-6.0f, 6.0f, -6.0f, pointHouseColour[7].x, pointHouseColour[7].y, pointHouseColour[7].z,
		0.0f, 12.0f, 0.0f, pointHouseColour[7].x, pointHouseColour[7].y, pointHouseColour[7].z,

		6.0f, 6.0f, 6.0f, pointHouseColour[8].x, pointHouseColour[8].y, pointHouseColour[8].z,
		-6.0f, 6.0f, 6.0f, pointHouseColour[8].x, pointHouseColour[8].y, pointHouseColour[8].z,
		0.0f, 12.0f, 0.0f, pointHouseColour[8].x, pointHouseColour[8].y, pointHouseColour[8].z,

		-6.0f, 6.0f, 6.0f, pointHouseColour[9].x, pointHouseColour[9].y, pointHouseColour[9].z,
		-6.0f, 6.0f, -6.0f, pointHouseColour[9].x, pointHouseColour[9].y, pointHouseColour[9].z,
		0.0f, 12.0f, 0.0f, pointHouseColour[9].x, pointHouseColour[9].y, pointHouseColour[9].z,

		6.0f, 6.0f, 6.0f, pointHouseColour[10].x, pointHouseColour[10].y, pointHouseColour[10].z,
		6.0f, 6.0f, -6.0f, pointHouseColour[10].x, pointHouseColour[10].y, pointHouseColour[10].z,
		0.0f, 12.0f, 0.0f, pointHouseColour[10].x, pointHouseColour[10].y, pointHouseColour[10].z,
	};
	startColorBuilding(pointHouseVertices, &pointHouseBuilding);
	pointHouseBuilding.buildingType = 1;
	physicalBuildings[newVectorPos(&physicalBuildings)] = pointHouseBuilding;
}

int standardHouseButton, pointyHouseButton;
vector<int> buildingSelectButtons;
int currentBuildingType = 0;
void getAllBuildingPositions(){
	vector<string> allLines = readLines(WorldGeneration.worldLinesPath);
	int lineCount = allLines.size();
	int saveType = getIntFile(WorldGeneration.worldLinesPath, "saveType");
	for (int l = 0; l < lineCount; l++) {
		int buildType = getIntFile(WorldGeneration.worldLinesPath,
			WorldGeneration.currentAreaPrefix + "BuildingType", l);
		if (buildType != -1) {
			vec2 areaScaleMax = vec2((currentPlanetScale.x) / divider, (currentPlanetScale.y) / divider);

			vec2 buildPos = getVec2File(WorldGeneration.worldLinesPath,
				WorldGeneration.currentAreaPrefix + "BuildingPosition", l + 1);
			vec2 buildUIPos = vec2(
				((buildPos.x / currentPlanetScale.x) * areaScaleMax.x) - (areaScaleMax.x / 2.0f),
				-(((buildPos.y / currentPlanetScale.y) * areaScaleMax.y) + areaScaleMax.y / 2.0f)
			);
			float buildRot = getFloatFile(WorldGeneration.worldLinesPath,
				WorldGeneration.currentAreaPrefix + "BuildingRotation", l + 2);
			placedMiniBuilding newBuilding;
			newBuilding.position = buildUIPos;
			newBuilding.scale = buildingScales[buildType];
			if(saveType == 1){
				newBuilding.scale = buildingScales[buildType] * vec2(65.0f / 200.0f);
			}
			newBuilding.rotation = vec3(0.0f, buildRot, 0.0f);
			allMiniBuildings[newVectorPos(&allMiniBuildings)] = newBuilding;

			if (buildType == 2) {
				continue; // bench already being rendered
			}

			// physical buildngs
			buildingColour newBuildingPhysical = physicalBuildings[buildType];
			newBuildingPhysical.position = vec3(buildPos.x, 0.0f, buildPos.y);
			newBuildingPhysical.rotation = vec3(0.0f, buildRot, 0.0f);
			allColourBuildings[newVectorPos(&allColourBuildings)] = newBuildingPhysical;
		}
	}
}

void changeBuildings() {
	int buttonCount = buildingSelectButtons.size();
	for (int i = 0; i < buttonCount; i++) {
		allButtons[buildingSelectButtons[i]].colour = vec3(1.0f);
		if (i == currentBuildingType) {
			allButtons[buildingSelectButtons[i]].colour = vec3(0.86f, 0.27f, 0.32f);
		}
		if (allButtons[buildingSelectButtons[i]].clickUp) {
			allButtons[buildingSelectButtons[i]].colour = vec3(0.86f, 0.27f, 0.32f);
			currentBuildingType = i;
		}
	}
}

void startBuildingSelectUI() {
	buildingSelectButtons.clear();
	int empty = createButton();
	allButtons[empty].position = vec3(1000.0f);

	standardHouseButton = createButton();
	allButtons[standardHouseButton].texture = loadTexture("assets/images/standardHouseImage.png");
	allButtons[standardHouseButton].scale = vec2(0.5f);
	allButtons[standardHouseButton].position = vec3(-8.0, 3.75f, 0.0f);
	allButtons[standardHouseButton].active = false;
	buildingSelectButtons[newVectorPos(&buildingSelectButtons)] = standardHouseButton;

	pointyHouseButton = createButton();
	allButtons[pointyHouseButton].texture = loadTexture("assets/images/pointRoofHouse.png");
	allButtons[pointyHouseButton].scale = vec2(0.5f);
	allButtons[pointyHouseButton].position = vec3(-8.0f, 1.5f, 0.0f);
	allButtons[pointyHouseButton].active = false;
	buildingSelectButtons[newVectorPos(&buildingSelectButtons)] = pointyHouseButton;
}

void getMountainLimits(vector<float> vertices){
	for (int p = 0; p < 4; p++) { // all limits
		float x = vertices[p * 3];
		float y = vertices[(p * 3) + 1];
		vec2 limit = vec2(x, y);

		mountainLimits[newVectorPos(&mountainLimits)] = limit;
	}
}

bool insideOtherBuilding(){
	vec2 buildPosMin = vec2(currentBuildingPosition.x - (0.1f * currentBuildingScale.x),
		currentBuildingPosition.y - (0.1f * currentBuildingScale.y));
	vec2 buildPosMax = vec2(currentBuildingPosition.x + (0.1f * currentBuildingScale.x),
		currentBuildingPosition.y + (0.1f * currentBuildingScale.y));

	int mCount = allMiniBuildings.size();
	for (int m = 0; m < mCount; m++) {
		placedMiniBuilding current = allMiniBuildings[m];
		vec2 minMin = current.position - (current.scale * 0.1f);
		vec2 maxMax = current.position + (current.scale * 0.1f);

		if (buildPosMin.x >= minMin.x && buildPosMin.x <= maxMax.x) {
			if (buildPosMin.y >= minMin.y && buildPosMin.y <= maxMax.y) {
				return true;
			}
		}

		if (buildPosMax.x >= minMin.x && buildPosMax.x <= maxMax.x) {
			if (buildPosMax.y >= minMin.y && buildPosMax.y <= maxMax.y) {
				return true;
			}
		}

		if (buildPosMax.x >= minMin.x && buildPosMin.x <= maxMax.x) {
			if (buildPosMax.y >= minMin.y && buildPosMin.y <= maxMax.y) {
				return true;
			}
		}
	}
	return false;
}

bool insideAreaScale() {
	vec2 areaScaleMin = vec2((-currentPlanetScale.x / 2.0f) / divider, (-currentPlanetScale.y / 2.0f) / divider);
	vec2 areaScaleMax = vec2((currentPlanetScale.x / 2.0f) / divider, (currentPlanetScale.y / 2.0f) / divider);

	vec2 buildPosMin = vec2(currentBuildingPosition.x - (0.1f * currentBuildingScale.x),
		currentBuildingPosition.y - (0.1f * currentBuildingScale.y));
	vec2 buildPosMax = vec2(currentBuildingPosition.x + (0.1f * currentBuildingScale.x),
		currentBuildingPosition.y + (0.1f * currentBuildingScale.y));

	if (buildPosMin.x >= areaScaleMin.x && buildPosMax.x <= areaScaleMax.x) {
		if (buildPosMin.y >= areaScaleMin.y && buildPosMax.y <= areaScaleMax.y) {
			return true;
		}
	}
	return false;
}

bool insideMountain(){
	vec2 buildPosMin = vec2(currentBuildingPosition.x - (0.1f * currentBuildingScale.x),
		currentBuildingPosition.y - (0.1f * currentBuildingScale.y));
	vec2 buildPosMax = vec2(currentBuildingPosition.x + (0.1f * currentBuildingScale.x),
		currentBuildingPosition.y + (0.1f * currentBuildingScale.y));

	int mCount = mountainLimits.size();
	for (int m = 0; m < mCount / 4; m++) {
		vec2 minMin = mountainLimits[m * 4];
		vec2 maxMax = mountainLimits[(m * 4) + 3];

		if (buildPosMin.x >= minMin.x && buildPosMin.x <= maxMax.x) {
			if (buildPosMin.y >= minMin.y && buildPosMin.y <= maxMax.y) {
				return true;
			}
		}

		if (buildPosMax.x >= minMin.x && buildPosMax.x <= maxMax.x) {
			if (buildPosMax.y >= minMin.y && buildPosMax.y <= maxMax.y) {
				return true;
			}
		}

		if (buildPosMax.x >= minMin.x && buildPosMin.x <= maxMax.x) {
			if (buildPosMax.y >= minMin.y && buildPosMin.y <= maxMax.y) {
				return true;
			}
		}
	}
	return false;
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
vec2 currentBuildingScale = vec2(0.5f), currentBuildingPosition, currentBuildingRotation;
bool benchInUse = false;

GLuint benchUIVAO, benchUIVBO, benchUIShader, benchUITotal;
GLuint currentBuildingVAO, currentBuildingVBO, currentBuildingTotal;
vector<vec2> mountainLimits;
int saveType = 0;
float divider = 65.0f;
void startBuildBenchUI() {
	float mountainDisplaySizeScale = 0.034f;
	saveType = getIntFile(WorldGeneration.worldLinesPath, "saveType");
	if(saveType == 1){
		mountainDisplaySizeScale = mountainDisplaySizeScale * (65.0f / 200.0f);
	}

	vec2 area = currentPlanetScale / vec2(2.0f);
	vec3 areaColourOne = WorldGeneration.currentAreaColour;
	vec3 areaColourTwo = areaColourOne + vec3(0.03f);

	float blueprintX = ((float) aspect_x / 10.0f) * 0.95f;
	float blueprintY = ((float) aspect_y / 10.0f) * 0.95f;

	divider = 65.0f;
	if(saveType == 1){
		divider = 200.0f;
	}
    
	vector<float> vertices = {
		// blueprint
		-blueprintX, -blueprintY, 0.0f, 0.16f, 0.42f, 0.83f,
		-blueprintX, blueprintY, 0.0f, 0.16f, 0.42f, 0.83f,
		blueprintX, -blueprintY, 0.0f, 0.16f, 0.42f, 0.83f,

		-blueprintX, blueprintY, 0.0f, 0.19f, 0.46f, 0.89f,
		blueprintX, blueprintY, 0.0f, 0.19f, 0.46f, 0.89f,
		blueprintX, -blueprintY, 0.0f, 0.19f, 0.46f, 0.89f,
		// area
		-area.x / divider, -area.y / divider, 0.1f,
		areaColourOne.x, areaColourOne.y, areaColourOne.z,
		-area.x / divider, area.y / divider, 0.1f,
		areaColourOne.x, areaColourOne.y, areaColourOne.z,
		area.x / divider, -area.y / divider, 0.1f,
		areaColourOne.x, areaColourOne.y, areaColourOne.z,

		area.x / divider, area.y / divider, 0.1f,
		areaColourTwo.x, areaColourTwo.y, areaColourTwo.z,
		-area.x / divider, area.y / divider, 0.1f,
		areaColourTwo.x, areaColourTwo.y, areaColourTwo.z,
		area.x / divider, -area.y / divider, 0.1f,
		areaColourTwo.x, areaColourTwo.y, areaColourTwo.z,
	};
	// add to mountains to vertices
	int mCount = currentAllMountainPositions.size();
	for (int m = 0; m < mCount; m++) {
		vec2 pos = currentAllMountainPositions[m];
		pos = pos / vec2(divider);
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

		vector<float> mountainVertices;
		for (int t = 0; t < 2; t++) {
			vec3 points[] = { whichPoint[t], two, four };
			vec3 usedColour = whichColour[t];
			for (int v = 0; v < 3; v++) {
				points[v].x = points[v].x - area.x / divider;
				points[v].y = points[v].y - area.y / divider;
				for (int p = 0; p < 3; p++) {
					vertices[newVectorPos(&vertices)] = points[v][p];
					mountainVertices[newVectorPos(&mountainVertices)] = points[v][p];
				}
				for (int p = 0; p < 3; p++) {
					vertices[newVectorPos(&vertices)] = usedColour[p];
				}
			}
		}
		getMountainLimits(mountainVertices);
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
		// door
		-0.1f, -0.06f, -0.5f, 1.0f, 1.0f, 1.0f,
		-0.075f, -0.06f, -0.5f, 1.0f, 1.0f, 1.0f,
		-0.1f, 0.06f, -0.5f, 1.0f, 1.0f, 1.0f,

		-0.1f, 0.06f, -0.5f, 1.0f, 1.0f, 1.0f,
		-0.075f, 0.06f, -0.5f, 1.0f, 1.0f, 1.0f,
		-0.075f, -0.06f, -0.5f, 1.0f, 1.0f, 1.0f,
		// building
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

bool okToBuild() {
	if (insideAreaScale()) {
		if (!insideMountain()) {
			if (!insideOtherBuilding()) {
				return true;
			}
		}
	}
	return false;
}

vec2 lastMouse;
vector<placedMiniBuilding> allMiniBuildings;
vector<string> newBuildingLines;

void buildBenchInteraction(){
	currentBuildingScale = buildingScales[currentBuildingType];
	if(saveType == 1){
		currentBuildingScale = buildingScales[currentBuildingType] * (65.0f / 200.0f);
	}
	float distance = glm::distance(mainPlayer.position, mainBench.position);
	if (distance < BENCH_DISTANCE) {
		if (checkKeyDown(interactKey) && !mainPlayer.insideRocketFixed) {
			benchInUse = !benchInUse;
			mainPlayer.canMove = !benchInUse;
			int size = buildingSelectButtons.size();
			for (int b = 0; b < size; b++) {
				allButtons[buildingSelectButtons[b]].active = false;
			}
			if(!benchInUse){
				for(int s = 0; s < HOTBAR_COUNT; s++){
					allButtons[allSlots[s].buttonIndex].active = true;
					if(allSlots[s].itemType != -1){
						allButtons[allSlots[s].buttonIconIndex].active = true;
					}
					allTexts[allSlotTexts[s]].active = true;
				}
			}
			if (benchInUse) {
				for (int b = 0; b < size; b++) {
					allButtons[buildingSelectButtons[b]].active = true;
				}
				for(int s = 0; s < SLOT_COUNT; s++){
					allButtons[allSlots[s].buttonIndex].active = false;
					if(allSlots[s].itemType != -1){
						allButtons[allSlots[s].buttonIconIndex].active = false;
					}
					allTexts[allSlotTexts[s]].active = false;
				}	
				currentBuildingPosition = vec2(0.0f);
				glfwSetCursorPos(window, display_x / 2.0, display_y / 2.0);

				lastMouse = vec2(mousePosX, mousePosY);
			}
		}
	}
	if (benchInUse) {	
		currentBuildingPosition.x = (((float) -display_x / 2.0f) + (float) mousePosX) /
			((float) display_x / 2.0f);
		currentBuildingPosition.y = ((float) (display_y / 2.0f) - (float)mousePosY) /
			((float) display_y / ((float) aspect_y / 5));

		currentBuildingPosition.x = glm::clamp(currentBuildingPosition.x, -1.0f, 1.0f);
		currentBuildingPosition.y = glm::clamp(currentBuildingPosition.y, (float) -aspect_y / 10.0f, (float) aspect_y / 10.0f);

		lastMouse = vec2(mousePosX, mousePosY);

		// clicks
		if (checkKeyDown(shootButton)) {
			currentBuildingRotation.y -= 90.0f;
		}
		if (checkKeyDown(aimButton)) {
			if (okToBuild()) {
				placedMiniBuilding newMiniBuilding;
				newMiniBuilding.position = currentBuildingPosition;
				newMiniBuilding.rotation.y = currentBuildingRotation.y;
				newMiniBuilding.scale = currentBuildingScale;

				allMiniBuildings[newVectorPos(&allMiniBuildings)] = newMiniBuilding;
				// lines to save
				if (saveType == LARGE_WORLD) {
					divider = 200.0f;
				}
				vec2 areaScaleMax = vec2(currentPlanetScale.x / divider, currentPlanetScale.y / divider);
				vec2 buildWorldPos = vec2(((currentBuildingPosition.x / areaScaleMax.x) * currentPlanetScale.x) + currentPlanetScale.x / 2.0f,
					((currentBuildingPosition.y / areaScaleMax.y) * currentPlanetScale.y) + currentPlanetScale.y / 2.0f);

				if (saveType == LARGE_WORLD) {
					float x = (currentBuildingPosition.x + (currentPlanetScale.x / 200.0f) / 2.0f) / areaScaleMax.x;
					float y = (currentBuildingPosition.y + (currentPlanetScale.y / 200.0f) / 2.0f) / areaScaleMax.y;

					buildWorldPos.x = x * currentPlanetScale.x;
					buildWorldPos.y = y * currentPlanetScale.y;
				}

				string buildTypeLine = WorldGeneration.currentAreaPrefix + "BuildingType " +
					to_string(currentBuildingType);
				string buildPosLine = WorldGeneration.currentAreaPrefix + "BuildingPosition " +
					to_string(buildWorldPos.x) + " " + to_string(-buildWorldPos.y);
				string buildRotLine = WorldGeneration.currentAreaPrefix + "BuildingRotation " +
					to_string(currentBuildingRotation.y);

				newBuildingLines[newVectorPos(&newBuildingLines)] = buildTypeLine;
				newBuildingLines[newVectorPos(&newBuildingLines)] = buildPosLine;
				newBuildingLines[newVectorPos(&newBuildingLines)] = buildRotLine;

				// physical
				buildingColour newBuildingPhysical = physicalBuildings[currentBuildingType];
				newBuildingPhysical.position = vec3(buildWorldPos.x, 0.0f, -buildWorldPos.y);
				newBuildingPhysical.rotation = vec3(0.0f, currentBuildingRotation.y, 0.0f);
				allColourBuildings[newVectorPos(&allColourBuildings)] = newBuildingPhysical;

				// create owner
				int personType = 0;
				if (WorldGeneration.currentAreaPrefix == "earth") {
					if (currentBuildingType == 0) {
						personType = 1;
					}
					if (currentBuildingType == 1) {
						personType = 0;
					}
				}
				int newPerson = createPlayer(personType, vec3(buildWorldPos.x, 0.0f, -buildWorldPos.y));
				allPeople[newPerson].rotation.y = currentBuildingRotation.y + 90.0f;
			}
		}
	}
}

void startBuildings() {
	startBuildBench();
	allColourBuildings.clear();
	mainBench.buildingType = 2;
	allColourBuildings = { mainBench, mainBlueprint };
}

int emptyInt = 0;
float emptyFloat = 0.0f;
bool emptyBool = false;
bool emptyBoolTwo = false;

void buildCollisions(vec3 &position, int &insideBuildingIndex, float &jumpVelocity, bool &lastOnBench, bool &hitBuilding, float playerDifference){
	// outside of buildings
	int buildingCount = allColourBuildings.size();
	float playerYDifference = 0.5f;
	bool onBenchCurrent = false;
	insideBuildingIndex = -1;
	for (int b = 0; b < buildingCount; b++) {
		buildingColour current = allColourBuildings[b];
		vec3 pos = current.position;
		vec3 sca = current.scale;
		vec3 rot = current.rotation;
		rot.y = rot.y - (floor((rot.y / 360.0f)) * 360.0f);
		if (current.buildingType == 0) { // normal house
			// check if inside for camera collisions
			if (position.x >= pos.x - 4.0f * sca.x && position.x <= pos.x + 4.0f * sca.x) {
				if (position.z >= pos.z - 4.0f * sca.z && position.z <= pos.z + 4.0f * sca.z) {
					insideBuildingIndex = b;
				}
			}
            // outside walls
			if (position.x >= pos.x - 4.0f * sca.x - playerDifference && position.x <= pos.x + 4.0f * sca.x) {
				if (position.z >= pos.z + 4.0f * sca.z && position.z <= playerDifference + pos.z + 4.0f * sca.z) {
					// 270 deg
					if (radians(90.0f) == radians(rot.y)) {
						if (position.x >= pos.x + 0.5f * sca.x) {
							position.z = playerDifference + pos.z + 4.0f * sca.z;
							hitBuilding = true;
						}
					}
					else {
						position.z = playerDifference + pos.z + 4.0f * sca.z;
						hitBuilding = true;
					}
				}
				if (position.z <= pos.z - 4.0f * sca.z && position.z >= -playerDifference + pos.z - 4.0f * sca.z) {
					// 90 deg
					if (radians(270.0f) == radians(rot.y)) {
						if (position.x <= pos.x - 0.5f * sca.x) {
							position.z = -playerDifference + pos.z - 4.0f * sca.z;
							hitBuilding = true;
						}
					}
					else {
						position.z = -playerDifference + pos.z - 4.0f * sca.z;
						hitBuilding = true;
					}
				}
			}
			if (position.z >= pos.z - 4.0f * sca.z - playerDifference && position.z <= pos.z + 4.0f * sca.z) {
				if (position.x >= pos.x + 4.0f * sca.x && position.x <= playerDifference + pos.x + 4.0f * sca.x) {
					// 180 deg
					if (radians(180.0f) == radians(rot.y)) {
						if (position.z <= pos.z - 0.5f * sca.z) {
							position.x = playerDifference + pos.x + 4.0f * sca.x;
							hitBuilding = true;
						}
					}
					else {
						position.x = playerDifference + pos.x + 4.0f * sca.x;
						hitBuilding = true;
					}
				}
				if (position.x <= pos.x - 4.0f * sca.x && position.x >= -playerDifference + pos.x - 4.0f * sca.x) {
					// 0 deg
					if (radians(0.0f) == radians(rot.y)) {
						if (position.z >= pos.z + 0.5f * sca.z) {
							position.x = -playerDifference + pos.x - 4.0f * sca.x;
							hitBuilding = true;
						}
					}
					else {
						position.x = -playerDifference + pos.x - 4.0f * sca.x;
						hitBuilding = true;
					}
				}
			}
			// inside walls
			if (position.x >= pos.x - 4.0f * sca.x && position.x <= pos.x + 4.0f * sca.x) {
				if (position.z <= pos.z + 4.0f * sca.z && position.z >= -playerDifference + pos.z + 4.0f * sca.z) {
					// 270 deg
					if (radians(90.0f) == radians(rot.y)) {
						if (position.x >= pos.x + 0.5f * sca.x) {
							position.z = -playerDifference + pos.z + 4.0f * sca.z;
							hitBuilding = true;
						}
					}
					else {
						position.z = -playerDifference + pos.z + 4.0f * sca.z;
						hitBuilding = true;
					}
				}
				if (position.z >= pos.z - 4.0f * sca.z && position.z <= playerDifference + pos.z - 4.0f * sca.z) {
					// 90 deg
					if (radians(270.0f) == radians(rot.y)) {
						if (position.x <= pos.x - 0.5f * sca.x) {
							position.z = playerDifference + pos.z - 4.0f * sca.z;
							hitBuilding = true;
						}
					}
					else {
						position.z = playerDifference + pos.z - 4.0f * sca.z;
						hitBuilding = true;
					}
				}
			}
			if (position.z >= pos.z - 4.0f * sca.z && position.z <= pos.z + 4.0f * sca.z) {
				if (position.x <= pos.x + 4.0f * sca.x && position.x >= -playerDifference + pos.x + 4.0f * sca.x) {
					// 180 deg
					if (radians(180.0f) == radians(rot.y)) {
						if (position.z <= pos.z - 0.5f * sca.z) {
							position.x = -playerDifference + pos.x + 4.0f * sca.x;
							hitBuilding = true;
						}
					}
					else {
						position.x = -playerDifference + pos.x + 4.0f * sca.x;
						hitBuilding = true;
					}
				}
				if (position.x >= pos.x - 4.0f * sca.x && position.x <= playerDifference + pos.x - 4.0f * sca.x) {
					// 0 deg
					if (radians(0.0f) == radians(rot.y)) {
						if (position.z >= pos.z + 0.5f * sca.z) {
							position.x = playerDifference + pos.x - 4.0f * sca.x;
							hitBuilding = true;
						}
					}
					else {
						position.x = playerDifference + pos.x - 4.0f * sca.x;
						hitBuilding = true;
					}
				}
			}
		}
		if (current.buildingType == 1) { // pointy house
			// outside walls
			if (position.x >= pos.x - 4.0f * sca.x && position.x <= pos.x + 4.0f * sca.x) {
				if (position.z >= pos.z + 4.0f * sca.z && position.z <= playerDifference + pos.z + 4.0f * sca.z) {
					// 270 deg
					if (radians(90.0f) == radians(rot.y)) {
						if (position.x >= pos.x - playerDifference + 2.0f * sca.x || position.x <= pos.x + playerDifference - 2.0f * sca.x) {
							position.z = playerDifference + pos.z + 4.0f * sca.z;
							hitBuilding = true;
						}
					}
					else {
						position.z = playerDifference + pos.z + 4.0f * sca.z;
						hitBuilding = true;
					}
				}
				if (position.z <= pos.z - 4.0f * sca.z && position.z >= -playerDifference + pos.z - 4.0f * sca.z) {
					// 90 deg
					if (radians(270.0f) == radians(rot.y)) {
						if (position.x <= pos.x + playerDifference - 2.0f * sca.x || position.x >= pos.x - playerDifference + 2.0f * sca.x) {
							position.z = -playerDifference + pos.z - 4.0f * sca.z;
							hitBuilding = true;
						}
					}
					else {
						position.z = -playerDifference + pos.z - 4.0f * sca.z;
						hitBuilding = true;
					}
				}
			}
			if (position.z >= pos.z - 4.0f * sca.z && position.z <= pos.z + 4.0f * sca.z) {
				if (position.x >= pos.x + 4.0f * sca.x && position.x <= playerDifference + pos.x + 4.0f * sca.x) {
					// 180 deg
					if (radians(180.0f) == radians(rot.y)) {
						if (position.z <= pos.z + playerDifference - 2.0f * sca.z || position.z >= pos.z - playerDifference + 2.0f * sca.z) {
							position.x = playerDifference + pos.x + 4.0f * sca.x;
							hitBuilding = true;
						}
					}
					else {
						position.x = playerDifference + pos.x + 4.0f * sca.x;
						hitBuilding = true;
					}
				}
				if (position.x <= pos.x - 4.0f * sca.x && position.x >= -playerDifference + pos.x - 4.0f * sca.x) {
					// 0 deg
					if (radians(0.0f) == radians(rot.y)) {
						if (position.z >= pos.z - playerDifference + 2.0f * sca.z || position.z <= pos.z + playerDifference - 2.0f * sca.z) {
							position.x = -playerDifference + pos.x - 4.0f * sca.x;
							hitBuilding = true;
						}
					}
					else {
						position.x = -playerDifference + pos.x - 4.0f * sca.x;
						hitBuilding = true;
					}
				}
			}
			if (position.x >= pos.x - 4.0f * sca.x && position.x <= pos.x + 4.0f * sca.x) {
				if (position.z <= pos.z + 4.0f * sca.z && position.z >= pos.z - 4.0f * sca.z) {
					insideBuildingIndex = b;
				}
			}
			// inside walls
			if (position.x >= pos.x - 4.0f * sca.x && position.x <= pos.x + 4.0f * sca.x) {
				if (position.z <= pos.z + 4.0f * sca.z && position.z >= -playerDifference + pos.z + 4.0f * sca.z) {
					// 270 deg
					if (radians(90.0f) == radians(rot.y)) {
						if (position.x >= pos.x - playerDifference + 2.0f * sca.x || position.x <= pos.x + playerDifference - 2.0f * sca.x) {
							position.z = -playerDifference + pos.z + 4.0f * sca.z;
							hitBuilding = true;
						}
					}
					else {
						position.z = -playerDifference + pos.z + 4.0f * sca.z;
						hitBuilding = true;
					}
				}
				if (position.z >= pos.z - 4.0f * sca.z && position.z <= playerDifference + pos.z - 4.0f * sca.z) {
					// 90 deg
					if (radians(270.0f) == radians(rot.y)) {
						if (position.x <= pos.x + playerDifference - 2.0f * sca.x || position.x >= pos.x - playerDifference + 2.0f * sca.x) {
							position.z = playerDifference + pos.z - 4.0f * sca.z;
							hitBuilding = true;
						}
					}
					else {
						position.z = playerDifference + pos.z - 4.0f * sca.z;
						hitBuilding = true;
					}
				}
			}
			if (position.z >= pos.z - 4.0f * sca.z && position.z <= pos.z + 4.0f * sca.z) {
				if (position.x <= pos.x + 4.0f * sca.x && position.x >= -playerDifference + pos.x + 4.0f * sca.x) {
					// 180 deg
					if (radians(180.0f) == radians(rot.y)) {
						if (position.z <= pos.z + playerDifference - 2.0f * sca.z || position.z >= pos.z - playerDifference + 2.0f * sca.z) {
							position.x = -playerDifference + pos.x + 4.0f * sca.x;
							hitBuilding = true;
						}
					}
					else {
						position.x = -playerDifference + pos.x + 4.0f * sca.x;
						hitBuilding = true;
					}
				}
				if (position.x >= pos.x - 4.0f * sca.x && position.x <= playerDifference + pos.x - 4.0f * sca.x) {
					// 0 deg
					if (radians(0.0f) == radians(rot.y)) {
						if (position.z >= pos.z - playerDifference + 2.0f * sca.z || position.z <= pos.z + playerDifference - 2.0f * sca.z) {
							position.x = playerDifference + pos.x - 4.0f * sca.x;
							hitBuilding = true;
						}
					}
					else {
						position.x = playerDifference + pos.x - 4.0f * sca.x;
						hitBuilding = true;
					}
				}
			}
			// ceiling colliders
			if (position.x >= pos.x - 6.0f * sca.x && position.x <= pos.x + 6.0f * sca.x) {
				if (position.z >= pos.z - 6.0f * sca.z && position.z <= pos.z + 6.0f * sca.z) {
					if (position.y + 2.3f >= playerYDifference + pos.y + 6.0f * sca.y) {
						position.y = playerYDifference + -2.3f + pos.y + 6.0f * sca.y;
						hitBuilding = true;
					}
				}
			}
		}
		if (current.buildingType == 2) { // bench
			if (position.y - 2.16f <= pos.y + 2.25f * sca.y) {
				if (position.x >= pos.x - 0.5f * sca.x && position.x <= pos.x + 1.5f * sca.x) {
					// on top
					if (position.z >= pos.z - 2.0f * sca.z && position.z <= pos.z + 3.0f * sca.z) {
						if (position.y - 2.16f <= pos.y + 2.25f * sca.y) {
							position.y = 2.16f + pos.y + 2.25f * sca.y;
							if (jumpVelocity < 0) { jumpVelocity = 0.0f; }
							int jumpKey = stoi(inputLines[9]);
							float jumpHeight = 8.0f;
							if (checkKeyDown(jumpKey)) {
								jumpVelocity = jumpHeight;
							}
							onBenchCurrent = true;
							lastOnBench = true;
						}
					}
					// sides
					if (position.z >= pos.z + 3.0f * sca.z && position.z <= playerDifference + pos.z + 3.0f * sca.z) {
						position.z = playerDifference + pos.z + 3.0f * sca.z;
						hitBuilding = true;
					}
					if (position.z <= pos.z - 2.0f * sca.z && position.z >= -playerDifference + pos.z - 2.0f * sca.z) {
						position.z = -playerDifference + pos.z - 2.0f * sca.z;
						hitBuilding = true;
					}
				}
				if (position.z >= pos.z - 2.0f * sca.z && position.z <= pos.z + 3.0f * sca.z) {
					if (position.x >= pos.x + 1.5f * sca.x && position.x <= playerDifference + pos.x + 1.5f * sca.x) {
						position.x = playerDifference + pos.x + 1.5f * sca.x;
						hitBuilding = true;
					}
					if (position.x <= pos.x - 0.5f * sca.x && position.x >= -playerDifference + pos.x - 0.5f * sca.x) {
						position.x = -playerDifference + pos.x - 0.5f * sca.x;
						hitBuilding = true;
					}
				}
			}
			if (!onBenchCurrent && lastOnBench) {
				lastOnBench = false;
			}
		}
	}
}

void renderBuildings() {
	int count = allColourBuildings.size();
	for (int i = 0; i < count; i++) {
		glUseProgram(playerShader);

		setMat4(playerShader, "projection", projectionMatrix());
		setMat4(playerShader, "view", viewMatrix());
		setMat4(playerShader, "model", modelMatrix(allColourBuildings[i].position,
			allColourBuildings[i].rotation, allColourBuildings[i].scale));

		setShaderVecThree(playerShader, "lightPos", lightPos);
		setShaderFloat(playerShader, "lightIntensity", lightIntensity);
		setShaderFloat(playerShader, "lightRadius", lightRadius);
		setShaderInt(playerShader, "useLight", 1);
		setShaderFloat(playerShader, "lowestLight", lowestLight);

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
		model = rotate(model, radians(currentBuildingRotation.y), vec3(0.0f, 0.0f, 1.0f));
		model = scale(model, vec3(currentBuildingScale, 1.0f));
		setMat4(benchUIShader, "model", model);

		glBindVertexArray(currentBuildingVAO);
		glDrawArrays(GL_TRIANGLES, 0, currentBuildingTotal);

		int miniBuildingCount = allMiniBuildings.size();
		for (int m = 0; m < miniBuildingCount; m++) {
			model = ortho(-aspect_x / 10, aspect_x / 10, -aspect_y / 10, aspect_y / 10);
			model = translate(model, vec3(allMiniBuildings[m].position, 1.0f));
			model = rotate(model, radians(allMiniBuildings[m].rotation.y), vec3(0.0f, 0.0f, 1.0f));
			model = scale(model, vec3(allMiniBuildings[m].scale, 1.0f));
			setMat4(benchUIShader, "model", model);
			glDrawArrays(GL_TRIANGLES, 0, currentBuildingTotal);
		}
	}
}

void buildingInteractions(){
	buildBenchInteraction();
}
