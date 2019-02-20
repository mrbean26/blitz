#include <iostream>
#include <vector>

#include <vec2.hpp>
#include <vec3.hpp>

#include "display.h"

#include "savefiles.h"

#include "interface.h"

display main_display;

bool opengl_context(int argc, char * argv[]) {
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
	glutInit(&argc, argv);
	return true;
}

void backend_mainloop() {
	main_display.mainloop();
	main_interface.interface_mainloop(main_display);
}

class texture {
public:
	GLuint texture_id;
	int width;
	int height;
	unsigned char * data;
};

GLuint load_texture(const char * texture_path, texture& assigned_texture) {
	//texture variables
	unsigned char header[54];
	unsigned int data_pos;
	unsigned int width, height;
	unsigned int image_size;
	unsigned char * data;
	//check that is BMP
	FILE * file = fopen(texture_path, "rb");
	if (!file) {
		std::cout << "File could not be opened" << std::endl;
		return 0;
	}
	if (fread(header, 1, 54, file) != 54) {
		std::cout << "Not a correct BMP file" << std::endl;
		return false;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		std::cout << "Not a correct BMP file" << std::endl;
		return 0;
	}
	//declare variables
	data_pos = *(int*)&(header[0x0A]);
	image_size = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	if (image_size == 0) {
		image_size = width * height * 3;
	}
	if (data_pos == 0) {
		data_pos = 54;
	}
	data = new unsigned char[image_size];
	fread(data, 1, image_size, file);
	fclose(file);
	//generate texture
	GLuint texture_id;
	glGenTextures(1, &texture_id);
	//assingn to class 
	assigned_texture.data = data;
	assigned_texture.height = height;
	assigned_texture.width = width;
	assigned_texture.texture_id = texture_id;
	return texture_id;
}

void enable_texture(texture used_texture) {
	glBindTexture(GL_TEXTURE_2D, used_texture.texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, used_texture.width, 
		used_texture.height, 0, GL_BGR, GL_UNSIGNED_BYTE, used_texture.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//enable
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	//make not repeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}