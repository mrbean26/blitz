#include "backend.h"

int main(){
	if (!openglBegin(window, false, 1280, 720)) {
		return -1;
	}
	while (!glfwWindowShouldClose(window)){
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		backendMainloop();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}