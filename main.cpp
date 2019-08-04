#include "backend.h"
#include "frontend.h"

int main(){
	if (!openglBegin(window, false, 1280, 720)) {
		return -1;
	}
	frontendBegin();
	backendBegin();
	model m(vec4(0.55f, 0.37f, 0.2f, 0.05f), "assets/models/oilCan.obj");
	m.outputVertices();
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