#include "backend.h"
#include "frontend.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main(){
	stbi_set_flip_vertically_on_load(true);
	if (!openglBegin(window, false, 960, 540)) {
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