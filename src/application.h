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
#include "physics/simulator.h"

constexpr float DELTA_TIME = 1.0f;

constexpr uint32_t SCR_WIDTH = 800;
constexpr uint32_t SCR_HEIGHT = 800;

inline std::shared_ptr<Camera> g_ActiveCamera = nullptr;

inline float g_LastX = SCR_WIDTH / 2.0f;
inline float g_LastY = SCR_HEIGHT / 2.0f;
inline bool g_IsFirstMouse = true;

inline bool g_IsMouseDisabled = true;

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
	uint32_t m_WindowWidth = SCR_WIDTH;
	uint32_t m_WindowHeight = SCR_HEIGHT;
	std::unique_ptr<Renderer> m_Renderer = nullptr;
	std::unique_ptr<Scene> m_Scene = nullptr;
	std::unique_ptr<Simulator> m_Simulator = nullptr;

private:
	void Init();

	void OnFrameStart();

	void OnFrameEnd();

	void SetImGuiWindows() const;

	void Render();

	void Simulate();

	bool WindowIsOpen() { return !glfwWindowShouldClose(m_Window); }

	void CleanUp() const { glfwTerminate(); }
};

// Application Callbacks:

// Sets the size of the OpenGL Viewport
inline void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

inline void MouseCallback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	const auto xPos = static_cast<float>(xPosIn);
	const auto yPos = static_cast<float>(yPosIn);

	if (g_IsFirstMouse)
	{
		g_LastX = xPos;
		g_LastY = yPos;
		g_IsFirstMouse = false;
	}

	const float xOffset = xPos - g_LastX;
	const float yOffset = g_LastY - yPos; // reversed since y-coordinates go from bottom to top

	g_LastX = xPos;
	g_LastY = yPos;

	if (g_ActiveCamera && g_IsMouseDisabled)
		g_ActiveCamera->ProcessMouseMovement(xOffset, yOffset);
}

inline void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	if (g_ActiveCamera)
		g_ActiveCamera->ProcessMouseScroll(static_cast<float>(yOffset));
}

inline void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, g_IsMouseDisabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		g_IsMouseDisabled = !g_IsMouseDisabled;
	}

	if (!g_ActiveCamera)
		return;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		g_ActiveCamera->ProcessKeyboard(FORWARD, DELTA_TIME);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		g_ActiveCamera->ProcessKeyboard(BACKWARD, DELTA_TIME);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		g_ActiveCamera->ProcessKeyboard(LEFT, DELTA_TIME);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		g_ActiveCamera->ProcessKeyboard(RIGHT, DELTA_TIME);
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		g_ActiveCamera->ProcessKeyboard(TOGGLE_FLY, DELTA_TIME);
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
