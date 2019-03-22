//game engine 

#include <cmath>
#include <vector>
#include <iostream>

#include <glm.hpp>

#include "display.h"

GLFWwindow * window;

#include "shader.h"
#include "texture.h"
#include "interface.h"
#include "saveFiles.h"

void backendMainloop() { //called every frame
	interfaceMainloop();
	displayMainloop();
}

void backendBegin() { //called before first frame
	interfaceBegin();
}