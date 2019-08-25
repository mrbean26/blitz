#include "interface.h"

#include <vector>
using namespace std;

unsigned int buttonTextureShader;
bool clickedDown = false;
bool lastClick = false;

GLuint buttonVAO, buttonVBO, buttonEBO;

std::vector<button> allButtons;

int createButton(vec2 size, vec3 position, bool interactive){
	button newButton;
	allButtons[newVectorPos(&allButtons)] = newButton;
	int index = allButtons.size() - 1;
	allButtons[index].scale = size;
	allButtons[index].position = position;
	allButtons[index].interactive = interactive;
	return index;
}

void renderButtons(){
	glDisable(GL_DEPTH_TEST);
	int buttonCount = allButtons.size();
	vec2 rescale = vec2(2.0f, 2.0f);
	vec2 aspectRatio = vec2(aspect_x, aspect_y);
	glBindVertexArray(buttonVAO);
	for (int i = 0; i < buttonCount; i++) {
		button currentButton = allButtons[i];
		if (!currentButton.active) {
			continue;
		}
		vec2 scale = currentButton.scale * rescale;
		vec3 position = currentButton.position;
		scale = scale / aspectRatio;
		//make button bigger if mouse is over it
		mat4 scaleMat = mat4(1.0f);
		if (currentButton.mouseOver && currentButton.interactive) {
			position *= vec3(0.95f, 0.95f, 1.0f);
			scale *= vec2(1.05f, 1.05f);
		}
		if (currentButton.clickDown && currentButton.interactive) {
			scale *= vec2(0.95f, 0.95f);
			position *= vec3(1.05f, 1.05f, 1.0f);
		}
		//rescale the matrix and send position info to shader
		scaleMat = glm::scale(scaleMat, vec3(scale, 1.0f));
		scaleMat = translate(scaleMat, position);
		scaleMat = glm::rotate(scaleMat, radians(currentButton.rotation), vec3(0.0f, 0.0f, 1.0f));
		//update position, scale and rotation info ready for the shader to use
		setMat4(buttonTextureShader, "modelviewMatrix", scaleMat);
		//texture color
		int colourLocation = glGetUniformLocation(buttonTextureShader, "textureColour");
		vec3 color = currentButton.colour;
		glUniform4f(colourLocation, color.x, color.y, color.z, currentButton.alpha);
		//draw
		glActiveTexture(GL_TEXTURE0 + i);
		enableTexture(currentButton.texture);

		//set texture for shader
		setShaderInt(buttonTextureShader, "texture0", i);
		glUseProgram(buttonTextureShader);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //if an error is being shown here for memory, shapes are being created before backendBegin()
	}
	glEnable(GL_DEPTH_TEST);
}

void registerClicks(){
	int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (mouseState == GLFW_PRESS) {
		clickedDown = true;
	}
	else {
		clickedDown = false;
	}
	//reset
	int buttonCount = allButtons.size();
	for (int i = 0; i < buttonCount; i++) {
		allButtons[i].clickUp = false;
	}
	//mouse pos
	for (int i = 0; i < buttonCount; i++) {
		if (!allButtons[i].active) {
			continue;
		}
		allButtons[i].clickDown = false;
		//button
		button currentButton = allButtons[i];
		vec3 buttonPosition = currentButton.position;
		vec2 buttonScale = currentButton.scale;
		//vertex coords
		int minX = 0, maxX = 0;
		int minY = 0, maxY = 0;
		//variables required to calculate minimum and maximum mouse positions for buttons to interact
		float xDivided = (float)display_x / 10.0f;
		float yDivided = (float)display_y / (float)aspect_y;
		vec2 midScreen = vec2(display_x / 2, display_y / 2);
		vec2 positionMultiplied = vec2(buttonPosition.x*buttonScale.x,
			buttonPosition.y*buttonScale.y);
		positionMultiplied = vec2(positionMultiplied.x*xDivided, positionMultiplied.y*yDivided);
		vec2 midButton = vec2(midScreen.x + positionMultiplied.x, midScreen.y + positionMultiplied.y);
		//x axis
		float rescaleX = xDivided * buttonScale.x;
		minX = (int)(midButton.x - rescaleX);
		maxX = (int)(midButton.x + rescaleX);
		//y axis - get axis from scale then pos
		float midY = midScreen.y - (yDivided * (buttonPosition.y * buttonScale.y));
		minY = (int)(midY - ((yDivided * buttonScale.y)));
		maxY = (int)(midY + (yDivided * buttonScale.y));
		//add to class
		allButtons[i].minX = minX;
		allButtons[i].maxX = maxX;
		allButtons[i].minY = minY;
		allButtons[i].maxY = maxY;
		//check for click
		allButtons[i].mouseOver = false;
		if (mousePosX >= minX && mousePosX <= maxX) {
			if (mousePosY >= minY && mousePosY <= maxY) {
				if (clickedDown) {
					allButtons[i].clickDown = true;
				}
				if (lastClick && !clickedDown) {
					allButtons[i].clickUp = true;
				}
				allButtons[i].mouseOver = true;
			}
		}
	}
	lastClick = clickedDown;
}

