#include <iostream>
#include <sstream>

#define GLEW_STATIC
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"

const char* APP_TITLE = "Modern OpenGL";
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

bool fullscreen = false;

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void showFPS(GLFWwindow* window);

int main()
{
	if (!glfwInit())
	{
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = nullptr;
	
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
		return -1;
	}

	glfwMakeContextCurrent(window);
	
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW initialization failed" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, glfw_onKey);

	while (!glfwWindowShouldClose(window))
	{
		showFPS(window);
		glfwPollEvents();

		glClearColor(0.23f, 0.38f, 0.47f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}
	
	
	glfwTerminate();

	return 0;
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
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