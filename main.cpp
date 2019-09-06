#include "backend.h"
#include "frontend.h"

int main() {
	if (!openglBegin(window, false, 1280, 720)) {
		return -1;
	}
	beginRandom();
	frontendBegin();
	backendBegin();
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		backendMainloop();
		frontendMainloop();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	endRandom();
	glfwTerminate();
	return 0;
}