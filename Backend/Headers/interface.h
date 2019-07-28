#pragma once
#ifndef INTERFACE_H
#define INTERFACE_H

#include <glew.h>
#include <glfw3.h>

#include "display.h"
#include "texture.h"
#include "shader.h"
#include "backend.h"

#include <iostream>
#include <map>
using namespace std;

#include <glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

#include <ft2build.h>
#include FT_FREETYPE_H

#define DEFAULT_FONT_SIZE 11

template<class T>
int newVectorPos(vector<T> * used) {
	int size = used->size();
	used->resize(size + 1);
	return size;
}

//buttons
class button {
public:
	string name = "New Button";

	vec3 position = vec3(0.0f, 0.0f, 0.0f);
	vec2 scale = vec2(1.0f, 1.0f);
	float rotation = 0.0f;

	bool clickUp = false;
	bool clickDown = false;

	bool interactive = true;
	bool mouseOver = false;

	int minX = 0;
	int minY = 0;
	int maxY = 0;

	texture texture;

	vec3 colour = vec3(1.0f, 1.0f, 1.0f);
	float alpha = 1.0f;

	bool active = true;
};

extern vector<button> allButtons;

int createButton(vec2 size = vec2(1.0f), vec3 position = vec3(1.0f), bool interactive = true); // return position in allbuttons vector

void renderButtons(); //bind vertexarray and draw with texture

void registerClicks(); //lots of maths

void buttonsBegin(); //start button VBO, VAO & EBO and reserve in memory

//texts
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	ivec2 Size;    // Size of glyph
	ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

class text {
public:
	string name = "New Text";
	string displayedText = "New Text";

	vec2 position = vec2(1.2f, 0.0f); //center of screen
	float size = 1.0f;
	int fontSize = DEFAULT_FONT_SIZE;

	const char * fontPath;
	map<GLchar, Character> fontCharacters;
	vec3 colour = vec3(1.0f, 1.0f, 1.0f);
	float alpha = 1.0f;

	bool active = true;
	bool loadFonts = true;
};

map<GLchar, Character> getFont(const char * path, int fontsize);

extern vector<text> allTexts;

int createText(); // return position in alltexts vector

void textsBegin(); // reserve memory size for quads and load in fonts

void renderText(string displayedText, vec2 position, float alpha,
	float size, vec3 colour, map<GLchar, Character> Characters); //draw text on screen

void renderTexts(); //bind vertex array and pass to shader alongside colour

//main internewFont
void interfaceBegin(); // all begin

void interfaceMainloop(); // run every frame

void interfaceLastcall(); // called last 

// key inputs
bool checkKey(int key);

extern bool rightButtonDown, leftButtonDown, middleButtonDown;
bool checkKeyDown(int key);
void updateKeys();

extern double mousePosX, mousePosY;
void updateMousePos();

#endif
