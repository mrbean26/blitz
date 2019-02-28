class button {
public:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec2 scale = glm::vec2(1.0f, 1.0f);
	float rotation = 0.0f;

	bool clicked = false;
};

std::vector<button> allButtons;

int createButton() {
	button newButton;
	int buttonCount = allButtons.size();
	allButtons.resize(buttonCount + 1);
	allButtons[buttonCount] = newButton;
	return buttonCount;
}

void renderButtons() {
	int buttonCount = allButtons.size();
	for (int i = 0; i < buttonCount; i++) {
		button currentButton = allButtons[i];
		glm::vec3 position = currentButton.position;
		float rotation = currentButton.rotation;
		glm::vec2 scale = currentButton.scale;
		//setup matrix
		glLoadIdentity();
		glOrtho(0.0, aspect_x, 0.0, aspect_y, -1.0, 1.0);
		glTranslatef(position.x, position.y, position.z);
		glRotated(rotation, 0.0, 0.0, 1.0);
		glScalef(scale.x, scale.y, 1.0f);
		//draw quad
		glBegin(GL_QUADS);
		glVertex2f(-1.0f, -1.0f);
		glVertex2f(-1.0f, 1.0f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(1.0f, -1.0f);
		glEnd();
	}
}

bool clickedDown = false;
bool lastClick = false;
bool clickDown() {
	if(clickedDown && !lastClick){
		return true;
	}
	return false;
}

void registerClicks() {
	int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (mouseState == GLFW_PRESS) {
		clickedDown = true;
	}
	else {
		clickedDown = false;
		lastClick = false;
	}
	//check for button input
	if (!clickDown()) {
		return;
	}
	//mouse pos
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	int buttonCount = allButtons.size();
	for (int i = 0; i < buttonCount; i++) {
		//button
		button currentButton = allButtons[i];
		glm::vec3 buttonPosition = currentButton.position;
		glm::vec2 buttonScale = currentButton.scale;
		//vertex coords
		int maxX, maxY;
		int minX, minY;
		int aspectMultiplier = aspectDivider(display_x, display_y);
		maxX = (aspectMultiplier * buttonPosition.x) + (aspectMultiplier * buttonScale.x);
		maxY = (aspectMultiplier * buttonPosition.y) + (aspectMultiplier * buttonScale.y);
		minX = (aspectMultiplier * buttonPosition.x) - (aspectMultiplier * buttonScale.x);
		minY = (aspectMultiplier * buttonPosition.y) - (aspectMultiplier * buttonScale.y);
		mouseY = display_y - mouseY;
		//check for click
		if (mouseX >= minX && mouseX <= maxX) {
			if (mouseY >= minY && mouseY <= maxY) {
				allButtons[i].clicked = true;
			}
		}
	}
	lastClick = true;
}

void interfaceMainloop() {
	renderButtons();
	registerClicks();
}