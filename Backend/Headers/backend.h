#pragma once
#ifndef BACKEND_H
#define BACKEND_H

//game engine 

#include <cmath>
#include <vector>
#include <iostream>

#include <glm.hpp>

#include "display.h"

extern GLFWwindow * window;

#include "shader.h"
#include "texture.h"
#include "interface.h"
#include "saveFiles.h"
#include "model.h"

void backendMainloop(); // run every frame

void backendBegin(); // run before first frame

vec2 mouseDifferences();
#endif