#include "terrain.h"

#include <random>

void Terrain::SetVertices(const std::vector<Vertex>& vertices)
{
	m_Vertices = vertices;

	m_VAO->Bind();

	m_VAO->hasEBO = false;
	m_VAO->vertexCount = static_cast<GLuint>(m_Vertices.size());
	m_VAO->triangleCount = m_VAO->vertexCount / 3U;

	m_VBO.SetBufferData(m_Vertices);

	SetVertexAttributesPointers();
}

void Terrain::SetRandomVertices(const GLuint length, const GLfloat minHeight, const GLfloat maxHeight, const GLfloat scale)
{
	m_VAO->Bind();
	m_VAO->hasEBO = false;

	const GLfloat lengthScaled = length / scale;

	const GLuint halfLength = length / 2U;

	const GLfloat halfLengthScaled = lengthScaled / 2U;

	std::vector<glm::vec3> positions = std::vector<glm::vec3>();

	std::vector<float> topYs = std::vector<float>(lengthScaled, 0.0f);
	std::vector<float> botYs = std::vector<float>(lengthScaled, 0.0f);

	for (auto& y : topYs)
	{
		y = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxHeight - minHeight))) + minHeight;
	}

	for (GLuint i = 0; i < static_cast<GLuint>(lengthScaled) - 1U; i++)
	{
		float scaledI = static_cast<float>(i) * scale;

		float z = scaledI - static_cast<float>(halfLength);
		float t = scaledI / static_cast<float>(length);
		float tNext = (scaledI + scale) / static_cast<float>(length);

		botYs[0] = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxHeight - minHeight))) + minHeight;

		if (i == static_cast<GLuint>(lengthScaled) - 2U)
			int q = 12;

		for (GLuint j = 0; j < static_cast<GLuint>(lengthScaled) - 1U; j++)
		{
			float scaledJ = static_cast<float>(j) * scale;
			float x = scaledJ - static_cast<float>(halfLength);
			float s = scaledJ / static_cast<float>(length);
			float sNext = (scaledJ + scale) / static_cast<float>(length);

			botYs[j + 1Ui64] = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxHeight - minHeight))) + minHeight;

			glm::vec3 TL = { x        , topYs[j     ], z};
			glm::vec3 TR = { x + scale, topYs[j + 1U], z};
			glm::vec3 BR = { x + scale, botYs[j + 1U], z + scale};
			glm::vec3 BL = { x        , botYs[j     ], z + scale};

			glm::vec3 normalBL = glm::cross(TL - BL, BR - BL);
			if (normalBL.y < 0.0f) normalBL = -1.0f * normalBL;

			glm::vec3 normalTR = glm::cross(TL - TR, BR - TR);
			if (normalTR.y < 0.0f) normalTR = -1.0f * normalTR;

			m_Vertices.push_back({ TL, normalBL, {s    , t    } });
			m_Vertices.push_back({ BL, normalBL, {s    , tNext} });
			m_Vertices.push_back({ BR, normalBL, {sNext, tNext} });

			m_Vertices.push_back({ TL, normalTR, {s    , t    } });
			m_Vertices.push_back({ BR, normalTR, {sNext, tNext} });
			m_Vertices.push_back({ TR, normalTR, {sNext, t    } });

			if (j == static_cast<GLuint>(lengthScaled) - 2U)
				int r = 47;
		}

		topYs = botYs;
	}

	m_VAO->vertexCount = static_cast<GLuint>(m_Vertices.size());
	m_VAO->triangleCount = m_VAO->vertexCount / 3U;

	m_VBO.SetBufferData(m_Vertices);

	SetVertexAttributesPointers();
}
