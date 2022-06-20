#pragma once

#include <GLFW/glfw3.h>

#include <chrono>

class Keyboard {

private:

	GLFWwindow* const window;

	char holdKey;

	float duration;

	std::chrono::duration<float> deltaTime;
	
	std::chrono::time_point<std::chrono::system_clock> previousTime;
	std::chrono::time_point<std::chrono::system_clock> currentTime;

public:
	Keyboard(GLFWwindow* const window);
	~Keyboard();

	bool isPressed(char key);
	bool isHold(char key);

	void update();

};