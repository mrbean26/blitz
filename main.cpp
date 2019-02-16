#include <glew.h>
#include <glfw3.h>

#include "backend.h"

int main(void){
	if (!opengl_context()) {
		return -1;
	}

	while (!glfwWindowShouldClose(main_display.main_window)){
		glClear(GL_COLOR_BUFFER_BIT);

		backend_mainloop();
	
		glfwSwapBuffers(main_display.main_window);

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}