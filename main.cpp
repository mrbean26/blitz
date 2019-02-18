#include <glew.h>
#include <glfw3.h>

#include "backend.h"

int main(int argc, char * argv[]){
	if (!opengl_context(argc, argv)) {
		return -1;
	}
	int b = create_button();
	int t = create_text();
	all_texts[t].displayed_text = "Hello";
	all_texts[t].position = glm::vec2(400.0f, 100.0f);

	while (!glfwWindowShouldClose(main_display.main_window)){
		glClear(GL_COLOR_BUFFER_BIT);

		glColor3f(1.0f, 1.0f, 1.0f);

		backend_mainloop();
		if (all_buttons[b].clicked) {
			std::cout << "HELLO" << std::endl;
			all_buttons[b].clicked = false;
		}
		glfwSwapBuffers(main_display.main_window);

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}