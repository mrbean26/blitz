#include "shader.h"
#include "saveFiles.h"

#include <glew.h>
#include <glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <iostream>
#include <vector>

int createShader(const char * filePath, GLenum shaderType){
	// get lines from file
	string allLines;
	vector<string> shaderLines = readLines(filePath);
	for (string line : shaderLines) {
		allLines = allLines + line + "\n";
	}
	const char * shaderSource = allLines.data();
	// compile
	int newShader = glCreateShader(shaderType);
	glShaderSource(newShader, 1, &shaderSource, NULL);
	glCompileShader(newShader);
	int compileResult;
	char infoLog[512];
	glGetShaderiv(newShader, GL_COMPILE_STATUS, &compileResult);
	if (!compileResult) {
		glGetShaderInfoLog(newShader, 512, NULL, infoLog);
		cout << "Shader compilation error: " << infoLog << endl;
		return 0;
	}
	return newShader;
}

int createProgram(vector<int> shaders){
	int newProgram = glCreateProgram();
	int shaderCount = shaders.size();
	for (int i = 0; i < shaderCount; i++) {
		glAttachShader(newProgram, shaders[i]);
	}
	glLinkProgram(newProgram);
	return newProgram;
}

void setMat4(int shader, const char * matrixName, mat4 usedMatrix) {
	int location = glGetUniformLocation(shader, matrixName);
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(usedMatrix));
}
