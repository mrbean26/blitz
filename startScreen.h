//what shows when program opens

class startScreen {
public:
	int titleImage, saveOneImage, saveTwoImage;
	int playOneButton, playTwoButton;
	int deleteOneButton, deleteTwoButton;

	int playOneTexture=2, playTwoTexture=2; //0=create, 1=play
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
		deleteOneButton = createButton();
		allButtons[deleteOneButton].texture = loadTexture("assets/images/trashCan.png");
		allButtons[deleteOneButton].scale = vec2(0.165f, 0.27f);
		allButtons[deleteOneButton].position = vec3(2.0f, 0.95f, 0.0f);
		deleteTwoButton = createButton();
		allButtons[deleteTwoButton].texture = loadTexture("assets/images/trashCan.png");
		allButtons[deleteTwoButton].scale = vec2(0.165f, 0.278f);
		allButtons[deleteTwoButton].position = vec3(2.0f, -2.95f, 0.0f);
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
	}
};