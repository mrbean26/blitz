#pragma once
#ifndef SHADER_H
#define SHADER_H

using namespace std;
#include <vector>

#include <glew.h>
#include <glfw3.h>

extern const char * textureVertSource;

extern const char * textureFragSource;

extern const char * textVertSource;

extern const char * textFragSource;

int createShader(const char * shaderSource, GLenum shaderType); // create shader and assign source to it

int createProgram(vector<int> shaders); // combine shaders into a program

#endif 