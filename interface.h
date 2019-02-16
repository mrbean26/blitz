class button {
public:
	float position_x=0.0f;
	float position_y=0.0f;
	float position_z=0.0f;

	float scale_x=1.0f;
	float scale_y=1.0f;

	std::string button_name = "";

	bool clicked = false;
};

std::vector<button> all_buttons;
int create_button() {
	button new_button;
	int new_size = all_buttons.size() + 1;
	all_buttons.resize(new_size);
	all_buttons[new_size - 1] = new_button;
	return new_size - 1;
}

class interface {
public:
	bool last_click = false;
	bool click_down() {
		if (!last_click) {
			return true;
		}
		return false;
	}
	void render_buttons() {
		for (int i = 0; i < all_buttons.size(); i++) {
			button current_button = all_buttons[i];
			glBegin(GL_QUADS);
			glVertex3f(current_button.position_x-current_button.scale_x,
					current_button.position_y-current_button.scale_y, 
						current_button.position_z);
			glVertex3f(current_button.position_x - current_button.scale_x,
				current_button.position_y + current_button.scale_y,
				current_button.position_z);
			glVertex3f(current_button.position_x + current_button.scale_x,
				current_button.position_y + current_button.scale_y,
				current_button.position_z);
			glVertex3f(current_button.position_x + current_button.scale_x,
				current_button.position_y - current_button.scale_y,
				current_button.position_z);
			glEnd();
		}
	}
	void register_clicks(display used_display) {
		double mouse_x, mouse_y;
		glfwGetCursorPos(used_display.main_window, &mouse_x, &mouse_y);
		int click_state = glfwGetMouseButton(used_display.main_window,
			GLFW_MOUSE_BUTTON_LEFT); //check if mouse1 is clicked
		if (click_state != GLFW_PRESS) {
			last_click = false;
			return; //not clicking
		}
		if (!click_down()) {
			return; //mouse is being held down
		}
		last_click = true;
		for (int i = 0; i < all_buttons.size(); i++) {
			button &selected_button = all_buttons[i];
			float min_x = used_display.divider * (selected_button.position_x - selected_button.scale_x);
			float max_x = used_display.divider * (selected_button.position_x + selected_button.scale_x);
			float min_y = used_display.divider * (selected_button.position_y - selected_button.scale_y);
			float max_y = used_display.divider * (selected_button.position_y + selected_button.scale_y);
			if (mouse_x >= min_x && mouse_x <= max_x) {
				if (mouse_y >= min_y && mouse_y <= max_y) {
					selected_button.clicked = true;
					break; //button clicked
				}
			}
			
		}
	}
	void interface_mainloop(display used_display) {
		render_buttons();
		register_clicks(used_display);
	}
};

interface main_interface;

