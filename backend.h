#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "display.h"

#include "savefiles.h"

#include "interface.h"

display main_display;

bool opengl_context() {
	if (!glfwInit()) {
		std::cout << "GLFW Error" << std::endl;
		return false;
	}
	main_display.create_window(main_display.main_window, false, 1280, 720);
	if (!main_display.main_window) {
		glfwTerminate();
		std::cout << "GLFW Error" << std::endl;
		return false;
	}
	glfwMakeContextCurrent(main_display.main_window);
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW Error" << std::endl;
		return false;
	}
	return true;
}

void backend_mainloop() {
	main_display.mainloop();
	main_interface.interface_mainloop(main_display);
}