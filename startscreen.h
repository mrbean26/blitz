int save_one_text, save_two_text, save_three_text;
int interact_save_one, interact_save_two, interact_save_three;

void start_start() {
	save_one_text = create_text();
	save_two_text = create_text();
	save_three_text = create_text();
	std::string displayed_texts[3] = { "Save 3", "Save 2", "Save 1" };
	float y_pos[3] = { 180.0f, 300.0f, 420.0f };
	for (int i = save_one_text; i <= save_three_text; i++) {
		all_texts[i].displayed_text = displayed_texts[i - save_one_text];
		all_texts[i].position = glm::vec2(100.0f,
			y_pos[i - save_one_text]);
		all_texts[i].scale = glm::vec2(0.7f, 0.7f);
		all_texts[i].color = glm::vec3(211.0f / 255.0f, 33.0f / 255.0f, 33.0f / 255.0f);
	}
	//buttons
	interact_save_one = create_button();
	interact_save_two = create_button();
	interact_save_three = create_button();
	float y_pos_buttons[3] = { 5.7f, 4.15f, 2.7f };
	for (int i = interact_save_one; i <= interact_save_three; i++) {
		all_buttons[i].position = glm::vec3(7.0f,
			y_pos_buttons[i - interact_save_one], 0.0f);
		all_buttons[i].scale = glm::vec2(1.0f, 0.45f);
	}
}

void start_main() {
	
}