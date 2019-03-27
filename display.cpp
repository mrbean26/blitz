#include "display.h"

#include <math.h>

double aspect_x, aspect_y;

int display_x, display_y;

double frames, frameTime;

int aspectDivider(int x, int y){
	int max = x;
	if (y > x) {
		max = y;
	}
	for (int i = max; i > 0; i--) {//get highest common factor
		float width = x / (float)i;
		float height = y / (float)i;
		if (width == ceil(width)) {
			if (height == ceil(height)) {
				aspect_x = x / i;
				aspect_y = y / i;
				return i;
			}
		}
	}
	return 1;
}

bool openglBegin(GLFWwindow *& used_window, bool fullscreen, 
	int width, int height){
	if (!glfwInit()) {
		return false;
	}
	GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode * monitor_mode = glfwGetVideoMode(primary_monitor);
	int monitor_x = monitor_mode->width;
	int monitor_y = monitor_mode->height;
	//stop resize
	glewExperimental = GL_TRUE;
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//used variables
	int used_x;
	int used_y;
	if (fullscreen) {
		used_window = glfwCreateWindow(monitor_x, monitor_y, "Blitz",
			primary_monitor, NULL);
		used_x = monitor_x;
		used_y = monitor_y;
	}
	if (!fullscreen) {
		used_window = glfwCreateWindow(width, height,
			"Blitz", NULL, NULL);
		used_x = width;//
		used_y = height;
	}
	glfwMakeContextCurrent(used_window);
	if (glewInit() != GLEW_OK) {
		return false;
	}
	// assign variables
	display_x = used_x;
	display_y = used_y;
	//get aspect ratio
	aspect_x = used_x / aspectDivider(used_x, used_y); //gives 16:10
	aspect_y = used_y / aspectDivider(used_x, used_y);
	//give 10:x
	double divider = aspect_x / 10.0;
	aspect_x = aspect_x / divider;
	aspect_y = aspect_y / divider;
	//glOrtho(0.0, aspect_x, 0.0, aspect_y, -1.0, 1.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return true;
}

void resizeWindow(GLFWwindow *& used_window, 
	int new_width, int new_height){
	glfwSetWindowSize(used_window, new_width, new_height);
	//aspect
	int window_x, window_y;
	glfwGetWindowSize(used_window, &window_x, &window_y);
	int aspect_x = window_x / aspectDivider(window_x,
		window_y);
	int aspect_y = window_y / aspectDivider(window_x,
		window_y);
}

double deltaTime, lastTime;

double getFrames(){
	double currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;
	return 1.0 / deltaTime;
}

void displayMainloop(){
	frames = getFrames();
}
