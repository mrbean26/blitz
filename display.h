#pragma once
#ifndef DISPLAY_H
#define DISPLAY_H

#include <glew.h>
#include <glfw3.h>

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

#endif