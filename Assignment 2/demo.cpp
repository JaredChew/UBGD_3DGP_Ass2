#include "demo.h"

Demo::Demo(GLFWwindow* const window) : window(window) {

	GLint linked;

	GLuint vertexShader[SHADER_PROGRAM_COUNT];
	GLuint fragmentShader[SHADER_PROGRAM_COUNT];

	//Init camera
	camera = new Camera(window, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, 0.0f, 3.0f);
	kbd = new Keyboard(window);

	//load textures
	glGenTextures(TEXTURE_COUNT, textureID);
	OpenGL::loadTexture("../media/wood.bmp", textureID[TEXTURE_OBJECT]);

	//load cubemaps
	glGenTextures(1, &cubeMaptextureID[CUBE_MAP_MOUNTAIN]);
	std::vector<std::string> skybox_Mountain{

		 "../media/right.bmp",
		 "../media/left.bmp",
		 "../media/bottom.bmp",
		 "../media/top.bmp",
		 "../media/front.bmp",
		 "../media/back.bmp"

	};
	OpenGL::loadCubemapTexture(skybox_Mountain, cubeMaptextureID[CUBE_MAP_MOUNTAIN]);

	//Init frame buffer
	glGenFramebuffers(1, &backBuffer);

	//Init depth buffer
	glGenTextures(1, &depthBuffer);
	glBindTexture(GL_TEXTURE_2D, depthBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, GtexBGWidth, GtexBGHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Init off textures
	for (int i = 0; i < OFF_TEXTURE_COUNT; ++i) {

		OpenGL::initTexture(offTextureID[i], 1, GtexBGWidth, GtexBGHeight);

	}

	//Load shader files
	vertexShader[SHADER_PROGRAM_DEFAULT] = OpenGL::loadShaderFromFile(GL_VERTEX_SHADER, "../default.vert");
	fragmentShader[SHADER_PROGRAM_DEFAULT] = OpenGL::loadShaderFromFile(GL_FRAGMENT_SHADER, "../default.frag");

	vertexShader[SHADER_PROGRAM_SKYBOX] = OpenGL::loadShaderFromFile(GL_VERTEX_SHADER, "../skybox.vert");
	fragmentShader[SHADER_PROGRAM_SKYBOX] = OpenGL::loadShaderFromFile(GL_FRAGMENT_SHADER, "../skybox.frag");

	vertexShader[SHADER_PROGRAM_LGHTPASS] = OpenGL::loadShaderFromFile(GL_VERTEX_SHADER, "../default.vert");
	fragmentShader[SHADER_PROGRAM_LGHTPASS] = OpenGL::loadShaderFromFile(GL_FRAGMENT_SHADER, "../filterLight.frag");

	vertexShader[SHADER_PROGRAM_BLUR] = OpenGL::loadShaderFromFile(GL_VERTEX_SHADER, "../default.vert");
	fragmentShader[SHADER_PROGRAM_BLUR] = OpenGL::loadShaderFromFile(GL_FRAGMENT_SHADER, "../blur.frag");

	vertexShader[SHADER_PROGRAM_BLEND] = OpenGL::loadShaderFromFile(GL_VERTEX_SHADER, "../default.vert");
	fragmentShader[SHADER_PROGRAM_BLEND] = OpenGL::loadShaderFromFile(GL_FRAGMENT_SHADER, "../blend.frag");

	vertexShader[SHADER_PROGRAM_DEPTH] = OpenGL::loadShaderFromFile(GL_VERTEX_SHADER, "../default.vert");
	fragmentShader[SHADER_PROGRAM_DEPTH] = OpenGL::loadShaderFromFile(GL_FRAGMENT_SHADER, "../depth.frag");

	//Init shader programs
	for (int i = 0; i < SHADER_PROGRAM_COUNT; ++i) {

		if (!OpenGL::initProgramObject_Shader(shaderProgramID[i], fragmentShader[i], vertexShader[i])) {
			return;
		}

	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//initialize matrices
	camera->gPerspectiveMatrix = Matrix4::perspective(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.5f, 400.0f);
	//camera->gOthorMatrix = Matrix4::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.5f, 30.0f);

	camera->gViewMatrix = Matrix4::translate(Vector3(0.0f, 0.0f, -15.0f));

	rotate = 0.0f;

}

Demo::~Demo() {

	delete camera;
	camera = nullptr;

	delete kbd;
	kbd = nullptr;

}

void Demo::renderSkyBox(const GLuint& texture) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID[SHADER_PROGRAM_SKYBOX]);

	glUniform1i(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_SKYBOX], "samplerCube1"), 0);

	mvpMatrix = camera->gPerspectiveMatrix * camera->getViewMatrixWithoutTranslate() * modelMatrix;
	//mvpMatrix = gPerspectiveMatrix * gViewMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_SKYBOX], "uMvpMatrix"), 1, GL_FALSE, mvpMatrix.data);

	Objects::drawSkyBox(texture);

}

