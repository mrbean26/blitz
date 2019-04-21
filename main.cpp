#include "backend.h"
#include "frontend.h"

void keyCallback(GLFWwindow*window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_W) {
		cameraPosition.x += deltaTime * 8;
	}
	if (key == GLFW_KEY_S){
		cameraPosition.x -= deltaTime * 8;
	}
	if (key == GLFW_KEY_A) {
		cameraPosition.z -= deltaTime * 8;
	}
	if (key == GLFW_KEY_D) {
		cameraPosition.z += deltaTime * 8;
	}
	if (key == GLFW_KEY_DOWN) {
		cameraPosition.y -= deltaTime * 8;
	}
	if (key == GLFW_KEY_UP) {
		cameraPosition.y += deltaTime * 8;
	}
	if (key == GLFW_KEY_RIGHT) {
		cameraRotation.x += deltaTime * 60;
	}
	if (key == GLFW_KEY_LEFT) {
		cameraRotation.x -= deltaTime * 60;
	}
}

int main(){
	if (!openglBegin(window, false, 640, 360)) {
		return -1;
	}
	glfwSetKeyCallback(window, keyCallback);
	frontendBegin();
	backendBegin();
	while (!glfwWindowShouldClose(window)){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		backendMainloop();
		frontendMainloop();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}