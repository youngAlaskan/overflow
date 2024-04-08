#include "input.h"

struct InputKeyActions input_key_actions;

bool g_WasToggleFlyJustPressed = false;
bool g_WasToggleMouseJustPressed = false;
bool g_WasCloseJustPressed = false;

InputKeyActions PollKeyActions(GLFWwindow* window) {
	bool isToggleFlyPressed = glfwGetKey(window, TOGGLE_FLY) == GLFW_PRESS;
	bool isToggleMousePressed = glfwGetKey(window, TOGGLE_MOUSE) == GLFW_PRESS;
	bool isClosePressed = glfwGetKey(window, CLOSE_WINDOW) == GLFW_PRESS;

	InputKeyActions keyActions = {
		glm::vec3(
		float(glfwGetKey(window, MOVE_RIGHT) == GLFW_PRESS) - float(glfwGetKey(window, MOVE_LEFT) == GLFW_PRESS),
		float(glfwGetKey(window, MOVE_UP) == GLFW_PRESS) - float(glfwGetKey(window, MOVE_DOWN) == GLFW_PRESS),
		float(glfwGetKey(window, MOVE_FORWARD) == GLFW_PRESS) - float(glfwGetKey(window, MOVE_BACK) == GLFW_PRESS)
		),
		g_WasToggleFlyJustPressed && !isToggleFlyPressed,
		g_WasToggleMouseJustPressed && !isToggleMousePressed,
		g_WasCloseJustPressed && !isClosePressed,
	};

	g_WasToggleFlyJustPressed = isToggleFlyPressed;
	g_WasToggleMouseJustPressed = isToggleMousePressed;
	g_WasCloseJustPressed = isClosePressed;

	return keyActions;
}