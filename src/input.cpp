#include "input.h"

struct InputKeyActions input_key_actions;

constexpr int MOVE_LEFT		= GLFW_KEY_A;
constexpr int MOVE_RIGHT	= GLFW_KEY_D;
constexpr int MOVE_UP		= GLFW_KEY_W;
constexpr int MOVE_DOWN		= GLFW_KEY_S;

constexpr int TOGGLE_MOUSE	= GLFW_MOUSE_BUTTON_2;
constexpr int TOGGLE_FLY	= GLFW_KEY_V;

constexpr int CLOSE_WINDOW = GLFW_KEY_ESCAPE;

bool g_WasToggleFlyJustPressed = false;
bool g_WasToggleMouseJustPressed = false;

InputKeyActions PollKeyActions(GLFWwindow* window) {
	bool isToggleFlyPressed = glfwGetKey(window, TOGGLE_FLY) == GLFW_PRESS;
	bool isToggleMousePressed = glfwGetMouseButton(window, TOGGLE_MOUSE) == GLFW_PRESS;

	InputKeyActions keyActions = {
		glm::vec2(
		float(glfwGetKey(window, MOVE_RIGHT) == GLFW_PRESS) - float(glfwGetKey(window, MOVE_LEFT) == GLFW_PRESS),
		float(glfwGetKey(window, MOVE_UP) == GLFW_PRESS) - float(glfwGetKey(window, MOVE_DOWN) == GLFW_PRESS)
		),
		g_WasToggleFlyJustPressed && !isToggleFlyPressed,
		g_WasToggleMouseJustPressed && !isToggleMousePressed,
		glfwGetKey(window, CLOSE_WINDOW) == GLFW_PRESS,
	};

	g_WasToggleFlyJustPressed = isToggleFlyPressed;
	g_WasToggleMouseJustPressed = isToggleMousePressed;

	return keyActions;
}