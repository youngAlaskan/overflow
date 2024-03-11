#pragma once

#include <glm\glm.hpp>
#include <glad/glad.h>

struct Vertex
{
	glm::vec4 Position; // Position in model space 
	glm::vec3 Normal; // Outward facing normal
	glm::vec2 TexCoord; // Texture Coordinate
	GLuint WaterLevel; // Water level at vertex [0, Inf]

	Vertex() = default;
	~Vertex() = default;

	Vertex(const glm::vec4& pos, const glm::vec3& norm, const glm::vec2& texCoord, const GLuint waterLevel = 0)
		: Position(pos), Normal(norm), TexCoord(texCoord), WaterLevel(waterLevel) {}

	bool operator==(const Vertex& other) const
	{
		return Position == other.Position &&
			Normal == other.Normal &&
			TexCoord == other.TexCoord;
	}
};

inline void SetVertexAttributes(const Vertex& vertex)
{
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Position))); // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal))); // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoord))); // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, WaterLevel))); // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(3);
}