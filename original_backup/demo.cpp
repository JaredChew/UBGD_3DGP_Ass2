#define GLFW_INCLUDE_ES2 1
#define GLFW_DLL 1

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include "angle_util/Matrix.h"
#include "angle_util/geometry_utils.h"
#include "bitmap.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define TEXTURE_COUNT 7

GLint GprogramID_Default = -1;
GLint GprogramID_SkyBox = -1;
GLint GprogramID_Bloom = -1;
GLint GprogramID_Blur = -1;
GLint GprogramID_Blend = -1;
GLint GprogramID_Depth = -1;

GLuint GtextureID[TEXTURE_COUNT];
GLuint GCubeMaptextureID;

GLuint Gframebuffer;
GLuint Gdepthbuffer;

GLuint GtextureBG;
GLuint Gblurpass1texture;
GLuint Gblurpass2texture;
GLuint Gbloomtexture;
GLuint Gblendtexture;
GLuint Gdepthtexture;

GLint GtexBGWidth = WINDOW_WIDTH / 1;
GLint GtexBGHeight = WINDOW_HEIGHT / 1;

GLFWwindow* window;

Matrix4 gPerspectiveMatrix;
Matrix4 gViewMatrix;

#define RECT_VERTICE_W 6
#define RECT_VERTICE_H 6

//every quad has 6 vertices
const int RECT_VERTEX_COUNT = RECT_VERTICE_W * RECT_VERTICE_H * 6;

//every vertex has 3 components(x, y z)
const int RECT_VERTEX_ARRAY_SIZE = RECT_VERTEX_COUNT * 3;

const int RECT_UV_ARRAY_SIZE = RECT_VERTEX_COUNT * 2;

GLfloat mRectVertices[RECT_VERTEX_ARRAY_SIZE];
GLfloat mRectUV[RECT_UV_ARRAY_SIZE];

float cameraYaw = 0.0f;
float cameraPitch = 0.0f;
float cameraDistance = 5.5f;

void genPlane(void)
{
	const float width = 2.0f;
	const float height = 2.0f;
	const float halfWidth = width * 0.5f;
	const float halfHeight = height * 0.5f;

	const float texMul = 1.0f;

	int currentVert = -1;
	int currentIndex = -1;
	for (int h = 0; h < RECT_VERTICE_H; h++)
	{
		for (int w = 0; w < RECT_VERTICE_W; w++)
		{
			//========= 6 vertices to form one sub-rectangle
			//1st vertex
			int vertex1 = ++currentVert;
			mRectVertices[vertex1 * 3] = -halfWidth + (float)(w) / (float)RECT_VERTICE_W * width;
			mRectVertices[vertex1 * 3 + 1] = -halfHeight + (float)(h) / (float)RECT_VERTICE_H * height;
			mRectVertices[vertex1 * 3 + 2] = 0.0f;
			mRectUV[vertex1 * 2] = (float)(w) / (float)RECT_VERTICE_W * texMul;
			mRectUV[vertex1 * 2 + 1] = (float)(h) / (float)RECT_VERTICE_H * texMul;

			//2nd vertex
			int vertex2 = ++currentVert;
			mRectVertices[vertex2 * 3] = -halfWidth + (float)(w) / (float)RECT_VERTICE_W * width;
			mRectVertices[vertex2 * 3 + 1] = -halfHeight + (float)(h + 1) / (float)RECT_VERTICE_H * height;
			mRectVertices[vertex2 * 3 + 2] = 0.0f;
			mRectUV[vertex2 * 2] = (float)(w) / (float)RECT_VERTICE_W * texMul;
			mRectUV[vertex2 * 2 + 1] = (float)(h + 1) / (float)RECT_VERTICE_H * texMul;


			//3rd vertex
			int vertex3 = ++currentVert;
			mRectVertices[vertex3 * 3] = -halfWidth + (float)(w + 1) / (float)RECT_VERTICE_W * width;
			mRectVertices[vertex3 * 3 + 1] = -halfHeight + (float)(h + 1) / (float)RECT_VERTICE_H * height;
			mRectVertices[vertex3 * 3 + 2] = 0.0f;
			mRectUV[vertex3 * 2] = (float)(w + 1) / (float)RECT_VERTICE_W * texMul;
			mRectUV[vertex3 * 2 + 1] = (float)(h + 1) / (float)RECT_VERTICE_H * texMul;


			//4th vertex
			int vertex4 = ++currentVert;
			mRectVertices[vertex4 * 3] = mRectVertices[vertex3 * 3];
			mRectVertices[vertex4 * 3 + 1] = mRectVertices[vertex3 * 3 + 1];
			mRectVertices[vertex4 * 3 + 2] = mRectVertices[vertex3 * 3 + 2];
			mRectUV[vertex4 * 2] = mRectUV[vertex3 * 2];
			mRectUV[vertex4 * 2 + 1] = mRectUV[vertex3 * 2 + 1];


			//5th vertex
			int vertex5 = ++currentVert;
			mRectVertices[vertex5 * 3] = -halfWidth + (float)(w + 1) / (float)RECT_VERTICE_W * width;
			mRectVertices[vertex5 * 3 + 1] = -halfHeight + (float)(h) / (float)RECT_VERTICE_H * height;
			mRectVertices[vertex5 * 3 + 2] = 0.0f;
			mRectUV[vertex5 * 2] = (float)(w + 1) / (float)RECT_VERTICE_W * texMul;
			mRectUV[vertex5 * 2 + 1] = (float)(h) / (float)RECT_VERTICE_H * texMul;

			//6th vertex
			int vertex6 = ++currentVert;
			mRectVertices[vertex6 * 3] = mRectVertices[vertex1 * 3];
			mRectVertices[vertex6 * 3 + 1] = mRectVertices[vertex1 * 3 + 1];
			mRectVertices[vertex6 * 3 + 2] = mRectVertices[vertex1 * 3 + 2];
			mRectUV[vertex6 * 2] = mRectUV[vertex1 * 2];
			mRectUV[vertex6 * 2 + 1] = mRectUV[vertex1 * 2 + 1];
		}
	}
}

