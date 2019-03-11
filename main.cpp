#include "backend.h"
#include "frontend.h"

int main(){
	stbi_set_flip_vertically_on_load(1);
	if (!openglBegin(window, false, 1280, 720)) {
		return -1;
	}
	frontendBegin();
	backendBegin();
	while (!glfwWindowShouldClose(window)){
		glClear(GL_COLOR_BUFFER_BIT);

		backendMainloop();
		frontendMainloop();

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}