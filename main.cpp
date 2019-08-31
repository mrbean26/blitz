#include "backend.h"
#include "frontend.h"

int main(){
	if (!openglBegin(window, true, 1280, 720)) {
		return -1;
	}
    beginRandom();
	frontendBegin();
	backendBegin();
	while (!glfwWindowShouldClose(window)){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		backendMainloop();
		frontendMainloop();
		
        if(checkKey(GLFW_KEY_P)){
            alert("No Charge. Alert Preview To Test. Why would do this.");
        }
        
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	endRandom();
	glfwTerminate();
	return 0;
}
