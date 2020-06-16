#include <iostream>
#include <sstream>

#define GLEW_STATIC
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Mesh.h"

const char* APP_TITLE = "Modern OpenGL";
int windowWidth = 1024;
int windowHeight = 768;
bool fullscreen = false;
bool wireframe = false;
GLFWwindow* window = nullptr;

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFrameBufferSize(GLFWwindow* window, int width, int height);
void glfw_onMouseMove(GLFWwindow* window, double posX, double posY);
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY);

void showFPS(GLFWwindow* window);
bool initOpenGL();
void Update(double elapsedTime);

FPSCamera fpsCamera(glm::vec3(0.f, 3.f, 50.f));
const double ZOOM_SENSITIVITY = -3.f;
const float MOVE_SPEED = 15.f;
const float MOUSE_SENSITIVITY = 0.1f;

float angle = 0.f;

int main()
{
	if (!initOpenGL())
	{
		std::cerr << "GLFW initialization failed" << std::endl;
		return 1;
	}
	
	ShaderProgram lightShader;
	lightShader.LoadShaders("shaders/basic.vert", "shaders/basic.frag");

	ShaderProgram lightingShader;
	lightingShader.LoadShaders("shaders/lighting.vert", "shaders/lighting.frag");

	glm::vec3 modelPos[] = {
		glm::vec3(-3.5f, 0.f,  0.f),	// crate1
		glm::vec3( 3.5f, 0.f,  0.f),	// crate2
		glm::vec3( 0.0f, 0.f, -2.f),	// robot
		glm::vec3( 0.0f, 0.f,  0.f),	// floor
		glm::vec3( 0.0f, 0.f,  2.f),	// pin
		glm::vec3(-2.0f, 0.f,  2.f)		// bunny
	};

	glm::vec3 modelScale[] = {
		glm::vec3( 1.0f, 1.0f,  1.0f),	// crate1
		glm::vec3( 1.0f, 1.0f,  1.0f),	// crate2
		glm::vec3( 1.0f, 1.0f,  1.0f),	// robot
		glm::vec3(10.0f, 1.0f, 10.0f),	// floor
		glm::vec3( 0.1f, 0.1f,  0.1f),	// pin
		glm::vec3( 0.7f, 0.7f,  0.7f)	// bunny
	};

	const int numModels = 6;
	Mesh mesh[numModels];
	Texture2D texture[numModels];

	mesh[0].LoadOBJ("models/crate.obj");
	mesh[1].LoadOBJ("models/woodcrate.obj");
	mesh[2].LoadOBJ("models/robot.obj");
	mesh[3].LoadOBJ("models/floor.obj");
	mesh[4].LoadOBJ("models/bowling_pin.obj");
	mesh[5].LoadOBJ("models/bunny.obj");

	texture[0].Load("textures/crate.jpg");
	texture[1].Load("textures/woodcrate_diffuse.jpg");
	texture[2].Load("textures/robot_diffuse.jpg");
	texture[3].Load("textures/tile_floor.jpg");
	texture[4].Load("textures/AMF.tga");
	texture[5].Load("textures/bunny_diffuse.jpg");

	Mesh lightMesh;
	lightMesh.LoadOBJ("models/light.obj");

	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		showFPS(window);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		glfwPollEvents();
		Update(deltaTime);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model, view, projection;

		view = fpsCamera.GetViewMatrix();
		projection = glm::perspective(glm::radians(fpsCamera.GetFOV()), (float)windowWidth / (float)windowHeight, 0.1f, 100.f);
		
		glm::vec3 viewPos;
		viewPos = fpsCamera.GetPosition();

		glm::vec3 lightPos(0.0f, 1.0f, 10.0f);
		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

		angle += (float)deltaTime * 50.f;
		lightPos.x = 8.f * sinf(glm::radians(angle));

		lightingShader.Use();

		lightingShader.SetUniform("view", view);
		lightingShader.SetUniform("projection", projection);
		lightingShader.SetUniform("viewPos", viewPos);
		
		lightingShader.SetUniform("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		lightingShader.SetUniform("light.diffuse", lightColor);
		lightingShader.SetUniform("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		lightingShader.SetUniform("light.position", lightPos);

		for (int i = 0; i < numModels; ++i)
		{
			model = glm::translate(glm::mat4(), modelPos[i]) * glm::scale(glm::mat4(), modelScale[i]);
			lightingShader.SetUniform("model", model);

			lightingShader.SetUniform("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
			lightingShader.SetUniformSampler("material.diffuseMap", 0);
			lightingShader.SetUniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
			lightingShader.SetUniform("material.shininess", 32.f);

			texture[i].Bind(0);
			mesh[i].Draw();
			texture[i].Unbind(0);
		}

		model = glm::translate(glm::mat4(), lightPos);
		lightShader.Use();
		lightShader.SetUniform("lightColor", lightColor);
		lightShader.SetUniform("model", model);
		lightShader.SetUniform("view", view);
		lightShader.SetUniform("projection", projection);
		lightMesh.Draw();

		glfwSwapBuffers(window);

		lastTime = currentTime;
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

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
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
	glfwSetScrollCallback(window, glfw_onMouseScroll);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, (windowWidth / 2.f), (windowHeight / 2.f));

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
	//static glm::vec2 lastMousePos = glm::vec2{ 0, 0 };

	//if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == 1)
	//{
	//	yaw -= ((float)posX - lastMousePos.x) * MOUSE_SENSITIVITY;
	//	pitch += ((float)posY - lastMousePos.y) * MOUSE_SENSITIVITY;
	//}
	//
	//if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == 1)
	//{
	//	float dx = 0.01f * ((float)posX - lastMousePos.x);
	//	float dy = 0.01f * ((float)posY - lastMousePos.y);
	//	radius += dx - dy;
	//}

	//lastMousePos.x = (float)posX;
	//lastMousePos.t = (float)posY;
}

void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY)
{
	double fov = fpsCamera.GetFOV() + deltaY * ZOOM_SENSITIVITY;
	fov = glm::clamp(fov, 1.0, 120.0);
	fpsCamera.SetFOV((float)fov);
}

void Update(double elapsedTime)
{
	double mouseX, mouseY;

	glfwGetCursorPos(window, &mouseX, &mouseY);
	fpsCamera.Rotate((float)(windowWidth / 2.0 - mouseX) * MOUSE_SENSITIVITY, (float)(windowHeight / 2.0 - mouseY) * MOUSE_SENSITIVITY);
	glfwSetCursorPos(window, windowWidth / 2.0, windowHeight / 2.0);

	// Forward | Backward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		fpsCamera.Move(MOVE_SPEED * (float)elapsedTime * fpsCamera.GetLook());
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		fpsCamera.Move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.GetLook());

	// Strafe left | right
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		fpsCamera.Move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.GetRight());
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		fpsCamera.Move(MOVE_SPEED * (float)elapsedTime * fpsCamera.GetRight());

	// Up | Down
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		fpsCamera.Move(MOVE_SPEED * (float)elapsedTime * fpsCamera.GetUp());
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		fpsCamera.Move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.GetUp());
}