void drawSkyBox()
{
	static float skyboxVertices[] =
	{
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, skyboxVertices);

	glEnableVertexAttribArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, GCubeMaptextureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDisableVertexAttribArray(0);
}

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

GLuint LoadShader(GLenum type, const char* shaderSrc)
{
	GLuint shader;
	GLint compiled;

	// Create the shader object
	shader = glCreateShader(type);

	if (shader == 0)
		return 0;

	// Load the shader source
	glShaderSource(shader, 1, &shaderSrc, NULL);

	// Compile the shader
	glCompileShader(shader);

	// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		GLint infoLen = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char infoLog[4096];
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			printf("Error compiling shader:\n%s\n", infoLog);
		}

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

GLuint LoadShaderFromFile(GLenum shaderType, std::string path)
{
	GLuint shaderID = 0;
	std::string shaderString;
	std::ifstream sourceFile(path.c_str());

	if (sourceFile)
	{
		shaderString.assign((std::istreambuf_iterator< char >(sourceFile)), std::istreambuf_iterator< char >());
		const GLchar* shaderSource = shaderString.c_str();

		return LoadShader(shaderType, shaderSource);
	}
	else
		printf("Unable to open file %s\n", path.c_str());

	return shaderID;
}


void loadTexture(const char* path, GLuint textureID)
{
	CBitmap bitmap(path);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// bilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.GetWidth(), bitmap.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.GetBits());
}

void loadCubemapTexture(std::vector<std::string> facesPath, GLuint textureID)
{
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (int i = 0; i < facesPath.size(); i++)
	{
		CBitmap bitmap(facesPath[i].c_str());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, bitmap.GetWidth(), bitmap.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.GetBits());
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glDisable(GL_TEXTURE_CUBE_MAP);
}

