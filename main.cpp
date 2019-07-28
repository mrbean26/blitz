#include "backend.h"
#include "frontend.h"

int main(){
	if (!openglBegin(window, false, 1200, 675)) {
		return -1;
	}
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