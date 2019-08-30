#include "backend.h"
#include "frontend.h"

int main(){
	if (!openglBegin(window, false, 640, 360)) {
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
            alert("FUCKING ALERT CUNT LOOK AT THE FUCKING COMPUTER");
        }
        
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	endRandom();
	glfwTerminate();
	return 0;
}