int Init(void)
{

	glfwSwapInterval(1);

	GLuint vertexShader_Default;
	GLuint fragmentShader_Default;
	GLuint programObject_Default;

	GLuint vertexShader_SkyBox;
	GLuint fragmentShader_SkyBox;
	GLuint programObject_SkyBox;

	GLuint vertexShader_Bloom;
	GLuint fragmentShader_Bloom;
	GLuint programObject_Bloom;

	GLuint vertexShader_Blur;
	GLuint fragmentShader_Blur;
	GLuint programObject_Blur;

	GLuint vertexShader_Blend;
	GLuint fragmentShader_Blend;
	GLuint programObject_Blend;

	GLuint vertexShader_Depth;
	GLuint fragmentShader_Depth;
	GLuint programObject_Depth;

	GLint linked;

	//load textures
	glGenTextures(TEXTURE_COUNT, GtextureID);
	loadTexture("../media/rocks.bmp", GtextureID[0]);
	loadTexture("../media/glass.bmp", GtextureID[1]);
	loadTexture("../media/win8.bmp", GtextureID[2]);
	loadTexture("../media/fury_nano2.bmp", GtextureID[3]);
	//====

	glGenTextures(1, &GCubeMaptextureID);
	std::vector<std::string> skybox
	{
		 "../media/right.bmp",
		 "../media/left.bmp",
		 "../media/bottom.bmp",
		 "../media/top.bmp",
		 "../media/front.bmp",
		 "../media/back.bmp"
	};
	loadCubemapTexture(skybox, GCubeMaptextureID);

	//genPlane();

	//==================== set up frame buffer, render buffer, and create an empty texture for blurring purpose
	// create a new FBO
	glGenFramebuffers(1, &Gframebuffer);

	//create a new empty texture
	glGenTextures(1, &GtextureBG);
	glBindTexture(GL_TEXTURE_2D, GtextureBG);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GtexBGWidth, GtexBGHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//depth texture
	glGenTextures(1, &Gdepthtexture);
	glBindTexture(GL_TEXTURE_2D, Gdepthtexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GtexBGWidth, GtexBGHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//bloom texture
	glGenTextures(1, &Gbloomtexture);
	glBindTexture(GL_TEXTURE_2D, Gbloomtexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GtexBGWidth, GtexBGHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//blur texture
	glGenTextures(1, &Gblurpass1texture);
	glBindTexture(GL_TEXTURE_2D, Gblurpass1texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GtexBGWidth, GtexBGHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//blur texture
	glGenTextures(1, &Gblurpass2texture);
	glBindTexture(GL_TEXTURE_2D, Gblurpass2texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GtexBGWidth, GtexBGHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//blend texture
	glGenTextures(1, &Gblendtexture);
	glBindTexture(GL_TEXTURE_2D, Gblendtexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GtexBGWidth, GtexBGHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//depth buffer
	glGenTextures(1, &Gdepthbuffer);
	glBindTexture(GL_TEXTURE_2D, Gdepthbuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, GtexBGWidth, GtexBGHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// !! Init depth shader !! //

	vertexShader_Default = LoadShaderFromFile(GL_VERTEX_SHADER, "../default.vert");
	fragmentShader_Default = LoadShaderFromFile(GL_FRAGMENT_SHADER, "../default.frag");

	// Create the program object
	programObject_Default = glCreateProgram();

	if (programObject_Default == 0)
		return 0;

	glAttachShader(programObject_Default, vertexShader_Default);
	glAttachShader(programObject_Default, fragmentShader_Default);

	glBindAttribLocation(programObject_Default, 0, "vPosition");
	glBindAttribLocation(programObject_Default, 1, "vColor");
	glBindAttribLocation(programObject_Default, 2, "vTexCoord");

	// Link the program
	glLinkProgram(programObject_Default);

	// Check the link status
	glGetProgramiv(programObject_Default, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(programObject_Default, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char infoLog[1024];
			glGetProgramInfoLog(programObject_Default, infoLen, NULL, infoLog);
			printf("Error linking program:\n%s\n", infoLog);
		}

		glDeleteProgram(programObject_Default);
		return 0;
	}

	// Store the program object
	GprogramID_Default = programObject_Default;

	// !! Init Bloom shader program !! //

	vertexShader_Bloom = LoadShaderFromFile(GL_VERTEX_SHADER, "../default.vert");
	fragmentShader_Bloom = LoadShaderFromFile(GL_FRAGMENT_SHADER, "../bloom.frag");

	// Create the program object
	programObject_Bloom = glCreateProgram();

	if (programObject_Bloom == 0)
		return 0;

	glAttachShader(programObject_Bloom, vertexShader_Bloom);
	glAttachShader(programObject_Bloom, fragmentShader_Bloom);

	glBindAttribLocation(programObject_Bloom, 0, "vPosition");
	glBindAttribLocation(programObject_Bloom, 1, "vColor");
	glBindAttribLocation(programObject_Bloom, 2, "vTexCoord");

	// Link the program
	glLinkProgram(programObject_Bloom);

	// Check the link status
	glGetProgramiv(programObject_Bloom, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(programObject_Bloom, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char infoLog[1024];
			glGetProgramInfoLog(programObject_Bloom, infoLen, NULL, infoLog);
			printf("Error linking program:\n%s\n", infoLog);
		}

		glDeleteProgram(programObject_Bloom);
		return 0;
	}

	// Store the program object
	GprogramID_Bloom = programObject_Bloom;

	// !! Init skybox shader program !! //

	vertexShader_SkyBox = LoadShaderFromFile(GL_VERTEX_SHADER, "../skybox.vert");
	fragmentShader_SkyBox = LoadShaderFromFile(GL_FRAGMENT_SHADER, "../skybox.frag");

	// Create the program object
	programObject_SkyBox = glCreateProgram();

	if (programObject_SkyBox == 0)
		return 0;

	glAttachShader(programObject_SkyBox, vertexShader_SkyBox);
	glAttachShader(programObject_SkyBox, fragmentShader_SkyBox);

	glBindAttribLocation(programObject_SkyBox, 0, "vPosition");
	glBindAttribLocation(programObject_SkyBox, 1, "vTexCoord");

	// Link the program
	glLinkProgram(programObject_SkyBox);

	// Check the link status
	glGetProgramiv(programObject_SkyBox, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(programObject_SkyBox, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char infoLog[1024];
			glGetProgramInfoLog(programObject_SkyBox, infoLen, NULL, infoLog);
			printf("Error linking program:\n%s\n", infoLog);
		}

		glDeleteProgram(programObject_SkyBox);
		return 0;
	}

	// Store the program object
	GprogramID_SkyBox = programObject_SkyBox;

	// !! Init Blur shader !! //

	vertexShader_Blur = LoadShaderFromFile(GL_VERTEX_SHADER, "../default.vert");
	fragmentShader_Blur = LoadShaderFromFile(GL_FRAGMENT_SHADER, "../blur.frag");

	// Create the program object
	programObject_Blur = glCreateProgram();

	if (programObject_Blur == 0)
		return 0;

	glAttachShader(programObject_Blur, vertexShader_Blur);
	glAttachShader(programObject_Blur, fragmentShader_Blur);

	glBindAttribLocation(programObject_Blur, 0, "vPosition");
	glBindAttribLocation(programObject_Blur, 1, "vColor");
	glBindAttribLocation(programObject_Blur, 2, "vTexCoord");

	// Link the program
	glLinkProgram(programObject_Blur);

	// Check the link status
	glGetProgramiv(programObject_Blur, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(programObject_Blur, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char infoLog[1024];
			glGetProgramInfoLog(programObject_Blur, infoLen, NULL, infoLog);
			printf("Error linking program:\n%s\n", infoLog);
		}

		glDeleteProgram(programObject_Blur);
		return 0;
	}

	// Store the program object
	GprogramID_Blur = programObject_Blur;

	// !! Init Blend shader !! //

	vertexShader_Blend = LoadShaderFromFile(GL_VERTEX_SHADER, "../default.vert");
	fragmentShader_Blend = LoadShaderFromFile(GL_FRAGMENT_SHADER, "../blend.frag");

	// Create the program object
	programObject_Blend = glCreateProgram();

	if (programObject_Blend == 0)
		return 0;

	glAttachShader(programObject_Blend, vertexShader_Blend);
	glAttachShader(programObject_Blend, fragmentShader_Blend);

	glBindAttribLocation(programObject_Blend, 0, "vPosition");
	glBindAttribLocation(programObject_Blend, 1, "vColor");
	glBindAttribLocation(programObject_Blend, 2, "vTexCoord");

	// Link the program
	glLinkProgram(programObject_Blend);

	// Check the link status
	glGetProgramiv(programObject_Blend, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(programObject_Blend, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char infoLog[1024];
			glGetProgramInfoLog(programObject_Blend, infoLen, NULL, infoLog);
			printf("Error linking program:\n%s\n", infoLog);
		}

		glDeleteProgram(programObject_Blend);
		return 0;
	}

	// Store the program object
	GprogramID_Blend = programObject_Blend;

	// !! Init Depth shader program !! //

	vertexShader_Depth = LoadShaderFromFile(GL_VERTEX_SHADER, "../default.vert");
	fragmentShader_Depth = LoadShaderFromFile(GL_FRAGMENT_SHADER, "../depth.frag");

	// Create the program object
	programObject_Depth = glCreateProgram();

	if (programObject_Depth == 0)
		return 0;

	glAttachShader(programObject_Depth, vertexShader_Depth);
	glAttachShader(programObject_Depth, fragmentShader_Depth);

	glBindAttribLocation(programObject_Depth, 0, "vPosition");
	glBindAttribLocation(programObject_Depth, 1, "vColor");
	glBindAttribLocation(programObject_Depth, 2, "vTexCoord");

	// Link the program
	glLinkProgram(programObject_Depth);

	// Check the link status
	glGetProgramiv(programObject_Depth, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(programObject_Depth, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char infoLog[1024];
			glGetProgramInfoLog(programObject_Depth, infoLen, NULL, infoLog);
			printf("Error linking program:\n%s\n", infoLog);
		}

		glDeleteProgram(programObject_Depth);
		return 0;
	}

	// Store the program object
	GprogramID_Depth = programObject_Depth;

	// ---------------- //

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//initialize matrices
	gPerspectiveMatrix = Matrix4::perspective(60.0f,
		(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
		0.5f, 400.0f);
	// gOthorMatrix = Matrix4::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.5f, 30.0f);

	gViewMatrix = Matrix4::translate(Vector3(0.0f, 0.0f, -15.0f));


	return 1;
}


void UpdateCamera(void)
{
	if (glfwGetKey(window, 'A')) cameraPitch -= 1.0f;
	if (glfwGetKey(window, 'D')) cameraPitch += 1.0f;
	if (glfwGetKey(window, 'W')) cameraYaw -= 1.0f;
	if (glfwGetKey(window, 'S')) cameraYaw += 1.0f;

	if (glfwGetKey(window, 'R'))
	{
		cameraDistance -= 0.06f;
		if (cameraDistance < 1.0f)
			cameraDistance = 1.0f;
	}
	if (glfwGetKey(window, 'F')) cameraDistance += 0.06f;

	gViewMatrix = Matrix4::translate(Vector3(0.0f, 0.0f, -cameraDistance)) *
		Matrix4::rotate(cameraYaw, Vector3(1.0f, 0.0f, 0.0f)) *
		Matrix4::rotate(cameraPitch, Vector3(0.0f, 1.0f, 0.0f));
}

Matrix4 getViewMatrixWithoutTranslate(void)
{
	return Matrix4::rotate(cameraYaw, Vector3(1.0f, 0.0f, 0.0f)) * Matrix4::rotate(cameraPitch, Vector3(0.0f, 1.0f, 0.0f));
}

void DrawSquare(GLuint texture)
{
	static GLfloat vVertices[] = { -1.0f,  1.0f, 0.0f,
								-1.0f, -1.0f, 0.0f,
								1.0f, -1.0f,  0.0f,
								1.0f,  -1.0f, 0.0f,
								1.0f, 1.0f, 0.0f,
								-1.0f, 1.0f,  0.0f };


	static GLfloat vColors[] = { 1.0f,  0.0f, 0.0f, 1.0f,
								 0.0f, 1.0f, 0.0f, 1.0f,
								 0.0f, 0.0f,  1.0f, 1.0f,
								 0.0f,  0.0f, 1.0f, 1.0f,
								 1.0f, 1.0f, 0.0f, 1.0f,
								 1.0f, 0.0f,  0.0f, 1.0f };

	static GLfloat vTexCoords[] = { 0.0f,  1.0f,
									 0.0f, 0.0f,
									 1.0f, 0.0f,
									 1.0f,  0.0f,
									 1.0f, 1.0f,
									 0.0f, 1.0f };


	glBindTexture(GL_TEXTURE_2D, texture);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, vColors);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, vTexCoords);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);


	glDrawArrays(GL_TRIANGLES, 0, 6);


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void Draw(void)
{

	UpdateCamera();

	GLenum status;

	//======================= bind the framebuffer and render BG to texture

	glBindFramebuffer(GL_FRAMEBUFFER, Gframebuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GtextureBG, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Gdepthbuffer, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {

		printf("(0) frame buffer is not ready!\n");

		if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) printf("Error GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
		else if (status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS) printf("Error GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n");
		else if (status == GL_FRAMEBUFFER_UNSUPPORTED) printf("Error GL_FRAMEBUFFER_UNSUPPORTED\n");

		return;

	}

	Matrix4 modelMatrix, mvpMatrix;

	// !! Set the viewport !! //

	glViewport(0, 0, GtexBGWidth, GtexBGHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// !! Skybox !! //

	glUseProgram(GprogramID_SkyBox);

	glUniform1i(glGetUniformLocation(GprogramID_SkyBox, "samplerCube1"), 0);

	modelMatrix = Matrix4::scale(Vector3(50.0f, 50.0f, 50.0f));

	mvpMatrix = gPerspectiveMatrix * getViewMatrixWithoutTranslate() * modelMatrix;
	//mvpMatrix = gPerspectiveMatrix * gViewMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(GprogramID_SkyBox, "uMvpMatrix"), 1, GL_FALSE, mvpMatrix.data);

	drawSkyBox();

	// !! First object !! //

	// change the render target to GtextureBlurred
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Gdepthtexture, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {

		printf("(0) frame buffer is not ready!\n");

		if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) printf("Error GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
		else if (status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS) printf("Error GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n");
		else if (status == GL_FRAMEBUFFER_UNSUPPORTED) printf("Error GL_FRAMEBUFFER_UNSUPPORTED\n");

		return;

	}

	glDepthFunc(GL_ALWAYS);

	glUseProgram(GprogramID_Depth);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, Gdepthbuffer);

	glUniform1i(glGetUniformLocation(GprogramID_Depth, "bgDepthTexture"), 1);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, GtextureID[0]);

	// set the sampler2D varying variable to the first texture unit(index 0)
	glUniform1i(glGetUniformLocation(GprogramID_Depth, "sampler2d"), 0);

	//========pass texture size to shader
	glUniform1f(glGetUniformLocation(GprogramID_Depth, "windowW"), (GLfloat)WINDOW_WIDTH);
	glUniform1f(glGetUniformLocation(GprogramID_Depth, "windowH"), (GLfloat)WINDOW_HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);;

	modelMatrix = Matrix4::translate(Vector3(0.0f, 0.0f, -3.0f)) *
		Matrix4::scale(Vector3(4.0f, 4.0f, 4.0f)) *
		Matrix4::rotate(0, Vector3(0.0f, 1.0f, 0.0f));

	mvpMatrix = gPerspectiveMatrix * gViewMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(GprogramID_Depth, "uMvpMatrix"), 1, GL_FALSE, mvpMatrix.data);

	DrawSquare(GtextureID[0]);

	// !! second object !! //

	modelMatrix = Matrix4::translate(Vector3(-4.0f, 0.0f, 4.0f)) *
		Matrix4::scale(Vector3(3.0f, 3.0f, 3.0f)) *
		Matrix4::rotate(0, Vector3(0.0f, 1.0f, 0.0f));

	mvpMatrix = gPerspectiveMatrix * gViewMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(GprogramID_Depth, "uMvpMatrix"), 1, GL_FALSE, mvpMatrix.data);

	DrawSquare(GtextureID[1]);

	glDepthFunc(GL_LESS);

	// !! Apply depth !! //


	// !! bloom !! //

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Gbloomtexture, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {

		printf("(0) frame buffer is not ready!\n");

		if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) printf("Error GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
		else if (status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS) printf("Error GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n");
		else if (status == GL_FRAMEBUFFER_UNSUPPORTED) printf("Error GL_FRAMEBUFFER_UNSUPPORTED\n");

		return;

	}

	glUseProgram(GprogramID_Bloom);

	// set the sampler2D varying variable to the first texture unit(index 0)
	glUniform1i(glGetUniformLocation(GprogramID_Bloom, "sampler2d"), 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset the mvpMatrix to identity matrix so that it renders fully on texture in normalized device coordinates
	glUniformMatrix4fv(glGetUniformLocation(GprogramID_Bloom, "uMvpMatrix"), 1, GL_FALSE, Matrix4::identity().data);

	DrawSquare(Gdepthtexture);

	// !! blur the texture, first pass(horizontal blur) !! //

	// change the render target to GtextureBlurred
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Gblurpass1texture, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {

		printf("(0) frame buffer is not ready!\n");

		if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) printf("Error GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
		else if (status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS) printf("Error GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n");
		else if (status == GL_FRAMEBUFFER_UNSUPPORTED) printf("Error GL_FRAMEBUFFER_UNSUPPORTED\n");

		return;

	}

	glUseProgram(GprogramID_Blur);

	// set the sampler2D varying variable to the first texture unit(index 0)
	glUniform1i(glGetUniformLocation(GprogramID_Blur, "sampler2d"), 0);

	//========pass texture size to shader
	glUniform1f(glGetUniformLocation(GprogramID_Blur, "uTextureW"), (GLfloat)WINDOW_WIDTH);
	glUniform1f(glGetUniformLocation(GprogramID_Blur, "uTextureH"), (GLfloat)WINDOW_HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset the mvpMatrix to identity matrix so that it renders fully on texture in normalized device coordinates
	glUniformMatrix4fv(glGetUniformLocation(GprogramID_Blur, "uMvpMatrix"), 1, GL_FALSE, Matrix4::identity().data);

	// tell the shader to apply horizontal blurring, for details please check the "uBlurDirection" flag in the shader code
	glUniform1i(glGetUniformLocation(GprogramID_Blur, "uBlurDirection"), 0);

	DrawSquare(Gbloomtexture);

	// change the render target to GtextureBlurred
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Gblurpass2texture, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {

		printf("(0) frame buffer is not ready!\n");

		if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) printf("Error GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
		else if (status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS) printf("Error GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n");
		else if (status == GL_FRAMEBUFFER_UNSUPPORTED) printf("Error GL_FRAMEBUFFER_UNSUPPORTED\n");

		return;

	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset the mvpMatrix to identity matrix so that it renders fully on texture in normalized device coordinates
	glUniformMatrix4fv(glGetUniformLocation(GprogramID_Blur, "uMvpMatrix"), 1, GL_FALSE, Matrix4::identity().data);

	// draw the texture that has been horizontally blurred, and apply vertical blurring
	glUniform1i(glGetUniformLocation(GprogramID_Blur, "uBlurDirection"), 1);

	DrawSquare(Gblurpass1texture);

	// !! additive blend !! //

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Gblendtexture, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {

		printf("(0) frame buffer is not ready!\n");

		if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) printf("Error GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
		else if (status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS) printf("Error GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n");
		else if (status == GL_FRAMEBUFFER_UNSUPPORTED) printf("Error GL_FRAMEBUFFER_UNSUPPORTED\n");

		return;

	}

	glUseProgram(GprogramID_Blend);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset the mvpMatrix to identity matrix so that it renders fully on texture in normalized device coordinates
	glUniformMatrix4fv(glGetUniformLocation(GprogramID_Blend, "uMvpMatrix"), 1, GL_FALSE, Matrix4::identity().data);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, GtextureBG);

	glUniform1i(glGetUniformLocation(GprogramID_Blend, "toBlend"), 1);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, GprogramID_Blend);

	// set the sampler2D varying variable to the first texture unit(index 0)
	glUniform1i(glGetUniformLocation(GprogramID_Blend, "sampler2d"), 0);

	DrawSquare(Gblurpass2texture);

	// !! Render !! //

	// this time, render directly to window system framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(GprogramID_Default);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset the mvpMatrix to identity matrix so that it renders fully on texture in normalized device coordinates
	glUniformMatrix4fv(glGetUniformLocation(GprogramID_Default, "uMvpMatrix"), 1, GL_FALSE, Matrix4::identity().data);

	// set the sampler2D varying variable to the first texture unit(index 0)
	glUniform1i(glGetUniformLocation(GprogramID_Default, "sampler2d"), 0);

	DrawSquare(Gblendtexture);

}

int main(void)
{
	glfwSetErrorCallback(error_callback);


	// Initialize GLFW library
	if (!glfwInit())
		return -1;

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Create and open a window
	window = glfwCreateWindow(WINDOW_WIDTH,
		WINDOW_HEIGHT,
		"Hello World",
		NULL,
		NULL);

	if (!window)
	{
		glfwTerminate();
		printf("glfwCreateWindow Error\n");
		exit(1);
	}

	glfwMakeContextCurrent(window);

	Init();

	// Repeat
	while (!glfwWindowShouldClose(window))
	{
		Draw();
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE))
			break;
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
