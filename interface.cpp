#include "interface.h"

#include <vector>

unsigned int buttonTextureShader;
bool clickedDown = false;
bool lastClick = false;

GLuint buttonVAO, buttonVBO, buttonEBO;

std::vector<button> allButtons;

int createButton(){
	button newButton;
	int buttonCount = allButtons.size();
	allButtons.resize(buttonCount + 1);
	allButtons[buttonCount] = newButton;
	return buttonCount;
}

void renderButtons(){
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
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if (currentButton.mouseOver && currentButton.interactive) {
			position *= vec3(0.95f, 0.95f, 1.0f);
			scale *= vec2(1.05f, 1.05f);
		}
		if (currentButton.clickDown && currentButton.interactive) {
			scale *= vec2(0.95f, 0.95f);
			position *= vec3(1.05f, 1.05f, 1.0f);
		}
		//rescale the matrix and send position info to shader
		glScalef(scale.x, scale.y, 1.0f);
		int shaderButtonPosition = glGetUniformLocation(buttonTextureShader, "buttonPos");
		glUniform3f(shaderButtonPosition, position.x,
			position.y, position.z);
		//update position, scale and rotation info ready for the shader to use
		float modelviewMatrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrix);
		int matrixLocation = glGetUniformLocation(buttonTextureShader, "modelviewMatrix");
		glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, modelviewMatrix);
		//texture color
		int colourLocation = glGetUniformLocation(buttonTextureShader, "textureColour");
		vec3 color = currentButton.colour;
		glUniform4f(colourLocation, color.x, color.y, color.z, currentButton.alpha);
		//draw
		glUseProgram(buttonTextureShader);
		glActiveTexture(GL_TEXTURE0 + i);
		enableTexture(currentButton.texture);

		//set texture for shader
		int shaderTextureLocation = glGetUniformLocation(buttonTextureShader, "texture0");
		glUniform1i(shaderTextureLocation, i);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //if an error is being shown here for memory, shapes are being created before backendBegin()
	}
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
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
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
		allButtons[i].minY = minY;
		allButtons[i].maxY = maxY;
		//check for click
		allButtons[i].mouseOver = false;
		if (mouseX >= minX && mouseX <= maxX) {
			if (mouseY >= minY && mouseY <= maxY) {
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
		// positions then colors then texture coords
		 1.0f, 1.0f, 0.0f,
		 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		 -1.0f, -1.0f, 0.0f,
		 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		 -1.0f, 1.0f, 0.0f,
		 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
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
	vertexShader = createShader(textureVertSource, GL_VERTEX_SHADER);
	fragmentShader = createShader(textureFragSource, GL_FRAGMENT_SHADER);
	buttonTextureShader = createProgram({ vertexShader, fragmentShader });
}

//texts
vector<text> allTexts;
unsigned int textShader;
GLuint textVAO, textVBO;

int createText(){
	text newText;
	int size = allTexts.size();
	allTexts.resize(size + 1);
	allTexts[size] = newText;
	return size;
}

void textsBegin(){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int vertShader = createShader(textVertSource, GL_VERTEX_SHADER);
	int fragShader = createShader(textFragSource, GL_FRAGMENT_SHADER);
	textShader = createProgram({ vertShader, fragShader });
	//shader details
	mat4 projectionMatrix = ortho(0.0f, static_cast<GLfloat>(display_x),
		0.0f, static_cast<GLfloat>(display_y));
	int projectionLocation = glGetUniformLocation(textShader, "projection");
	glUseProgram(textShader);
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(projectionMatrix));
	//begin freetype
	FT_Library ftLibrary;
	if (FT_Init_FreeType(&ftLibrary)) {
		cout << "Couldn't Init Freetype" << endl;
		return;
	}
	//load fonts
	int textCount = allTexts.size();
	for (int i = 0; i < textCount; i++) {
		text currentText = allTexts[i];
		FT_Face newFont;
		if (FT_New_Face(ftLibrary, currentText.fontPath, 0, &newFont)) {
			cout << "Couldn't load font: " << currentText.fontPath << endl;
			continue;
		}
		FT_Set_Pixel_Sizes(newFont, 0, currentText.fontSize);
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
			allTexts[i].fontCharacters.insert(std::pair<GLchar, Character>(c, character));
		}
		FT_Done_Face(newFont);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_FreeType(ftLibrary);
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

void renderText(string displayedText, vec2 position, float alpha, float size, vec3 colour, 
	map<GLchar, Character> Characters){
	glActiveTexture(GL_TEXTURE0);
	int textureLocation = glGetUniformLocation(textShader, "text");
	glUniform1i(textureLocation, 0);
	glUseProgram(textShader);
	int colourLocation = glGetUniformLocation(textShader, "textColor");
	glUniform4f(colourLocation, colour.x, colour.y, colour.z, alpha);

	glBindVertexArray(textVAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = displayedText.begin(); c != displayedText.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = position.x + ch.Bearing.x * size;
		GLfloat ypos = position.y - (ch.Size.y - ch.Bearing.y) * size;

		GLfloat w = ch.Size.x * size;
		GLfloat h = ch.Size.y * size;
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
}

void renderTexts(){
	int size = allTexts.size();
	for (int i = 0; i < size; i++) {
		text currentText = allTexts[i];
		if (!currentText.active) {
			continue;
		}
		renderText(currentText.displayedText, currentText.position, currentText.alpha,
			currentText.size, currentText.colour, currentText.fontCharacters);
	}
}

void interfaceBegin(){
	buttonsBegin();
	textsBegin();
}

void interfaceMainloop(){
	renderButtons();
	registerClicks();
	renderTexts();
}
