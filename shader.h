#pragma once
#ifndef SHADER_H
#define SHADER_H

using namespace std;
#include <vector>

#include <glew.h>
#include <glfw3.h>

int createShader(const char * filePath, GLenum shaderType); // create shader and assign source to it

int createProgram(vector<int> shaders); // combine shaders into a program

#endif 