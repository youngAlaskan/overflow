#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <exception>
#include <iostream>

#include "rendering/renderer.h"
#include "scene/scene.h"

constexpr float DELTA_TIME = 1.0f;

// Handles all GLFW and glad window management
class Application
{
public:
	Application() { Init(); }
	Application(uint32_t width, uint32_t height)
		: m_WindowWidth(width), m_WindowHeight(height) { Init(); }
	~Application() { CleanUp(); }

	void Run();

public:
	GLFWwindow* m_Window = nullptr;
	uint32_t m_WindowWidth = 400;
	uint32_t m_WindowHeight = 400;
	std::unique_ptr<Renderer> m_Renderer = nullptr;
	std::unique_ptr<Scene> m_Scene = nullptr;

private:
	void Init();

	void OnFrameStart();

	void OnFrameEnd();

	bool WindowIsOpen() { return !glfwWindowShouldClose(m_Window); }

	void CleanUp() const { glfwTerminate(); }
};

// Application Callbacks:

// Sets the size of the OpenGL Viewport
inline void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

inline void processInput(GLFWwindow* window, Camera& camera)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, DELTA_TIME);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, DELTA_TIME);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, DELTA_TIME);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, DELTA_TIME);
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		camera.ProcessKeyboard(TOGGLE_FLY, DELTA_TIME);
}

// Exceptions

inline void PrintAndThrowException(const std::exception& e)
{
	std::cerr << e.what() << std::endl;
	throw e;
}

// The function glfwCreateWindow failed (returned nullptr)
class GLFWWindowCreationException : public std::exception {
public:
	char* what()
	{
		return "Failed to create GLFW window";
	}
};

// The function gladLoadGLLoader failed (returned 0)
class GLADLoaderException : public std::exception {
public:
	char* what()
	{
		return "Failed to initialize GLAD";
	}
};