void buttonsBegin(){
	//vertex data
	float vertices[] = {
		// positions then colors then texture coords, they are 0.99f to stop weird lines on edge of textures
		0.99f, 0.99f, 0.0f,
		 0.99f, 0.0f, 0.0f, 0.99f, 0.99f,
		 0.99f, -0.99f, 0.0f,
		 0.0f, 0.99f, 0.0f, 0.99f, 0.0f,
		 -0.99f, -0.99f, 0.0f,
		 0.0f, 0.0f, 0.99f, 0.0f, 0.0f,
		 -0.99f, 0.99f, 0.0f,
		 0.99f, 0.99f, 0.0f, 0.0f, 0.99f
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	glGenVertexArrays(1, &buttonVAO);
	glGenBuffers(1, &buttonVBO);
	glGenBuffers(1, &buttonEBO);
	glBindVertexArray(buttonVAO);
	glBindBuffer(GL_ARRAY_BUFFER, buttonVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buttonEBO);
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
	//create shader
	int vertexShader, fragmentShader;
	vertexShader = createShader("assets/shaders/textureVert.txt", GL_VERTEX_SHADER);
	fragmentShader = createShader("assets/shaders/textureFrag.txt", GL_FRAGMENT_SHADER);
	buttonTextureShader = createProgram({ vertexShader, fragmentShader });
}

//texts
vector<text> allTexts;
unsigned int textShader;
GLuint textVAO, textVBO;

int createText(){
	text newText;
	allTexts[newVectorPos(&allTexts)] = newText;
	return allTexts.size() - 1;
}

map<GLchar, Character> getFont(const char * path, int fontsize){
	map<GLchar, Character> returned;

	FT_Library ftLibrary;
	if (FT_Init_FreeType(&ftLibrary)) {
		cout << "Couldn't Init Freetype" << endl;
		return returned;
	}
	FT_Face newFont;
	if (FT_New_Face(ftLibrary, path, 0, &newFont)) {
		cout << "Couldn't load font: " << path << endl;
		return returned;
	}
	FT_Set_Pixel_Sizes(newFont, 0, fontsize);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c = 0; c < 128; c++) { //load first 128 of ASCII
		if (FT_Load_Char(newFont, c, FT_LOAD_RENDER)) {
			cout << "Couldn't Load Character" << c << endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RED,
			newFont->glyph->bitmap.width,
			newFont->glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE,
			newFont->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(newFont->glyph->bitmap.width, newFont->glyph->bitmap.rows),
			glm::ivec2(newFont->glyph->bitmap_left, newFont->glyph->bitmap_top),
			(GLuint) newFont->glyph->advance.x
		};
		returned.insert(std::pair<GLchar, Character>(c, character));
	}
	FT_Done_Face(newFont);
	FT_Done_FreeType(ftLibrary);
	return returned;
}

