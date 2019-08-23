#include "backend.h"

#include <glm.hpp>

GLFWwindow * window;

void backendMainloop() {
	interfaceMainloop();
	displayMainloop();
	debugMainloop();
}

void backendBegin() {
	debugBegin();
	interfaceBegin();
	startModelTextureShader();
}

double lastMouseX, lastMouseY;
vec2 mouseDifferences() {
	double newMouseX, newMouseY;
	glfwGetCursorPos(window, &newMouseX, &newMouseY);
	vec2 returned = vec2(newMouseX - lastMouseX, lastMouseY-newMouseY);
	lastMouseX = newMouseX; lastMouseY = newMouseY;
	return returned;
}