void Demo::renderObject(const GLuint& texture) {

	glUseProgram(shaderProgramID[SHADER_PROGRAM_DEFAULT]);

	glUniform1i(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_DEFAULT], "sampler2d"), 0);

	mvpMatrix = camera->gPerspectiveMatrix * camera->gViewMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_DEFAULT], "uMvpMatrix"), 1, GL_FALSE, mvpMatrix.data);

	Objects::drawSquare(texture);

}

void Demo::filterLightPass(const GLuint& texture) {

	glUseProgram(shaderProgramID[SHADER_PROGRAM_LGHTPASS]);

	// set the sampler2D varying variable to the first texture unit(index 0)
	glUniform1i(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_LGHTPASS], "sampler2d"), 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset the mvpMatrix to identity matrix so that it renders fully on texture in normalized device coordinates
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_LGHTPASS], "uMvpMatrix"), 1, GL_FALSE, Matrix4::identity().data);

	Objects::drawSquare(offTextureID[OFF_TEXTURE_BASE]);

}

void Demo::blur(const int &direction, const GLuint& texture, const float& blurRadius, const float& totalWeight) {

	glUseProgram(shaderProgramID[SHADER_PROGRAM_BLUR]);

	// set the sampler2D varying variable to the first texture unit(index 0)
	glUniform1i(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_BLUR], "sampler2d"), 0);

	//========pass texture size to shader
	glUniform1f(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_BLUR], "uTextureW"), (GLfloat)WINDOW_WIDTH);
	glUniform1f(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_BLUR], "uTextureH"), (GLfloat)WINDOW_HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset the mvpMatrix to identity matrix so that it renders fully on texture in normalized device coordinates
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_BLUR], "uMvpMatrix"), 1, GL_FALSE, Matrix4::identity().data);

	// tell the shader to apply horizontal blurring, for details please check the "uBlurDirection" flag in the shader code
	glUniform1i(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_BLUR], "uBlurDirection"), direction);

	//Set blur radius
	glUniform1f(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_BLUR], "radius"), blurRadius);

	//Set total weight
	glUniform1f(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_BLUR], "weight"), totalWeight);

	Objects::drawSquare(texture);

}

void Demo::textureBlend(const int& blendType, const GLuint& toBlend, const GLuint& texture) {

	glUseProgram(shaderProgramID[SHADER_PROGRAM_BLEND]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset the mvpMatrix to identity matrix so that it renders fully on texture in normalized device coordinates
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_BLEND], "uMvpMatrix"), 1, GL_FALSE, Matrix4::identity().data);

	glUniform1i(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_BLEND], "blendType"), blendType);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, toBlend);

	glUniform1i(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_BLEND], "toBlend"), 1);

	glActiveTexture(GL_TEXTURE0 + 0);
	//glBindTexture(GL_TEXTURE_2D, toBlend2);

	glUniform1i(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_BLEND], "sampler2d"), 0);

	Objects::drawSquare(texture);

}

