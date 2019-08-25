using namespace std;

#include <vector>

#include <glew.h>
#include <glfw3.h>

#include "startScreen.h"
#include "interface.h"
#include "worldGeneration.h"
#include "frontend.h"
#include "debug.h"

int lastKey = -1;
void keyInput(GLFWwindow * window, int key, 
		int scancode, int action, int mods){
	lastKey = key;
}

void mouseInput(GLFWwindow * window, int button,
	int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			lastKey = 256256;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			lastKey = 128128;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (action == GLFW_PRESS) {
			lastKey = 512512;
		}
	}
}

string startScreen::asciiToString(int ascii){
	if (ascii >= 'a' && ascii <= 'z') {
		char returned = char(ascii);
		return to_string(returned);
	}
	if (ascii >= 'A' && ascii <= 'Z') {
		char returned = char(ascii);
		string returnedString = string(1, returned);
		return returnedString;
	}
	if (ascii == 32) {
		return "Space";
	}
	if (ascii == 280) {
		return "Caps";
	}
	if (ascii == 340) {
		return "LShift";
	}
	if (ascii == 341) {
		return "LCtrl";
	}
	if (ascii == 258) {
		return "Tab";
	}
	if (ascii == 290) {
		return "F1";
	}
	if (ascii == 291) {
		return "F2";
	}
	if (ascii == 292) {
		return "F3";
	}
	if (ascii == 293) {
		return "F4";
	}
	if (ascii == 294) {
		return "F5";
	}
	if (ascii == 295) {
		return "F6";
	}
	if (ascii == 296) {
		return "F7";
	}
	if (ascii == 297) {
		return "F8";
	}
	if (ascii == 298) {
		return "F8";
	}
	if (ascii == 299) {
		return "F9";
	}
	if (ascii == 300) {
		return "F10";
	}
	if (ascii == 301) {
		return "F11";
	}
	if (ascii == 302) {
		return "F12";
	}
	if (ascii == 342) {
		return "LAlt";
	}
	if (ascii == 346) {
		return "RAlt";
	}
	if (ascii == 344) {
		return "RShift";
	}
	if (ascii == 345) {
		return "RCtrl";
	}
	if(ascii == 256){
		return "Escape";
	}
	if (ascii == 257) {
		return "Enter";
	}
	if (ascii == 259) {
		return "Back";
	}
	if (ascii == 261) {
		return "Del";
	}
	if (ascii == 265) {
		return "Up";
	}
	if (ascii == 263) {
		return "Left";
	}
	if (ascii == 262) {
		return "Right";
	}
	if (ascii == 264) {
		return "Down";
	}
	if (ascii == 343) {
		return "Windows";
	}
	if (ascii == 128128) {
		return "Mouse1";
	}
	if (ascii == 256256) {
		return "Mouse2";
	}
	if (ascii == 512512) {
		return "MWheel";
	}
	if (ascii == 162) {
		return "\\";
	}
	char unread = char(ascii);
	string unreadString = string(1, unread);
	return unreadString;
}

void startScreen::changeInputs(){
	vector<int> keyTextsInts = { forwardKey, leftKey, backKey, rightKey, aimKey,
			shootKey, interactKey, sprintKey, crouchKey, jumpKey, pauseKey, weaponKey };
	for (int i = forwardButton; i <= weaponButton; i++) {
		//check for key
		if (allButtons[i].clickUp && !checkKey) {
			checkKey = true;
			lastKey = -1;
			location = i - forwardButton;
			textChange = keyTextsInts[i - forwardButton];
		}
	}
	if (checkKey) {
		glfwSetKeyCallback(window, keyInput);
		glfwSetMouseButtonCallback(window, mouseInput);
		if (lastKey != -1) {
			allTexts[textChange].displayedText = asciiToString(lastKey);
			//check for duplicate
			vector<string> previousLines = readLines("assets/saves/inputs.save");
			int vectorSize = inputLines.size();
			for (int i = 0; i < vectorSize; i++) {
				if (i == location) {
					continue;
				}
				if (to_string(lastKey) == previousLines[i]) {
					allTexts[textChange].displayedText = asciiToString(stoi(previousLines[location]));
					//error
					goto reset;
				}
			}
			//if it runs here, change went well
			//write to file here
			previousLines[location] = to_string(lastKey);
			writeLines("assets/saves/inputs.save", previousLines);
			//reset
			reset:
			checkKey = false;
			textChange = -1;
			lastKey = -1;
			location = -1;
		}
	}
}

