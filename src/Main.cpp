#include <iostream>
#include <sstream>

#define GLEW_STATIC
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include "ShaderProgram.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Texture2D.h"

const char* APP_TITLE = "Modern OpenGL";
int WINDOW_WIDTH = 1024;
int WINDOW_HEIGHT = 768;
bool fullscreen = false;
bool wireframe = false;
GLFWwindow* window = nullptr;

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFrameBufferSize(GLFWwindow* window, int width, int height);
void showFPS(GLFWwindow* window);
bool initOpenGL();

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

	Texture2D airplaneTex;
	airplaneTex.Load("textures/crate.jpg", true);

	//Texture2D crateTex;
	//crateTex.Load("textures/crate.jpg", true);

	float cubeAngle = 0.0f;
	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		showFPS(window);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		airplaneTex.Bind(0);
		//crateTex.Bind(1);

		cubeAngle += (float)(deltaTime * 50.0f);
		if (cubeAngle >= 360.0) cubeAngle = 0.0f;

		glm::mat4 model, view, projection;
		model = glm::translate(model, cubePos) * glm::rotate(model, glm::radians(cubeAngle), glm::vec3(0.f, 1.f, 0.f));

		glm::vec3 camPos{ 0.f, 0.f, 0.f };
		glm::vec3 targetPos{ 0.f, 0.f, -1.f };
		glm::vec3 up{ 0.f, 1.f, 0.f };
		view = glm::lookAt(camPos, targetPos, up);

		projection = glm::perspective(glm::radians(45.f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.f);
		
		shaderProgram.Use();

		shaderProgram.SetUniform("model", model);
		shaderProgram.SetUniform("view", view);
		shaderProgram.SetUniform("projection", projection);

		glBindVertexArray(vao);
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
		window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APP_TITLE, NULL, NULL);
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

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void glfw_onFrameBufferSize(GLFWwindow* window, int width, int height)
{
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}