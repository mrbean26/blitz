using namespace glm;
using namespace std;

class button {
public:
	vec3 position = vec3(0.0f, 0.0f, 0.0f);
	vec2 scale = vec2(1.0f, 1.0f);
	float rotation = 0.0f;

	bool clicked = false;
	bool interactive = true;
	bool mouseOver = false;

	texture texture;

	unsigned int vertexArray;
	unsigned int vertexBuffer, elementBuffer;

	vec3 colour = vec3(1.0f, 1.0f, 1.0f);
	float alpha = 1.0f;
};

std::vector<button> allButtons;

int createButton() {
	button newButton;
	int buttonCount = allButtons.size();
	allButtons.resize(buttonCount + 1);
	allButtons[buttonCount] = newButton;
	return buttonCount;
}

unsigned int buttonTextureShader;

void renderButtons() {
	int buttonCount = allButtons.size();
	for (int i = 0; i < buttonCount; i++) {
		button currentButton = allButtons[i];
		//make button bigger if mouse is over it
		glMatrixMode(GL_MODELVIEW);
		if (currentButton.mouseOver) {
			
		}
		float modelviewMatrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrix);
		int matrixLocation = glGetUniformLocation(buttonTextureShader, "modelviewMatrix");
		glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, modelviewMatrix);
		//texture color
		int colourLocation = glGetUniformLocation(buttonTextureShader, "textureColour");
		vec3 color = currentButton.colour;
		glUniform4f(colourLocation, color.x, color.y, color.z, currentButton.alpha);
		//draw
		enableTexture(currentButton.texture); 
		glUseProgram(buttonTextureShader);
		glBindVertexArray(currentButton.vertexArray);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //if an error is being shown here for memory, shapes are being created before backendBegin()
		glBindTexture(GL_TEXTURE_2D, 0);
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
	//reset
	int buttonCount = allButtons.size();
	for (int i = 0; i < buttonCount; i++) {
		allButtons[i].clicked = false;
	}
	//mouse pos
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	for (int i = 0; i < buttonCount; i++) {
		//button
		button currentButton = allButtons[i];
		vec3 buttonPosition = currentButton.position;
		vec2 buttonScale = currentButton.scale;
		buttonScale.y *= 2;
		//vertex coords
		float maxX=0, maxY=0;
		float minX=0, minY=0;
	
		float middleScreen[2] = { (float)display_x / 2, (float)display_y / 2 };
		float scaleJump[2] = { (float)display_x / 10, (float)display_y / (float)aspect_y };
		minX = middleScreen[0] - (scaleJump[0] * buttonScale.x);
		maxX = middleScreen[0] + (scaleJump[0] * buttonScale.x);
		minY = middleScreen[1] - (scaleJump[1] * (buttonScale.y / 2));
		maxY = middleScreen[1] + (scaleJump[1] * (buttonScale.y / 2));

		float positionDivided[2] = { (float)display_x / 10, (float)display_y / (float)aspect_y };
		minX += positionDivided[0] * buttonPosition.x;
		maxX += positionDivided[0] * buttonPosition.x;

		double yDivided = (double)display_y / aspect_y;
		minY -= (float)yDivided * buttonPosition.y;
		maxY -= (float)yDivided * buttonPosition.y;

		//check for click
		allButtons[i].mouseOver = false;
		if (mouseX >= minX && mouseX <= maxX) {
			if (mouseY >= minY && mouseY <= maxY) {
				if (clickDown()) {
					allButtons[i].clicked = true;
				}
				if (currentButton.interactive) {
					allButtons[i].mouseOver = true;
				}
			}
		}
	}
	if (clickDown()) {
		lastClick = true;
	}
}

void buttonsBegin() {
	int buttonCount = allButtons.size();
	vec2 twoMultiplier = vec2(2.0f, 2.0f);
	vec2 buttonRescale = vec2(1.0f / (float)aspect_x, 1.0f / (float)aspect_y);
	//position
	for (int i = 0; i < buttonCount; i++) {
		button currentButton = allButtons[i];
		vec3 position = currentButton.position / vec3((float)aspect_x, 
			(float)aspect_y, 1.0f);
		position = position * vec3(twoMultiplier, 1.0f);
		vec2 scale = currentButton.scale * buttonRescale * twoMultiplier;
		float vertices[] = {
			// positions then colors then texture coords
			 position.x + scale.x, position.y + scale.y, position.z, 
			 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 
			 position.x + scale.x, position.y - scale.y, position.z, 
			 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			 position.x - scale.x, position.y - scale.y, position.z, 
			 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			 position.x - scale.x, position.y + scale.y, position.z, 
			 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
		};
		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};
		glGenVertexArrays(1, &allButtons[i].vertexArray);
		glGenBuffers(1, &allButtons[i].vertexBuffer);
		glGenBuffers(1, &allButtons[i].elementBuffer);
		glBindVertexArray(allButtons[i].vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, allButtons[i].vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, allButtons[i].elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	//create shader
	int vertexShader, fragmentShader;
	vertexShader = createShader(textureVertSource, GL_VERTEX_SHADER);
	fragmentShader = createShader(textureFragSource, GL_FRAGMENT_SHADER);
	buttonTextureShader = createProgram({ vertexShader, fragmentShader });
}

void interfaceBegin() {
	buttonsBegin();
}

void interfaceMainloop() {
	renderButtons();
	registerClicks();
}