bool loading = false;
const char * linesPathHold;
vector<text> previousAllTexts;
vector<button> previousAllButtons;

void loadWorld(const char * linesPath) {
	loading = true;
	allButtons[StartScreen.playOneButton].clickUp = false;
	allButtons[StartScreen.playTwoButton].clickUp = false;
	previousAllButtons = allButtons;
	previousAllTexts = allTexts;
	allButtons.clear();
	int textCount = allTexts.size();
	allTexts[StartScreen.loadingText].active = true;
	for (int i = 0; i < textCount; i++) {
		if (i != StartScreen.loadingText) {
			if(i != debugText){
				allTexts[i].active = false;
			}
		}
	}
	linesPathHold = linesPath;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void startWorld(const char * linesPath) {
	StartScreen.active = false;
	WorldGeneration.active = true;
	WorldGeneration.worldLinesPath = linesPath;
	text debugTextHolder = allTexts[debugText];
	allTexts.clear();
	allTexts = {debugTextHolder};
	debugText = 0;
	mainPlayer.active = true;
}

void startScreen::begin(){
	//title
	titleImage = createButton(vec2(1.5f, 0.712f), vec3(0.0f, 2.5f, 0.0f), false);
	allButtons[titleImage].texture = loadTexture("assets/images/blitzTitle.png");
	//save one text
	saveOneImage = createButton(vec2(1.1f, 0.295f), vec3(-3.0f, 0.95f, 0.0f), false);
	allButtons[saveOneImage].texture = loadTexture("assets/images/saveOneTitle.png");
	//save two text
	saveTwoImage = createButton(vec2(1.1f, 0.295f), vec3(-3.0f, -2.5f, 0.0f), false);
	allButtons[saveTwoImage].texture = loadTexture("assets/images/saveTwoTitle.png");
	//play buttons
	playOneButton = createButton();
	playTwoButton = createButton();
	//delete buttons
	texture trashCan = loadTexture("assets/images/trashCan.png");
	deleteOneButton = createButton(vec2(0.165f, 0.27f), vec3(2.0f, 0.95f, 0.0f));
	allButtons[deleteOneButton].texture = trashCan;
	deleteTwoButton = createButton(vec2(0.165f, 0.27f), vec3(2.0f, -2.95f, 0.0f));
	allButtons[deleteTwoButton].texture = trashCan;
	//INPUT MANAGER
	texture inputButtonBackground = loadTexture("assets/images/inputButtonBackground.png");
	vec2 inputButtonScale = vec2(0.4f, 0.175f);
	button defaultInputButton;
	defaultInputButton.scale = inputButtonScale;
	defaultInputButton.texture = inputButtonBackground;
	//forward
	forwardButton = createButton();
	leftButton = createButton();
	backButton = createButton();
	rightButton = createButton();
	aimButton = createButton();
	shootButton = createButton();
	interactButton = createButton();
	sprintButton = createButton();
	crouchButton = createButton();
	jumpButton = createButton();
	pauseButton = createButton();
	weaponButton = createButton();
	for (int i = forwardButton; i <= weaponButton; i++) {
		allButtons[i] = defaultInputButton;
		float xPosition = 0.0f;
		float yPosition = 0.0f;
		if (i < interactButton) {
			xPosition = 5.5f;
			yPosition = -13.5f + (((float)i - forwardButton)*3.5f);
		}
		if (i > shootButton) {
			xPosition = 9.5f;
			yPosition = -13.5f + (((float)i - interactButton)*3.5f);
		}
		allButtons[i].position = vec3(xPosition, yPosition, 0.0f);
	}
	//title texts
	forwardText = createText();
	leftText = createText();
	backText = createText();
	rightText = createText();
	aimText = createText();
	shootText = createText();
	interactText = createText();
	sprintText = createText();
	crouchText = createText();
	jumpText = createText();
	pauseText = createText();
	weaponText = createText();
	vector<string> displayedTexts = { "Forward", "Left",
		"Backward", "Right", "Aim", "Shoot", "Interact", "Sprint",
			"Crouch", "Jump", "Pause", "Weapon" };
	for (int i = forwardText; i <= weaponText; i++) {
		allTexts[i].fontPath = "assets/fonts/zekton.ttf";
		allTexts[i].displayedText = displayedTexts[i - forwardText];
		allTexts[i].fontSize = display_x / 50;
	}
	//key input texts (ontop of buttons)
	forwardKey = createText();
	leftKey = createText();
	backKey = createText();
	rightKey = createText();
	aimKey = createText();
	shootKey = createText();
	interactKey = createText();
	sprintKey = createText();
	crouchKey = createText();
	jumpKey = createText();
	pauseKey = createText();
	weaponKey = createText();
	inputLines = readLines("assets/saves/inputs.save");
	for (int i = forwardKey; i <= weaponKey; i++) {
		allTexts[i].fontPath = "assets/fonts/zekton.ttf";
		allTexts[i].colour = vec3(0.0f, 0.0f, 1.0f);
		allTexts[i].fontSize = display_x / 55;
		allTexts[i].displayedText = asciiToString(stoi(inputLines[i - forwardKey]));
	}
	// world size text
	int worldSizeText = createText();
	allTexts[worldSizeText].fontPath = "assets/fonts/zekton.ttf";
	allTexts[worldSizeText].fontSize = display_x / 30;
	allTexts[worldSizeText].position = vec2(display_x / 4, display_y / 5);
	allTexts[worldSizeText].displayedText = "World 1 : Small Worlds";

	int worldSizeTextLarge = createText();
	allTexts[worldSizeTextLarge].fontPath = "assets/fonts/zekton.ttf";
	allTexts[worldSizeTextLarge].fontSize = display_x / 30;
	allTexts[worldSizeTextLarge].position = vec2(display_x / 4, display_y / 10);
	allTexts[worldSizeTextLarge].displayedText = "World 2 : Large Worlds";
	// loading text
	loadingText = createText();
	allTexts[loadingText].fontPath = "assets/fonts/zekton.ttf";
	allTexts[loadingText].fontSize = (int)((float)display_x / 30.0f);
	allTexts[loadingText].displayedText = "Loading...";
	allTexts[loadingText].position = vec2(display_x / 2.0f, display_y / 2.0f);
	allTexts[loadingText].centered = true;
	allTexts[loadingText].active = false;
}

void startScreen::mainloop(){
	if (!active) {
		return;
	}
	if (loading) {
		startWorld(linesPathHold);
		createButton(); // new empty button
		loading = false;
		return;
	}
	// button data
	vector<string> saveLinesOne = readLines("assets/saves/saveOne.save");
	vec2 createScale = vec2(0.8375f, 0.215f);
	vec2 playScale = vec2(0.63f, 0.27f);
	//check if savefiles are used - if true, change texture to "play" and enable delete button
	if (saveLinesOne[0] == "NOT USED" && playOneTexture != 0) {
		allButtons[playOneButton].texture =
			loadTexture("assets/images/createTitle.png");
		playOneTexture = 0;
		allButtons[playOneButton].scale = createScale;
		allButtons[playOneButton].position = vec3(-1.0f, 1.3f, 0.0f);
		allButtons[deleteOneButton].active = false;
	}
	if (contains(saveLinesOne[0], "IN USE") && playOneTexture != 1) {
		allButtons[playOneButton].texture =
			loadTexture("assets/images/playTitle.png");
		playOneTexture = 1;
		allButtons[playOneButton].scale = playScale;
		allButtons[playOneButton].position = vec3(-1.4f, 1.0f, 0.0f);
		allButtons[deleteOneButton].active = true;
	}
	vector<string> saveLinesTwo = readLines("assets/saves/saveTwo.save");
	if (saveLinesTwo[0] == "NOT USED" && playTwoTexture != 0) {
		allButtons[playTwoButton].texture =
			loadTexture("assets/images/createTitle.png");
		playTwoTexture = 0;
		allButtons[playTwoButton].scale = createScale;
		allButtons[playTwoButton].position = vec3(-1.0f, -3.3f, 0.0f);
		allButtons[deleteTwoButton].active = false;
	}
	if (contains(saveLinesTwo[0], "IN USE") && playTwoTexture != 1) {
		allButtons[playTwoButton].texture =
			loadTexture("assets/images/playTitle.png");
		playTwoTexture = 1;
		allButtons[playTwoButton].scale = playScale;
		allButtons[playTwoButton].position = vec3(-1.4f, -3.0f, 0.0f);
		allButtons[deleteTwoButton].active = true;
	}
	//INPUT MANAGER
	vector<int> buttonInts = { forwardButton, leftButton, backButton, rightButton,
		aimButton, shootButton, interactButton, sprintButton, crouchButton, jumpButton, pauseButton, weaponButton };
	for (int i = forwardText; i <= weaponText; i++) {
		int index = buttonInts[i - forwardText];
		float positionX = allButtons[index].minX * 1.005f;
		float positionY = ((float)display_y - allButtons[index].minY) + (display_y*0.01f);
		allTexts[i].colour = vec3(0.18f, 0.82f, 0.86f);
		allTexts[i].fontSize = display_x / 45;
		allTexts[i].position = vec2(positionX, positionY);
	}
	for (int i = forwardKey; i <= weaponKey; i++) {
		int index = buttonInts[i - forwardKey];
		float positionX = allButtons[index].minX * 1.005f;
		float positionY = ((float)display_y - allButtons[index].maxY) + (display_y*0.015f);
		allTexts[i].position = vec2(positionX, positionY);
	}
	changeInputs();
	//world delete
	vector<string> deletedWorldSave = { "NOT USED" };
	if (allButtons[deleteOneButton].clickUp) {
		bool previous = false;
		if (allButtons[deleteOneButton].colour != vec3(1.0f, 1.0f, 0.0f)) {
			allButtons[deleteOneButton].colour = vec3(1.0f, 1.0f, 0.0f);
			previous = true;
		}
		if (allButtons[deleteOneButton].colour.z == 0.0f && !previous) {
			//delete here
			writeLines("assets/saves/saveOne.save", deletedWorldSave);
		}
	}
	if (deleteOneTime >= 1.5f) {
		allButtons[deleteOneButton].colour = vec3(1.0f, 1.0f, 1.0f);
		deleteOneTime = 0.0f;
	}
	if (allButtons[deleteOneButton].colour == vec3(1.0f, 1.0f, 0.0f)) {
		deleteOneTime += (float)deltaTime;
	}
	if (allButtons[deleteTwoButton].clickUp) {
		bool previous = false;
		if (allButtons[deleteTwoButton].colour != vec3(1.0f, 1.0f, 0.0f)) {
			allButtons[deleteTwoButton].colour = vec3(1.0f, 1.0f, 0.0f);
			previous = true;
		}
		if (allButtons[deleteTwoButton].colour.z == 0.0f && !previous) {
			//delete here
			writeLines("assets/saves/saveTwo.save", deletedWorldSave);
		}
	}
	if (deleteTwoTime >= 1.5f) {
		allButtons[deleteTwoButton].colour = vec3(1.0f, 1.0f, 1.0f);
		deleteTwoTime = 0.0f;
	}
	if (allButtons[deleteTwoButton].colour == vec3(1.0f, 1.0f, 0.0f)) {
		deleteTwoTime += (float)deltaTime;
	}
	if (allButtons[playOneButton].clickUp) {
		if (playOneTexture == 0) {
			createSave("assets/saves/saveOne.save", DEFAULT_SAVE);
		}
		if (playOneTexture == 1) {
			loadWorld("assets/saves/saveOne.save");
			return;
		}
	}
	if (allButtons[playTwoButton].clickUp) {
		if (playTwoTexture == 0) {
			createSave("assets/saves/saveTwo.save", LARGE_WORLD);
		}
		if (playTwoTexture == 1) {
			loadWorld("assets/saves/saveTwo.save");
			return;
		}
	}
}
