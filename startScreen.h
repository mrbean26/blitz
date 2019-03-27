#pragma once
//what shows when program opens

#ifndef START_SCREEN
#define START_SCREEN

extern int lastKey;
void keyInput(GLFWwindow * window, int key,
	int scancode, int action, int mods); // detect if a key is clicked

void mouseInput(GLFWwindow * window, int button,
	int action, int mods); // detect if m1, m2 or mwheel is clicked

class startScreen {
public:
	bool active = true;

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
	string asciiToString(int ascii); // if the character is not being recognised then this is being called

	void changeInputs(); // if the button has been clicked then this is being called

	void begin(); // called before first frame

	void mainloop(); // called every frame
};

#endif