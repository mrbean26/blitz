#include "display.h"
#include "worldGeneration.h"
#include "interface.h"
#include "frontend.h"

#include <math.h>
#include <vector>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;
using namespace glm;

double aspect_x, aspect_y;

int display_x, display_y;

double frames, frameTime;

vector<vec2> faceCirclePoints;
vector<vec2> sideCirclePoints;
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
	// shader for mac
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
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
	glEnable(GL_DEPTH_TEST);
	//create circles for cam rot
	faceCirclePoints = circleCoords(vec2(0.0f, 0.0f), 1.0f, 360, 1.0f);
	//flip textures to correct way round
	stbi_set_flip_vertically_on_load(true);
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

float nearPlane = 0.1f;
float farPlane = 100.0f;

vec3 cameraPosition = vec3(0.0f, 4.5f, 10.0f);
vec3 cameraRotation = vec3(-15.0f, -90.0f, 0.0f);

mat4 projectionMatrix() {
	mat4 newMatrix = mat4(1.0f);
	newMatrix = perspective(radians(45.0f), (float) display_x / (float) display_y,
		nearPlane, farPlane);
	return newMatrix;
}

mat4 modelMatrix(vec3 position, vec3 rotation, vec3 scale, bool child, vec3 parentPosition, vec3 parentRotation) {
	mat4 newMatrix = mat4(1.0f);
	if (!child) {
		newMatrix = translate(newMatrix, position);
		newMatrix = glm::scale(newMatrix, scale);
		newMatrix = rotate(newMatrix, radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
		newMatrix = rotate(newMatrix, radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
		newMatrix = rotate(newMatrix, radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
		return newMatrix;
	}
	
	newMatrix = translate(newMatrix, parentPosition);
	newMatrix = rotate(newMatrix, radians(parentRotation.x), vec3(1.0f, 0.0f, 0.0f));
	newMatrix = rotate(newMatrix, radians(parentRotation.y), vec3(0.0f, 1.0f, 0.0f));
	newMatrix = rotate(newMatrix, radians(parentRotation.z), vec3(0.0f, 0.0f, 1.0f));
	newMatrix = rotate(newMatrix, radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
	newMatrix = rotate(newMatrix, radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
	newMatrix = rotate(newMatrix, radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
	newMatrix = translate(newMatrix, position);
	newMatrix = glm::scale(newMatrix, scale);
	return newMatrix;
}

bool playerView = false;
float playerYaw = 0.0f;
float playerPitch = 0.0f;

mat4 viewMatrix(){ // camera matrix - apply transformations to the opposite sign
	mat4 newMatrix = mat4(1.0f);
	// third person camera
	float distanceFromCharacter = 10.0f;
	float distanceAboveCharacter = 0.5f;
	// limit to area limits
	float cameraXPos = -(distanceFromCharacter * (-sin(radians(playerYaw)) * cos(radians(playerPitch)))) + mainPlayer.position.x;
	float cameraZPos = -(distanceFromCharacter * (-cos(radians(playerYaw)) * cos(radians(playerPitch)))) + mainPlayer.position.z;
	// give matrix data
	newMatrix = translate(newMatrix, -vec3(0.0f, distanceAboveCharacter, distanceFromCharacter)); // slightly above head & behind character slightly
	newMatrix = rotate(newMatrix, -radians(playerPitch), vec3(1.0f, 0.0f, 0.0f)); // x rot
	newMatrix = rotate(newMatrix, -radians(playerYaw), vec3(0.0f, 1.0f, 0.0f)); // y rot
	
	newMatrix = translate(newMatrix, -vec3(0.0f, mainPlayer.headLookAtY, 0.0f)); // translate to head
	newMatrix = rotate(newMatrix, -radians(mainPlayer.rotation.y), vec3(0.0f, 1.0f, 0.0f)); // with character rotation
	newMatrix = translate(newMatrix, -vec3(mainPlayer.position.x, 0.0f, mainPlayer.position.z)); // with player position
	// not 3rd person
	if (!playerView) {
		// clamp 0-360
		vec3 rotation = cameraRotation;
		for (int point = 0; point < 3; point++) {
			float currentValue = rotation[point];
			bool negative = false;
			if (currentValue < 0) {
				currentValue = currentValue * -1.0f;
				negative = true;
			}
			// for over 360
			float lowestFullTurn = floor(currentValue / 360.0f);
			lowestFullTurn = lowestFullTurn * 360.0f;
			currentValue = currentValue - lowestFullTurn;
			if (negative) {
				currentValue = 360 - currentValue;
			}
			// assign
			rotation[point] = currentValue;
		}
		// make not integers
		for (int point = 0; point < 3; point++) {
			float currentValue = rotation[point];
			if (floor(currentValue) == currentValue) {
				currentValue = currentValue + 0.05f;
			}
			// assign
			rotation[point] = currentValue;
		}
		// front
		vec3 cameraFront;
		cameraFront.x = cos(radians(rotation.x)) * cos(radians(rotation.y));
		cameraFront.y = sin(radians(rotation.x));
		cameraFront.z = cos(radians(rotation.x)) * sin(radians(rotation.y));
		// currently not possible to rotate more than one axis
		newMatrix = lookAt(cameraPosition, cameraPosition + cameraFront, vec3(0.0f, 1.0f, 0.0f));
	}
	
	return newMatrix;
}