void Demo::finalRender(const GLuint& texture) {

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(shaderProgramID[SHADER_PROGRAM_DEFAULT]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset the mvpMatrix to identity matrix so that it renders fully on texture in normalized device coordinates
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_DEFAULT], "uMvpMatrix"), 1, GL_FALSE, Matrix4::identity().data);

	// set the sampler2D varying variable to the first texture unit(index 0)
	glUniform1i(glGetUniformLocation(shaderProgramID[SHADER_PROGRAM_DEFAULT], "sampler2d"), 0);

	Objects::drawSquare(texture);

}

void Demo::draw(void) {

	camera->updateCamera();

	kbd->update();

	//Set the viewport
	glViewport(0, 0, GtexBGWidth, GtexBGHeight);

	//Bind framebuffer to backbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, backBuffer);

	//set depth buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	//set render target to base
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, offTextureID[OFF_TEXTURE_BASE], 0);

	if (OpenGL::checkIsFramebufferReady()) {
		
		//skybox
		modelMatrix = Matrix4::scale(Vector3(50.0f, 50.0f, 50.0f));

		renderSkyBox(cubeMaptextureID[CUBE_MAP_MOUNTAIN]);

		//first object
		modelMatrix = Matrix4::rotate(rotate, Vector3(2.0f, 1.0f, 0.0f)) *
					  Matrix4::translate(Vector3(5.0f, 7.0f, -10.0f)) *
					  Matrix4::scale(Vector3(7.0f, 7.0f, 7.0f));

		renderObject(textureID[TEXTURE_OBJECT]);

		//second object
		modelMatrix = Matrix4::rotate(rotate / 2, Vector3(4.0f, 1.0f, 0.0f)) *
					  Matrix4::translate(Vector3(-5.0f, 7.0f, 10.0f)) *
					  Matrix4::scale(Vector3(5.0f, 5.0f, 5.0f));

		renderObject(textureID[TEXTURE_OBJECT]);

		//third object
		modelMatrix = Matrix4::rotate(rotate / 5, Vector3(0.0f, 1.0f, 0.0f)) *
					  Matrix4::translate(Vector3(2.0f, 5.0f, -15.0f)) *
					  Matrix4::scale(Vector3(10.0f, 10.0f, 10.0f));

		renderObject(textureID[TEXTURE_OBJECT]);

	}

	//set render target to light_pass
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, offTextureID[OFF_TEXTURE_LIGHTPASS], 0);

	//light pass
	if (OpenGL::checkIsFramebufferReady()) {
		filterLightPass(offTextureID[OFF_TEXTURE_BASE]);
	}

	//set render target to blur_pass_horizontal
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, offTextureID[OFF_TEXTURE_BLURPASSHORIZONTAL], 0);

	//blurr horizontal
	if (OpenGL::checkIsFramebufferReady()) {
		blur(0, offTextureID[OFF_TEXTURE_LIGHTPASS], BLUR_RADIUS, BLUR_WEIGHT);
	}

	//set render target to blur_pass_vertical
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, offTextureID[OFF_TEXTURE_BLURPASSVERTICAL], 0);

	//blur vertical
	if (OpenGL::checkIsFramebufferReady()) {
		blur(1, offTextureID[OFF_TEXTURE_BLURPASSHORIZONTAL], BLUR_RADIUS, BLUR_WEIGHT);
	}
	
	//set render target to blend
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, offTextureID[OFF_TEXTURE_BLEND], 0);

	//Additive blend 2 textures
	if (OpenGL::checkIsFramebufferReady()) {
		textureBlend(0, offTextureID[OFF_TEXTURE_BASE], offTextureID[OFF_TEXTURE_BLURPASSVERTICAL]);
	}

	finalRender(offTextureID[OFF_TEXTURE_BLEND]);

	rotate += ROTATE_SPEED;
	
}


