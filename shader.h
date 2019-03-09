using namespace std;

const char * textureVertSource = {
	"#version 330 core\n"
	"layout(location = 0) in vec3 aPos;\n"
	"layout(location = 2) in vec2 aTexCoord;\n"
	"out vec2 TexCoord;\n"
	"uniform mat4 modelviewMatrix;\n"
	"void main(){\n"
	"	gl_Position = vec4(aPos, 1.0) * modelviewMatrix;\n"
	"	TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
	"}\0"
};

const char * textureFragSource = {
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"uniform vec4 textureColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
	"in vec2 TexCoord;\n"
	"uniform sampler2D texture1;\n"
	"void main(){\n"
	"	FragColor = texture(texture1, TexCoord) * textureColour;\n"
	"}\0"
};

int createShader(const char * shaderSource, GLenum shaderType) {
	int newShader = glCreateShader(shaderType);
	glShaderSource(newShader, 1, &shaderSource, NULL);
	glCompileShader(newShader);
	int compileResult;//
	glGetShaderiv(newShader, GL_COMPILE_STATUS, &compileResult);
	if (!compileResult) {
		cout << "Shader compilation error." << endl;
		return 0;
	}
	return newShader;
}

int createProgram(std::vector<int> shaders) {
	int newProgram = glCreateProgram();
	int shaderCount = shaders.size();
	for (int i = 0; i < shaderCount; i++) {
		glAttachShader(newProgram, shaders[i]);
	}
	glLinkProgram(newProgram);
	return newProgram;
}