#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class texture {
public:
	GLuint textureId = 0;
	int width, height;
	unsigned char * data;
};

void loadTexture(const char * texturePath, texture& newTexture) {
	int width, height, channels;
	unsigned char * data = stbi_load(texturePath, &width, &height, &channels, STBI_rgb_alpha);
	if (!data) {
		std::cout << "Texture File: " << texturePath << "cannot be found." << std::endl;
		return;
	}
	GLuint textureId;
	glGenTextures(1, &textureId);
	newTexture.textureId = textureId;
	newTexture.width = width;
	newTexture.height = height;
	newTexture.data = data;
}

void enableTexture(texture usedTexture) {
	glBindTexture(GL_TEXTURE_2D, usedTexture.textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, usedTexture.width, 
		usedTexture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, usedTexture.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}