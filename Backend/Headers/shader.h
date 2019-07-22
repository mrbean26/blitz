#pragma once
#ifndef SHADER_H
#define SHADER_H

using namespace std;
#include <vector>

#include <glew.h>
#include <glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

int createShader(const char * filePath, GLenum shaderType); // create shader and assign source to it

int createProgram(vector<int> shaders); // combine shaders into a program

void setMat4(int shader, const char* matrixName, mat4 usedMatrix);

void setShaderInt(int shader, const char* intName, int usedInt);

void setShaderFloat(int shader, const char* floatName, float usedFloat);

void setShaderVecThree(int shader, const char* vecName, vec3 usedVec);

#endif 