void textsBegin(){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int vertShader = createShader("assets/shaders/textVert.txt", GL_VERTEX_SHADER);
	int fragShader = createShader("assets/shaders/textFrag.txt", GL_FRAGMENT_SHADER);
	textShader = createProgram({ vertShader, fragShader });
	//load fonts
	int textCount = allTexts.size();
	for (int i = 0; i < textCount; i++) {
		text currentText = allTexts[i];
		if(!currentText.loadFonts){
			continue;
		}
		allTexts[i].fontCharacters = getFont(currentText.fontPath, currentText.fontSize);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	//ready vbo & vao
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

vec2 renderText(string displayedText, vec2 position, float alpha, float size, vec3 colour, 
	map<GLchar, Character> Characters){
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL); // allows text over skybox

	mat4 projectionMatrix = ortho(0.0f, static_cast<GLfloat>(display_x),
		0.0f, static_cast<GLfloat>(display_y));
	setMat4(textShader, "projection", projectionMatrix);

	glUseProgram(textShader);
	glActiveTexture(GL_TEXTURE0);
	int textureLocation = glGetUniformLocation(textShader, "text");
	glUniform1i(textureLocation, 0);
	int colourLocation = glGetUniformLocation(textShader, "textColor");
	glUniform4f(colourLocation, colour.x, colour.y, colour.z, alpha);

	glBindVertexArray(textVAO);

	float totalWidth = 0.0f;
	float totalHeight = 0.0f;

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = displayedText.begin(); c != displayedText.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = position.x + ch.Bearing.x * size;
		GLfloat ypos = position.y - (ch.Size.y - ch.Bearing.y) * size;

		GLfloat w = ch.Size.x * size;
		GLfloat h = ch.Size.y * size;

		totalWidth += ((ch.Advance >> 6) * size) + ((ch.Size.x >> 6) * size);
		if(h > totalHeight){
			totalHeight = h;
		}

		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		position.x += (ch.Advance >> 6) * size; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	return vec2(totalWidth, totalHeight);
}

void renderTexts(){
	int size = allTexts.size();
	for (int i = 0; i < size; i++) {
		text currentText = allTexts[i];
		if (!currentText.active) {
			continue;
		}
		vec2 usedTextPosition = currentText.position;
		if(currentText.centered){
			usedTextPosition.x -= currentText.totalWidth / 2.0f;
			usedTextPosition.y -= currentText.totalHeight / 2.0f;
		}
		vec2 widthHeight = renderText(currentText.displayedText, usedTextPosition, currentText.alpha,
			currentText.size, currentText.colour, currentText.fontCharacters);
		allTexts[i].totalWidth = widthHeight.x;
		allTexts[i].totalHeight = widthHeight.y;
	}
}

void interfaceBegin(){
	buttonsBegin();
	textsBegin();
}

void interfaceMainloop(){
	renderTexts();
	updateMousePos();
	registerClicks();
	randomInt(0, 1000);
}

void interfaceLastcall(){
	renderButtons();
	renderTexts();
}

double mousePosX, mousePosY;
void updateMousePos() {
	glfwGetCursorPos(window, &mousePosX, &mousePosY);
}

bool checkKey(int key){
	vector<int> glfwMouse = { GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_MIDDLE };
	vector<int> blitzMouse = { 256256, 128128, 512512 };
	for (int i = 0; i < 3; i++) {
		if (key == blitzMouse[i]) {
			int state = glfwGetMouseButton(window, glfwMouse[i]);
			if (state == GLFW_PRESS) {
				return true;
			}
			return false;
		}
	}
	int keyState = glfwGetKey(window, key);
	if (keyState == GLFW_PRESS) {
		return true;
	}
	return false;
}

vector<bool> allKeysPrevious(146);
bool rightButtonDown, leftButtonDown, middleButtonDown;
vector<int> keyIndexes = { 32, 39, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68,
							69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 96, 
							161, 162, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 
							274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 
							294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 
							314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 
							334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347 };
bool checkKeyDown(int key) {
	vector<bool> booleans = { rightButtonDown, leftButtonDown, middleButtonDown };
	vector<int> blitzMouse = { 256256, 128128, 512512 };
	bool returned = false;
	for (int m = 0; m < 3; m++) {
		if (key == blitzMouse[m]) {
			if (checkKey(blitzMouse[m])) {
				if (!booleans[m]) {
					returned = true;
				}
				booleans[m] = true;
				continue;
			}
			booleans[m] = false;
		}
	}
	// update vector
	rightButtonDown = booleans[0];
	leftButtonDown = booleans[1];
	middleButtonDown = booleans[2];
	if (key == 256256 || key == 128128 || key == 512512) {
		return returned;
	}
	// keys
	if (checkKey(key)) {
		vector<int>::iterator iterator = find(keyIndexes.begin(), keyIndexes.end(), key);
		int index = std::distance(keyIndexes.begin(), iterator);
		if (!allKeysPrevious[index]) {
			return true;
		}
	}
	return false;
}

// all glfw keys
void updateKeys() {
	for (int i = 0; i < 146; i++) {
		allKeysPrevious[i] = false;
		if (checkKey(keyIndexes[i])) {
			allKeysPrevious[i] = true;
		}
	}
}