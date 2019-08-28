#include "display.h"
#include "worldGeneration.h"

#include <math.h>
#include <vector>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;
using namespace glm;

double aspectX, aspectY;

int displayX, displayY;

double frames, frameTime;

vector<vec2> faceCirclePoints;
vector<vec2> sideCirclePoints;
int aspectDivider(int x, int y){ // get the highest common factor between to integers -  divide the display x & y to get an aspect ratio
	int max = x;
	if (y > x) {
		max = y;
	}
	for (int i = max; i > 0; i--) {//get highest common factor
		float width = x / (float)i;
		float height = y / (float)i;
		if (width == ceil(width)) {
			if (height == ceil(height)) {
				aspectX = x / i;
				aspectY = y / i;
				return i;
			}
		}
	}
	return 1;
}

bool openglBegin(GLFWwindow *& usedWindow, bool fullscreen, 
	int width, int height){ // create window & setup gl variables
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
	//used variables
	int usedX;
	int usedY;
	if (fullscreen) {
		usedWindow = glfwCreateWindow(monitor_x, monitor_y, "Blitz",
			primary_monitor, NULL);
		usedX = monitor_x;
		usedY = monitor_y;
	}
	if (!fullscreen) {
		usedWindow = glfwCreateWindow(width, height,
			"Blitz", NULL, NULL);
		usedX = width;//
		usedY = height;
	}
	glfwMakeContextCurrent(usedWindow);
	if (glewInit() != GLEW_OK) {
		return false;
	}
	// assign variables
	displayX = usedX;
	displayY = usedY;
	//get aspect ratio
	aspectX = usedX / aspectDivider(usedX, usedY); //gives 16:10
	aspectY = usedY / aspectDivider(usedX, usedY);
	//give 10:x
	double divider = aspectX / 10.0;
	aspectX = aspectX / divider;
	aspectY = aspectY / divider;
	//glOrtho(0.0, aspectX, 0.0, aspectY, -1.0, 1.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	//create circles for cam rot
	faceCirclePoints = circleCoords(vec2(0.0f, 0.0f), 1.0f, 360, 1.0f);
	//flip textures to correct way round
	stbi_set_flip_vertically_on_load(true);
	return true;
}

void resizeWindow(GLFWwindow *& usedWindow, 
	int new_width, int new_height){
	glfwSetWindowSize(usedWindow, new_width, new_height);
	//aspect
	aspectX = new_width / aspectDivider(new_width,
		new_height);
	aspectY = new_height / aspectDivider(new_width,
		new_height);
}

double deltaTime, lastTime;

double getFrames(){ // do not use this to return fps - use frames variables
	double currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;
	return 1.0 / deltaTime;
}

void displayMainloop(){
	frames = getFrames();
}

// view distances
float nearPlane = 0.1f;
float farPlane = 100.0f;

vec3 cameraPosition = vec3(25.0f, 60.0f, -25.0f); // in center of world looking downwards
vec3 cameraRotation = vec3(0.0f, 0.0f, 0.0f);

mat4 projectionMatrix() { // setting up view distances
	mat4 newMatrix = mat4(1.0f);
	newMatrix = perspective(radians(45.0f), (float) displayX / (float) displayY,
		nearPlane, farPlane);
	return newMatrix;
}

mat4 modelMatrix() { // this can be used for rotating shapes & objects
	mat4 newMatrix = mat4(1.0f);
	return newMatrix;
}

mat4 viewMatrix(){ // camera matrix - apply transformations to the opposite sign
	mat4 newMatrix = mat4(1.0f);
	// points
	vec3 xRot = vec3(0.0f, 0.0f, 0.0f);
	xRot.z = faceCirclePoints[std::round(cameraRotation.x)].x;
	xRot.y = faceCirclePoints[std::round(cameraRotation.x)].y;

	vec3 yRot = vec3(0.0f, 0.0f, 0.1f);
	yRot.x = faceCirclePoints[std::round(cameraRotation.y)].x;
	yRot.y = faceCirclePoints[std::round(cameraRotation.y)].y;

	vec3 zRot = vec3(0.0f, 0.0f, 0.0f);
	zRot.x = faceCirclePoints[std::round(cameraRotation.z)].x;
	zRot.z = faceCirclePoints[std::round(cameraRotation.z)].y;
	//combine
	vec3 cameraFront = vec3(0.0f, 0.0f, 0.0f); // involved in the rotation of the camera
	// cameraFront = (xRot+yRot+zRot) / vec3(3.0f, 3.0f, 3.0f); // this could be used
	// cameraFront = vec3((yRot.x+zRot.x)/2.0f, (xRot.y+yRot.y)/2.0f, (xRot.z+zRot.z)/2.0f); // this may also work

	// currently not possible to rotate more than one axis
	newMatrix = lookAt(cameraPosition, cameraPosition + cameraFront, vec3(0.0f, 1.0f, 0.0f));
	return newMatrix;
}
