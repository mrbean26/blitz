#include "backend.h"

int main(){
	stbi_set_flip_vertically_on_load(1);
	if (!openglBegin(window, false, 1280, 720)) {
		return -1;
	}
	backendBegin();
	while (!glfwWindowShouldClose(window)){
		glClear(GL_COLOR_BUFFER_BIT);

		backendMainloop();

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}