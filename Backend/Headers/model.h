#pragma once

#include <glew.h>
#include <glfw3.h>

#include <vector>
using namespace std;

#include <glm/glm.hpp>
using namespace glm;

class model {
public:
	model(float colourMultiplier, const char* filePath);
	void render();

	vector<float> vertices;
	mat4 modelMat = mat4(1.0f);

	void outputVertices();
private:
	GLuint VAO, VBO, size;
};

void readyModel(const char* filePath);