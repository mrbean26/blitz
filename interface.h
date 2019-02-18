#include <freeglut.h>

class button {
public:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec2 scale = glm::vec2(1.0f, 1.0f);

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

class text {
public:
	std::string text_name = "";
	std::string displayed_text = "";

	glm::vec2 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec2 scale = glm::vec2(1.0f, 1.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	void *font = GLUT_STROKE_ROMAN;//
};

void draw_text(const char * display_text, glm::vec2 position,
	void *font, display used_display, int length,
		glm::vec2 scale, glm::vec3 rotation) {
	glMatrixMode(GL_PROJECTION);
	double *matrix = new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX, matrix);
	glLoadIdentity();
	glOrtho(0, used_display.used_width, 0, used_display.used_height, 
		-5, 5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(position.x, position.y, 0.0f);
	glScalef(scale.x, scale.y, 1.0f);
	glRotatef(1.0f, rotation.x, rotation.y, rotation.z);
	for (int i = 0; i < length; i++) {
		glutStrokeCharacter(font, (int)display_text[i]);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);
	glMatrixMode(GL_MODELVIEW);
}

std::vector<text> all_texts;

int create_text() {
	text new_text;
	int new_vector_size = all_texts.size() + 1;
	all_texts.resize(new_vector_size);
	all_texts[new_vector_size - 1] = new_text;
	return new_vector_size - 1;
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
		int button_count = all_buttons.size();
		if (button_count < 1) {
			return;
		}
		for (int i = 0; i < button_count; i++) {
			button current_button = all_buttons[i];
			glBegin(GL_QUADS);
			glVertex3f(current_button.position.x-current_button.scale.x,
					current_button.position.y-current_button.scale.y, 
						current_button.position.x);
			glVertex3f(current_button.position.x - current_button.scale.x,
				current_button.position.y + current_button.scale.y,
				current_button.position.z);
			glVertex3f(current_button.position.x + current_button.scale.x,
				current_button.position.y + current_button.scale.y,
				current_button.position.z);
			glVertex3f(current_button.position.x + current_button.scale.x,
				current_button.position.y - current_button.scale.y,
				current_button.position.z);
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
		int button_count = all_buttons.size();
		if (button_count < 1) {
			return;
		}
		for (int i = 0; i < button_count; i++) {
			button &selected_button = all_buttons[i];
			float min_x = used_display.divider * (selected_button.position.x - selected_button.scale.x);
			float max_x = used_display.divider * (selected_button.position.x + selected_button.scale.x);
			float max_y = used_display.used_height-(used_display.divider * (selected_button.position.y - selected_button.scale.y));
			float min_y = used_display.used_height-(used_display.divider * (selected_button.position.y + selected_button.scale.y));
			if (mouse_x >= min_x && mouse_x <= max_x) {
				if (mouse_y >= min_y && mouse_y <= max_y) {
					selected_button.clicked = true;
					break; //button clicked
				}
			}
			
		}
	}
	void render_texts(display used_display) {
		int text_count = all_texts.size();
		if (text_count < 1) {
			return;
		}
		for (int i = 0; i < text_count; i++) {
			text current_text = all_texts[i];
			draw_text(current_text.displayed_text.data(), current_text.position,
				current_text.font, used_display, current_text.displayed_text.length(),
					current_text.scale, current_text.rotation);
		}
	}
	void interface_mainloop(display used_display) {
		render_buttons();
		register_clicks(used_display);
		render_texts(used_display);
	}
};

interface main_interface;