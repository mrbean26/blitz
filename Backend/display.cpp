#include "display.h"
#include "worldGeneration.h"
#include "interface.h"
#include "frontend.h"
#include "structures.h"
#include "monsters.h"

#include <math.h>
#include <vector>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace glm;

#define DEFAULT_CAMERA_DISTANCE 10

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
	glfwSwapInterval(0); // unlimited fps
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

float deltaTime, lastTime;

double getFrames(){
	double currentTime = glfwGetTime();
	deltaTime = (float) (currentTime - lastTime);
	lastTime = (float) currentTime;
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
		newMatrix = rotate(newMatrix, radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
		newMatrix = rotate(newMatrix, radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
		newMatrix = rotate(newMatrix, radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
		return newMatrix;
	}
	newMatrix = translate(newMatrix, parentPosition);
	newMatrix = rotate(newMatrix, radians(parentRotation.y), vec3(0.0f, 1.0f, 0.0f));
	newMatrix = rotate(newMatrix, radians(parentRotation.x), vec3(1.0f, 0.0f, 0.0f));
	newMatrix = rotate(newMatrix, radians(parentRotation.z), vec3(0.0f, 0.0f, 1.0f));
	newMatrix = rotate(newMatrix, radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
	newMatrix = rotate(newMatrix, radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
	newMatrix = rotate(newMatrix, radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
	newMatrix = translate(newMatrix, position);
	newMatrix = glm::scale(newMatrix, scale);
	return newMatrix;
}

bool playerView = false;
bool aimingView = false;
float playerYaw = 0.0f;
float playerPitch = 0.0f;
float lowestCameraY = 0.0f;
float distanceFromCharacter = 0.0f;
float defaultDistance = DEFAULT_CAMERA_DISTANCE;
vec3 cameraThirdPos;
float var = 1.0f;
mat4 viewMatrix(){ // camera matrix - apply transformations to the opposite sign
	mat4 newMatrix = mat4(1.0f);
	// third person camera
	distanceFromCharacter = defaultDistance;
	float distanceAboveCharacter = 0.5f;
	// camera positions
	float cameraXPos = -(distanceFromCharacter * (-sin(radians(playerYaw)) * 
		cos(radians(playerPitch)))) + mainPlayer.position.x;
	float cameraZPos = -(distanceFromCharacter * (-cos(radians(playerYaw)) * 
		cos(radians(playerPitch)))) + mainPlayer.position.z;
	float cameraYPos = (-sin(radians(playerPitch)) * distanceFromCharacter) + 
		mainPlayer.position.y + distanceAboveCharacter;
	vec3 lookAtPos = vec3(mainPlayer.position.x, mainPlayer.position.y + 1.25f, mainPlayer.position.z);
	// monster colliders
	//cameraXPos = glm::clamp(cameraXPos, 0.1f, currentPlanetScale.x);
	//cameraZPos = glm::clamp(cameraZPos, -currentPlanetScale.y, 0.1f);
	cameraYPos = glm::clamp(cameraYPos, lowestCameraY, 10000.0f);
	cameraThirdPos = vec3(cameraXPos, cameraYPos, cameraZPos);
	float newDistance = monsterCameraColliders(cameraThirdPos, lookAtPos,
		defaultDistance);
	 cameraXPos = -(newDistance * (-sin(radians(playerYaw)) * 
		cos(radians(playerPitch)))) + mainPlayer.position.x;
	 cameraZPos = -(newDistance * (-cos(radians(playerYaw)) * 
		cos(radians(playerPitch)))) + mainPlayer.position.z;
	 cameraYPos = (-sin(radians(playerPitch)) * newDistance) + 
		mainPlayer.position.y + distanceAboveCharacter;
	// clamp values
	//cameraXPos = glm::clamp(cameraXPos, 0.1f, currentPlanetScale.x);
	//cameraZPos = glm::clamp(cameraZPos, -currentPlanetScale.y, 0.1f);
	cameraYPos = glm::clamp(cameraYPos, lowestCameraY, 10000.0f);
	cameraThirdPos = vec3(cameraXPos, cameraYPos, cameraZPos);
	cameraThirdPos = cameraBuildingCollisions(cameraThirdPos);
	// matrix
	newMatrix = glm::lookAt(cameraThirdPos, 
		lookAtPos, vec3(0.0f, 1.0f, 0.0f));
	// aiming
	if(playerView){
		if(mainPlayer.aiming && !mainPlayer.crouching){
			vec3 pPos = mainPlayer.position;
			vec3 pRot = mainPlayer.rotation;
			vec3 armRot = mainPlayer.armRotation;
            armRot.x = playerPitch + 90.0f;

			vec3 position = pPos;
			position.y += 0.9f;
			
			vec3 lookAt = pPos;
			lookAt.x = pPos.x - 1.2f * sin(radians(pRot.y)) * glm::clamp(sin(radians(armRot.x)), 0.0f, 1.0f);
			lookAt.y += 0.9f - sin(radians(armRot.x+ 90.0f)) * 1.2f;
			lookAt.z = pPos.z - 1.2f * cos(radians(pRot.y)) * glm::clamp(sin(radians(armRot.x)), 0.0f, 1.0f);

			newMatrix = glm::lookAt(position, lookAt, vec3(0.0f, 1.0f, 0.0f));
		}
	}
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
	distanceFromCharacter = glm::distance(lookAtPos, cameraThirdPos);
	distanceFromCharacter = glm::clamp(distanceFromCharacter, 0.0f, defaultDistance);
	return newMatrix;
}
