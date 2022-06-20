#pragma once

#include <GLFW/glfw3.h>

#include "angle_util/Matrix.h"

class Camera {

private:

	GLFWwindow* const window;

	float cameraYaw;
	float cameraPitch;
	float cameraDistance;

	float cameraSide;
	float cameraScroll;
	float cameraDepth;

	float cameraSpeed;

public:

	Matrix4 gPerspectiveMatrix;
	Matrix4 gViewMatrix;

public:

	Camera(GLFWwindow * const window, const float& cameraYaw, const float& cameraPitch, const float& cameraDistance, const float& cameraSide, const float& cameraScroll, const float& cameraDepth, const float& cameraSpeed);
	~Camera();

	Matrix4 getViewMatrixWithoutTranslate(void);

	void updateCamera();

};