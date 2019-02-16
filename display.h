class display {
public:
	GLFWwindow* main_window;

	int used_width;
	int used_height;

	int divider;

	void create_window(GLFWwindow*& used_window, bool fullscreen, 
			int window_width, int window_height) {
		GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* monitor_mode = glfwGetVideoMode(primary_monitor);
		int larger = monitor_mode->width;
		if (monitor_mode->height > monitor_mode->width) {
			larger = monitor_mode->height;
		}
		if (fullscreen) {
			//set window size, refresh rate, and create a window
			glfwWindowHint(GLFW_REFRESH_RATE, monitor_mode->refreshRate);
			//create
			used_window = glfwCreateWindow(monitor_mode->width,
				monitor_mode->height, "Blitz", primary_monitor, NULL);
			used_width = monitor_mode->width;
			used_height = monitor_mode->height;
		}
		if (!fullscreen) {
			used_window = glfwCreateWindow(window_width,
				window_height, "Blitz", NULL, NULL);
			used_width = window_width;
			used_height = window_height;
		}
		glfwSetWindowSizeLimits(used_window, used_width, used_height, used_width, used_height);
	}
	void resize_window(GLFWwindow*& used_window,
		int new_width, int new_height) {
		glfwSetWindowSize(used_window, new_width, new_height);
		glfwSetWindowSizeLimits(used_window, new_width, new_height, new_width, new_height);
	}
	void mainloop() {
		//aspect ratio
		int aspect_x = 0;
		int aspect_y = 0;
		int larger = used_width;
		if (used_width > used_height) {
			larger = used_height;
		}
		for (int i = larger; i > 0; i--) {//get highest common factor
			float width = used_width / (float)i;
			float height = used_height / (float)i;
			if (width == ceil(width)) {
				if (height == ceil(height)) {
					divider = i;
					aspect_x = (int)width;
					aspect_y = (int)height;
					break;
				}
			}
		}
		glLoadIdentity();
		glOrtho(0.0, aspect_x, 0.0, aspect_y, -1.0, 1.0);
	}
};