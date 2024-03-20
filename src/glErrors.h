#pragma once

#include <glad/glad.h>
#include <iostream>
#include <unordered_map>

#define OPENGL_DEBUGGING

inline std::unordered_map<GLenum, std::string> errorMessages
{
	{ GL_INVALID_ENUM, "INVALID_ENUM, 0x0500" },
	{ GL_INVALID_VALUE, "INVALID_VALUE, 0x0501" },
	{ GL_INVALID_OPERATION, "INVALID_OPERATION, 0x0502" },
	{ GL_STACK_OVERFLOW, "STACK_OVERFLOW, 0x0503" },
	{ GL_STACK_UNDERFLOW, "STACK_UNDERFLOW, 0x0504" },
	{ GL_OUT_OF_MEMORY, "OUT_OF_MEMORY, 0x0505" },
	{ GL_INVALID_FRAMEBUFFER_OPERATION, "INVALID_FRAMEBUFFER_OPERATION, 0x0506" },
	{ GL_CONTEXT_LOST, "CONTEXT_LOST, 0x0507"}
};

inline std::unordered_map<GLenum, std::string> errorSeverity
{
	{ GL_DEBUG_SEVERITY_HIGH, "HIGH_SEVERITY (0x9146)" },
	{ GL_DEBUG_SEVERITY_MEDIUM, "MEDIUM_SEVERITY (0x9147)" },
	{ GL_DEBUG_SEVERITY_LOW, "HIGH_SEVERITY (0x9148)" },
	{ GL_DEBUG_SEVERITY_NOTIFICATION, "HIGH_SEVERITY (0x826B)" },
};

inline std::unordered_map<GLenum, std::string> messageType
{
	{ GL_DEBUG_TYPE_ERROR, "ERROR (0x824C)" },
	{ GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR (0x824D)" },
	{ GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED_BEHAVIOR (0x824E)" },
	{ GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY (0x824F)" },
	{ GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE (0x8250)" },
	{ GL_DEBUG_TYPE_MARKER, "MARKER (0x8268)" },
	{ GL_DEBUG_TYPE_PUSH_GROUP, "PUSH_GROUP (0x8269)" },
	{ GL_DEBUG_TYPE_POP_GROUP, "POP_GROUP (0x826A)" },
	{ GL_DEBUG_TYPE_OTHER, "OTHER (0x8251)" }
};

inline void CheckForErrors(const char* message)
{
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR)
		std::cerr << message << errorMessages[error] << std::endl;
}

inline void GLAPIENTRY MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	std::cerr
		<< "GL MESSAGE CALLBACK: "
		<< messageType[type] << " | "
		<< errorSeverity[severity] << std::endl
		<< "  MESSAGE: " << message << std::endl;
}
