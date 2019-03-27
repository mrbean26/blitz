#include "backend.h"

//#include <glfw3.h>

GLFWwindow * window;

void backendMainloop() {
	interfaceMainloop();
	displayMainloop();
}

void backendBegin() {
	interfaceBegin();
}