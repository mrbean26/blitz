//what shows when program opens

int lastKey = -1;
void keyInput(GLFWwindow * window, int key,
		int scancode, int action, int mods) {
	lastKey = key;
}
void mouseInput(GLFWwindow * window, int button, 
		int action, int mods) {
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

class startScreen {
public:
	int titleImage, saveOneImage, saveTwoImage;
	int playOneButton, playTwoButton;

	int deleteOneButton, deleteTwoButton;
	float deleteOneTime, deleteTwoTime;


	int forwardButton, forwardText;
	int leftButton, leftText;
	int backButton, backText;
	int rightButton, rightText;
	int aimButton, aimText;
	int shootButton, shootText;
	int interactButton, interactText;
	int sprintButton, sprintText;
	int crouchButton, crouchText;
	int jumpButton, jumpText;

	int forwardKey;
	int leftKey;
	int backKey;
	int rightKey;
	int aimKey;
	int shootKey;
	int interactKey;
	int sprintKey;
	int crouchKey;
	int jumpKey;

	int playOneTexture=2, playTwoTexture=2; //0=create, 1=play

	vector<string> inputLines;

	bool checkKey = false;
	int textChange = -1;
	int location = -1;
	string asciiToString(int ascii){
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
		if (ascii == 257) {
			return "Enter";
		}
		if (ascii == 259) {
			return "Backspace";
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
	void changeInputs() {
		vector<int> keyTextsInts = { forwardKey, leftKey, backKey, rightKey, aimKey, 
			shootKey, interactKey, sprintKey, crouchKey, jumpKey };
		for (int i = forwardButton; i <= jumpButton; i++) {
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
	void begin() {
		//title
		titleImage = createButton();
		allButtons[titleImage].texture = loadTexture("assets/images/blitzTitle.png");
		allButtons[titleImage].scale = vec2(1.5f, 0.712f);
		allButtons[titleImage].position = vec3(0.0f, 2.5f, 0.0f);
		allButtons[titleImage].interactive = false;
		//save one text
		saveOneImage = createButton();
		allButtons[saveOneImage].texture = loadTexture("assets/images/saveOneTitle.png");
		allButtons[saveOneImage].scale = vec2(1.1f, 0.295f);
		allButtons[saveOneImage].position = vec3(-3.0f, 0.95f, 0.0f);
		allButtons[saveOneImage].interactive = false;
		//save two text
		saveTwoImage = createButton();
		allButtons[saveTwoImage].texture = loadTexture("assets/images/saveTwoTitle.png");
		allButtons[saveTwoImage].scale = vec2(1.1f, 0.295f);
		allButtons[saveTwoImage].position = vec3(-3.0f, -2.5f, 0.0f);
		allButtons[saveTwoImage].interactive = false;
		//play buttons
		playOneButton = createButton();
		playTwoButton = createButton();
		//delete buttons
		texture trashCan = loadTexture("assets/images/trashCan.png");
		deleteOneButton = createButton();
		allButtons[deleteOneButton].texture = trashCan;
		allButtons[deleteOneButton].scale = vec2(0.165f, 0.27f);
		allButtons[deleteOneButton].position = vec3(2.0f, 0.95f, 0.0f);
		deleteTwoButton = createButton();
		allButtons[deleteTwoButton].texture = trashCan;
		allButtons[deleteTwoButton].scale = vec2(0.165f, 0.278f);
		allButtons[deleteTwoButton].position = vec3(2.0f, -2.95f, 0.0f);
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
		for (int i = forwardButton; i <= jumpButton; i++) {
			allButtons[i] = defaultInputButton;
			float xPosition;
			float yPosition;
			if (i < shootButton) {
				xPosition = 5.5f;
				yPosition = -13.5f + (((float)i - forwardButton)*3.5f);
			}
			if (i > aimButton) {
				xPosition = 9.5f;
				yPosition = -13.5f + (((float)i - shootButton)*3.5f);
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
		vector<string> displayedTexts = { "Forward", "Left",
			"Backward", "Right", "Aim", "Shoot", "Interact", "Sprint",
				"Crouch", "Jump" };
		for (int i = forwardText; i <= jumpText; i++) {
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
		inputLines = readLines("assets/saves/inputs.save");
		for (int i = forwardKey; i <= jumpKey; i++) {
			allTexts[i].fontPath = "assets/fonts/zekton.ttf";
			allTexts[i].colour = vec3(0.0f, 0.0f, 1.0f);
			allTexts[i].fontSize = display_x / 55;
			allTexts[i].displayedText = asciiToString(stoi(inputLines[i-forwardKey]));
		}
	}
	void mainloop() {
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
		if (saveLinesOne[0] == "IN USE" && playOneTexture != 1) {
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
		if (saveLinesTwo[0] == "IN USE" && playTwoTexture != 1) {
			allButtons[playTwoButton].texture =
				loadTexture("assets/images/playTitle.png");
			playTwoTexture = 1;
			allButtons[playTwoButton].scale = playScale;
			allButtons[playTwoButton].position = vec3(-1.4f, -3.0f, 0.0f);
			allButtons[deleteTwoButton].active = true;
		}
		//INPUT MANAGER
		vector<int> buttonInts = { forwardButton, leftButton, backButton, rightButton,
			aimButton, shootButton, interactButton, sprintButton, crouchButton, jumpButton };
		for (int i = forwardText; i <= jumpText; i++) {
			int index = buttonInts[i - forwardText];
			float positionX = allButtons[index].minX * 1.005f;
			float positionY = ((float)display_y - allButtons[index].minY) + (display_y*0.01f);
			allTexts[i].colour = vec3(0.18f, 0.82f, 0.86f);
			allTexts[i].fontSize = display_x / 45;
			allTexts[i].position = vec2(positionX, positionY);
		}
		for (int i = forwardKey; i <= jumpKey; i++) {
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
		}
		if (allButtons[playTwoButton].clickUp) {
			if (playTwoTexture == 0) {
				createSave("assets/saves/saveTwo.save", DEFAULT_SAVE);
			}
		}
	}
};