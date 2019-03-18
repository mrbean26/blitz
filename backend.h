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

void backendMainloop() {
	interfaceMainloop();
	displayMainloop();
}

void backendBegin() {
	interfaceBegin();
}