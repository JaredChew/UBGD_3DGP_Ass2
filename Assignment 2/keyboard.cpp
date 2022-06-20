#include "keyboard.h"

Keyboard::Keyboard(GLFWwindow* const window) : window(window) {

	previousTime = std::chrono::system_clock::now();

	holdKey = ' ';

	duration = 0.0f;

}

Keyboard::~Keyboard() { }

bool Keyboard::isPressed(char key) {

	return glfwGetKey(window, key);

}

bool Keyboard::isHold(char key) {

	if (isPressed(key)) {

		if (duration >= 0.5f){

			duration = 0.0f;

			return true;

		}


		holdKey = key;

	}

	return false;

}

void Keyboard::update() {

	previousTime = currentTime;
	currentTime = std::chrono::system_clock::now();

	deltaTime = currentTime - previousTime;

	if (isPressed(holdKey) && duration > 0.0f) { duration += deltaTime.count(); }
	else { duration = 0.0f; }

}