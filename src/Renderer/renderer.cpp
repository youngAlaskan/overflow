#include "renderer.h"

void Renderer::Render()
{
	for (auto VAO : m_VAOs)
	{
		m_VAOtoShaderProgram[VAO].Use();
		VAO.Bind();
		if (VAO.hasEBO)
			glDrawElements(GL_TRIANGLES, VAO.triangleCount, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, VAO.triangleCount);
	}
}