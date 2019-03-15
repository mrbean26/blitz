using namespace std;

const char * textureVertSource = { //rendering the texture onto quads (and repositioning them)
	"#version 330 core\n"
	"layout(location = 0) in vec3 aPos;\n"
	"layout(location = 2) in vec2 aTexCoord;\n"
	"out vec2 TexCoord;\n"
	"uniform mat4 modelviewMatrix;\n"
	"uniform vec3 buttonPos;\n"
	"void main(){\n"
	"   vec3 changedPos = vec3(aPos.x+buttonPos.x, aPos.y+buttonPos.y, aPos.z+buttonPos.z);\n"
	"	gl_Position = vec4(changedPos, 1.0) * modelviewMatrix;\n"
	"	TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
	"}\0"
};

const char * textureFragSource = {
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"uniform vec4 textureColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
	"in vec2 TexCoord;\n"
	"uniform sampler2D texture0;\n" //this corresponds to the texture unit (0-31) assigned by using glActiveTexture()
	"void main(){\n"
	"	FragColor = texture(texture0, TexCoord) * textureColour;\n"
	"}\0"
};

int createShader(const char * shaderSource, GLenum shaderType) { 
	int newShader = glCreateShader(shaderType);
	glShaderSource(newShader, 1, &shaderSource, NULL);
	glCompileShader(newShader);
	int compileResult;
	char infoLog[512];
	glGetShaderiv(newShader, GL_COMPILE_STATUS, &compileResult);
	if (!compileResult) {
		glGetShaderInfoLog(newShader, 512, NULL, infoLog);
		cout << "Shader compilation error: " << infoLog << endl;
		return 0;
	}
	return newShader;
}

int createProgram(std::vector<int> shaders) { //attach shaders
	int newProgram = glCreateProgram();
	int shaderCount = shaders.size();
	for (int i = 0; i < shaderCount; i++) {
		glAttachShader(newProgram, shaders[i]);
	}
	glLinkProgram(newProgram);
	return newProgram;
}