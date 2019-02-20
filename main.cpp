#include <glew.h>
#include <glfw3.h>

#include "frontend.h"

int main(int argc, char * argv[]){
	if (!opengl_context(argc, argv)) {
		return -1;
	}
	frontend_start();
	while (!glfwWindowShouldClose(main_display.main_window)){
		glClear(GL_COLOR_BUFFER_BIT);	
	
		backend_mainloop(); frontend_mainloop();

		glfwSwapBuffers(main_display.main_window);

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}