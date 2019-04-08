#include "backend.h"

GLFWwindow * window;

void backendMainloop() {
	interfaceMainloop();
	displayMainloop();
}

void backendBegin() {
	interfaceBegin();
}