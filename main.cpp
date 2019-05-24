#include "backend.h"
#include "frontend.h"

#include "structures.h"

int main(){
	if (!openglBegin(window, false, 800, 600)) {
		return -1;
	}
	frontendBegin();
	backendBegin();
	while (!glfwWindowShouldClose(window)){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		backendMainloop();
		frontendMainloop();
		insideMountain();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}