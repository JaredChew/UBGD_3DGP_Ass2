#include "camera.h"

Camera::Camera(GLFWwindow* const window, const float& cameraYaw, const float& cameraPitch, const float& cameraDistance, const float& cameraSide, const float& cameraScroll, const float& cameraDepth, const float& cameraSpeed) : window(window) {

	this->cameraYaw = cameraYaw;
	this->cameraPitch = cameraPitch;
	this->cameraDistance = cameraDistance;

	this->cameraSide = cameraSide;
	this->cameraScroll = cameraScroll;
	this->cameraDepth = cameraDepth;

	this->cameraSpeed = cameraSpeed;

}

Camera::~Camera() { }

Matrix4 Camera::getViewMatrixWithoutTranslate(void) {
	return Matrix4::rotate(cameraYaw, Vector3(1.0f, 0.0f, 0.0f)) * Matrix4::rotate(cameraPitch, Vector3(0.0f, 1.0f, 0.0f));
}

void Camera::updateCamera() {

	if (glfwGetKey(window, 'A')) cameraPitch -= cameraSpeed;
	if (glfwGetKey(window, 'D')) cameraPitch += cameraSpeed;

	if (glfwGetKey(window, 'W')) cameraYaw -= cameraSpeed;
	if (glfwGetKey(window, 'S')) cameraYaw += cameraSpeed;

	if (glfwGetKey(window, 'R')) { cameraDistance -= cameraSpeed * 0.02f; }
	if (glfwGetKey(window, 'F')) cameraDistance += cameraSpeed * 0.02f;

	if (glfwGetKey(window, 'X')) cameraSide += cameraSpeed * 0.02f;
	if (glfwGetKey(window, 'C')) cameraSide -= cameraSpeed * 0.02f;

	if (glfwGetKey(window, 'Q')) cameraScroll += cameraSpeed * 0.02f;
	if (glfwGetKey(window, 'E')) cameraScroll -= cameraSpeed * 0.02f;

	if (glfwGetKey(window, 'Z')) cameraDepth += cameraSpeed;
	if (glfwGetKey(window, 'V')) cameraDepth -= cameraSpeed;

	gViewMatrix = Matrix4::translate(Vector3(cameraSide, cameraScroll, -cameraDistance)) *
				  Matrix4::rotate(cameraYaw, Vector3(1.0f, 0.0f, 0.0f)) *
				  Matrix4::rotate(cameraPitch, Vector3(0.0f, 1.0f, 0.0f)) *
				  Matrix4::rotate(cameraDepth, Vector3(0.0f, 0.0f, 1.0f));

}