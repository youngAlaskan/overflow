#include "renderer.h"

void Renderer::Render()
{
	if (!m_VAOs) return;

	for (const auto& VAO : *m_VAOs)
	{
		if (!VAO)
			continue;

		ShaderProgram::Use(m_VAOtoShaderProgram[VAO->m_ID]);

		VAO->Bind();
		if (VAO->hasEBO)
			glDrawElements(GL_TRIANGLES, VAO->triangleCount, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, VAO->vertexCount);
	}
}