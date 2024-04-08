#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

inline constexpr int MOVE_LEFT = GLFW_KEY_A;
inline constexpr int MOVE_RIGHT = GLFW_KEY_D;
inline constexpr int MOVE_FORWARD = GLFW_KEY_W;
inline constexpr int MOVE_BACK = GLFW_KEY_S;
inline constexpr int MOVE_UP = GLFW_KEY_SPACE;
inline constexpr int MOVE_DOWN = GLFW_KEY_LEFT_SHIFT;

inline constexpr int TOGGLE_FLY = GLFW_KEY_V;
inline constexpr int TOGGLE_MOUSE = GLFW_KEY_TAB;

inline constexpr int CLOSE_WINDOW = GLFW_KEY_ESCAPE;

extern struct InputKeyActions {
	glm::vec3 move = glm::vec3(0.f);
	bool toggleFly = false;
	bool toggleMouse = false;
	bool closeWindow = false;
};

InputKeyActions PollKeyActions(GLFWwindow* window);