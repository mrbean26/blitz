#include <cstdlib>

#define DEFAULT_SAVE 0

int randomInt(int max) {
	int random = rand();
	return random % max;
}

void createSave(const char * filePath, int saveType) {
	vector<string> saveLines;
	int lineCount = 0;
	if (saveType == DEFAULT_SAVE) {
		//in use
		lineCount++;
		saveLines.resize(lineCount);
		saveLines[lineCount - 1] = "IN USE";
		//hotbar
		lineCount++;
		saveLines.resize(lineCount);
		saveLines[lineCount - 1] = "hotbar 0 0 0 0 0"; //item id, 5 slots in hotbar
		//inventory
		lineCount++;
		saveLines.resize(lineCount);
		saveLines[lineCount - 1] = "inventory 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"; //item id, 15 slots in inventory
		//current area
		lineCount++;
		saveLines.resize(lineCount);
		saveLines[lineCount - 1] = "currentArea homeBase"; //science base on earth
		//area size - homebase 
		int homeBaseSizeX = randomInt(40) + 10;
		int homeBaseSizeY = randomInt(40) + 10;
		lineCount++;
		saveLines.resize(lineCount);
		saveLines[lineCount - 1] = "areaSizeHomeBase " + to_string(homeBaseSizeX) + " " + to_string(homeBaseSizeY);
		//area colour - homebase
		lineCount++;
		saveLines.resize(lineCount);
		saveLines[lineCount - 1] = "areaColourHomeBase 0.36 0.94 0.34";
		//area mountains - homebase
		int mountainCountHomeBase = randomInt(6)+1;
		string homeBaseMountains;
		for (int i = 0; i < mountainCountHomeBase; i++) {

		}
		lineCount++;
		saveLines.resize(lineCount);
		saveLines[lineCount - 1] = homeBaseMountains;
	}
	writeLines(filePath, saveLines);
}