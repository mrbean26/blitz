#pragma once
#ifndef DISPLAY_H
#define DISPLAY_H

#include <glew.h>
#include <glfw3.h>

#include "player.h"

#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

extern double aspect_x, aspect_y;

extern int display_x, display_y;

extern double frames, frameTime;

int aspectDivider(int x, int y); //return highest common factor between x and y

bool openglBegin(GLFWwindow* &used_window, bool fullscreen,
	int width, int height); //create window and assign an aspect ratio

void resizeWindow(GLFWwindow* &used_window,
	int new_width, int new_height); 

extern double deltaTime, lastTime;
double getFrames(); //get fps of window, to get fps frontend, use double 'frames'

void displayMainloop(); // run every frame

extern float nearPlane;
extern float farPlane;

extern vec3 cameraPosition;
extern vec3 cameraRotation;

mat4 projectionMatrix();
mat4 modelMatrix(vec3 position = vec3(0.0f), vec3 rotation = vec3(0.0f), vec3 scale = vec3(1.0f), bool child = false, vec3 parentPosition = vec3(0.0f), vec3 parentRotation=vec3(0.0f));

extern bool playerView;
extern float playerYaw, playerPitch;
mat4 viewMatrix();

#endif