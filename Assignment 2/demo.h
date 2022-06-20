#pragma once

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <GLFW/glfw3.h>

#include "angle_util/Matrix.h"
#include "angle_util/geometry_utils.h"

#include "camera.h"
#include "opengl.h"
#include "objects.h"
#include "settings.h"
#include "keyboard.h"
#include "specification.h"

#define TEXTURE_COUNT 1
#define CUBE_MAP_COUNT 1

#define SHADER_PROGRAM_COUNT 6

#define OFF_TEXTURE_COUNT 6

class Demo {

private:

	Matrix4 mvpMatrix;
	Matrix4 modelMatrix;

	GLuint backBuffer;
	GLuint depthBuffer;

	GLuint shaderProgramID[SHADER_PROGRAM_COUNT] = { (GLuint)-1 };

	GLuint offTextureID[OFF_TEXTURE_COUNT];

	GLuint textureID[TEXTURE_COUNT];
	GLuint cubeMaptextureID[CUBE_MAP_COUNT];

	GLint GtexBGWidth = WINDOW_WIDTH / 1;
	GLint GtexBGHeight = WINDOW_HEIGHT / 1;

	Camera* camera;

	Keyboard* kbd;

	GLFWwindow* const window;

	float rotate;

public:
	Demo(GLFWwindow* const window);
	~Demo();

	void renderSkyBox(const GLuint& texture);
	void renderObject(const GLuint& texture);
	void filterLightPass(const GLuint& texture);
	void blur(const int& direction, const GLuint& texture, const float& blurRadius, const float& totalWeigh);
	void textureBlend(const int& blendType, const GLuint& toBlend, const GLuint& texture);
	void finalRender(const GLuint& texture);

	void draw();

};