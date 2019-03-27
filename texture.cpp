#include "texture.h"

#include <glew.h>
#include <glfw3.h>

#include <stb_image.h> //image library

#include <iostream>
using namespace std;

texture loadTexture(const char * filePath){
	texture newTexture;
	glGenTextures(1, &newTexture.textureId);
	newTexture.data = stbi_load(filePath, &newTexture.width,
		&newTexture.height, &newTexture.channels, STBI_rgb_alpha);
	if (!newTexture.data)
		cout << "File cannot be found: " << filePath << endl;
	return newTexture;
}

void enableTexture(texture usedTexture){
	glBindTexture(GL_TEXTURE_2D, usedTexture.textureId);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, usedTexture.width,
		usedTexture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, usedTexture.data);
	glGenerateMipmap(GL_TEXTURE_2D);
}
