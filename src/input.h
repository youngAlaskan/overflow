#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

extern struct InputKeyActions {
	glm::vec2 move = glm::vec2(0.f);
	bool toggleFly = false;
	bool toggleMouse = false;
	bool closeWindow = false;
};

InputKeyActions PollKeyActions(GLFWwindow* window);