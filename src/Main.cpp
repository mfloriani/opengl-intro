#include <iostream>
#include <sstream>

#define GLEW_STATIC
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include "ShaderProgram.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Texture2D.h"
#include "Camera.h"

const char* APP_TITLE = "Modern OpenGL";
int windowWidth = 1024;
int windowHeight = 768;
bool fullscreen = false;
bool wireframe = false;
GLFWwindow* window = nullptr;

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFrameBufferSize(GLFWwindow* window, int width, int height);
void glfw_onMouseMove(GLFWwindow* window, double posX, double posY);

void showFPS(GLFWwindow* window);
bool initOpenGL();

OrbitCamera orbitCamera;
float yaw = 0.f;
float pitch = 0.f;
float radius = 10.f;

const float MOUSE_SENSITIVITY = 0.25f;

int main()
{
	if (!initOpenGL())
	{
		std::cerr << "GLFW initialization failed" << std::endl;
		return 1;
	}

	GLfloat vertices[] = {
		// position         //tex coords

		// front face
		-1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

		 // back face
		 -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		  1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		  1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
		 -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		 -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		  1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

		  // left face
		  -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		  -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
		  -1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
		  -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		  -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		  -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

		  // right face
		   1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		   1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		   1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
		   1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		   1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
		   1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

		   // top face
		  -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		   1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
		   1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
		  -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		  -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
		   1.0f,  1.0f,  1.0f, 1.0f, 0.0f,

		   // bottom face
		  -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
		   1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		   1.0f, -1.0f,  1.0f, 1.0f, 1.0f,
		  -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
		  -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		   1.0f, -1.0f, -1.0f, 1.0f, 0.0f
	};

	glm::vec3 cubePos = glm::vec3(0.0f, 0.0f, -5.f);

	GLuint vbo, vao;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);

	//tex coord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLfloat*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	
	ShaderProgram shaderProgram;
	shaderProgram.LoadShaders("shaders/basic.vert", "shaders/basic.frag");

	Texture2D woodenCrateTex;
	woodenCrateTex.Load("textures/wooden_crate.jpg", true);

	Texture2D crateTex;
	crateTex.Load("textures/grid.jpg", true);

	float cubeAngle = 0.0f;
	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		showFPS(window);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		woodenCrateTex.Bind(0);
		
		glm::mat4 model, view, projection;

		orbitCamera.SetLookAt(cubePos);
		orbitCamera.Rotate(yaw, pitch);
		orbitCamera.SetRadius(radius);

		model = glm::translate(model, cubePos);
		
		view = orbitCamera.GetViewMatrix();

		projection = glm::perspective(glm::radians(45.f), (float)windowWidth / (float)windowHeight, 0.1f, 100.f);
		
		shaderProgram.Use();

		shaderProgram.SetUniform("model", model);
		shaderProgram.SetUniform("view", view);
		shaderProgram.SetUniform("projection", projection);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		crateTex.Bind(0);

		glm::vec3 floorPos;
		floorPos.y = -1.f;
		model = glm::translate(model, floorPos) * glm::scale(model, glm::vec3(10.f, 0.01f, 10.f));
		shaderProgram.SetUniform("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		glfwSwapBuffers(window);

		lastTime = currentTime;
	}
	
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	glfwTerminate();

	return 0;
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		wireframe = !wireframe;
		if (wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
}

void showFPS(GLFWwindow* window)
{
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime();

	elapsedSeconds = currentSeconds - previousSeconds;

	if (elapsedSeconds > 0.25)
	{
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		std::ostringstream outs;
		outs.precision(3);
		outs << std::fixed << APP_TITLE << " FPS: " << fps << " Frame Time: " << msPerFrame << " (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());
		frameCount = 0;
	}
	frameCount++;
}

bool initOpenGL()
{
	if (!glfwInit())
	{
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	if (fullscreen)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vidMode = glfwGetVideoMode(monitor);
		if (vidMode)
		{
			window = glfwCreateWindow(vidMode->width, vidMode->height, APP_TITLE, monitor, NULL);
		}
	}
	else
	{
		window = glfwCreateWindow(windowWidth, windowHeight, APP_TITLE, NULL, NULL);
	}

	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW initialization failed" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwSetKeyCallback(window, glfw_onKey);
	glfwSetCursorPosCallback(window, glfw_onMouseMove);

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);
	glViewport(0, 0, windowWidth, windowHeight);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void glfw_onFrameBufferSize(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, windowWidth, windowHeight);
}

void glfw_onMouseMove(GLFWwindow* window, double posX, double posY)
{
	static glm::vec2 lastMousePos = glm::vec2{ 0, 0 };

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == 1)
	{
		yaw -= ((float)posX - lastMousePos.x) * MOUSE_SENSITIVITY;
		pitch += ((float)posY - lastMousePos.y) * MOUSE_SENSITIVITY;
	}
	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == 1)
	{
		float dx = 0.01f * ((float)posX - lastMousePos.x);
		float dy = 0.01f * ((float)posY - lastMousePos.y);
		radius += dx - dy;
	}

	lastMousePos.x = (float)posX;
	lastMousePos.t = (float)posY;
}