#include "renderer.h"

void Renderer::Render()
{ 
	if (!m_VAOs) return;

	for (const auto& VAO : *m_VAOs)
	{
		if (!VAO)
			continue;

		ShaderProgram::Use(m_VAOtoShaderProgram[VAO->GetID()]);

		VAO->Bind();

		if (VAO->isInstanced)
		{
			if (VAO->hasEBO)
				glDrawElementsInstanced(GL_TRIANGLES, 3 * VAO->triangleCount, GL_UNSIGNED_INT, 0, VAO->instanceCount);
			else
				glDrawArraysInstanced(GL_TRIANGLES, 0, VAO->vertexCount, VAO->instanceCount);
			glPointSize(5.0f);
			glDrawArrays(GL_POINTS, 0, VAO->vertexCount);
		}
		else
		{
			if (VAO->hasEBO)
				glDrawElements(GL_TRIANGLES, 3 * VAO->triangleCount, GL_UNSIGNED_INT, 0);
			else
				glDrawArrays(GL_TRIANGLES, 0, VAO->vertexCount);
		}
	}
}
