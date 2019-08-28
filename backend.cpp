#include "backend.h"

GLFWwindow * window;

void backendMainloop() { // run every frame
	interfaceMainloop();
	displayMainloop();
}

void backendBegin() { // bind bufferdata to VAO & VBO
	interfaceBegin();
}