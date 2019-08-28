#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <glew.h>
#include <glfw3.h>

#include <vector>
using namespace std;

#include <glm/glm.hpp>
using namespace glm;

#include "texture.h"

class model {
public:
	model(float colourMultiplier, const char* filePath);
	void render();

	vector<float> vertices;
	mat4 modelMat = mat4(1.0f);

	void outputVertices();
	GLuint VAO, VBO, size;
};

extern int modelTextureShader;
void startModelTextureShader();

class textureModel{
public:
	textureModel(const char * filePath, texture usedTexture);
	void render(mat4 model);

	vector<float> vertices;
	texture modelTexture;

	void outputVertices();
	GLuint VAO, VBO, size;
};

//extern texture blankTexture;
class readyTextureModel{
public:
	readyTextureModel(std::vector<float> usedVertices = {}, texture usedTexture = emptyTexture, bool initiate = false);
	
	texture modelTexture;
	GLuint VAO, VBO, size;

	void render(mat4 model, bool useLight, vec3 multiply = vec3(1.0f));
};

void readyTextureModelfile(const char * filePath);
void readyModel(const char* filePath);

#endif