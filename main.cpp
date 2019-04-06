#include "backend.h"
#include "frontend.h"

int main(){
	if (!openglBegin(window, true, 1600, 900)) {
		return -1;
	}
	frontendBegin();
	backendBegin();
	while (!glfwWindowShouldClose(window)){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_TEXTURE_2D);
		backendMainloop();
		frontendMainloop();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}