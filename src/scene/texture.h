#pragma once

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION

class Texture
{
public:
	Texture() { Init(); }
	Texture(const glm::vec4& color);
	Texture(const std::string& filepath);
	~Texture()
		{ glDeleteTextures(1, &m_ID); }

	GLuint GetID() const { return m_ID; }
	std::string GetPath() const { return m_Path; }

	void SetWrap(const GLint sWrap, const GLint tWrap);
	void SetWrap(const GLint wrap);
	void SetFilter(const GLint minFilter, const GLint maxFilter);
	void SetFilter(const GLint filter);

	void Use(const int index) const;
	void Bind() const { glBindTexture(GL_TEXTURE_2D, m_ID); }

	bool operator==(const Texture& other) const
		{ return m_ID == other.m_ID && m_Path == other.m_Path; }
	
	operator GLuint& () { return m_ID; }
	operator const GLuint& () const { return m_ID; }

private:
	void Init() { glGenTextures(1, &m_ID); }

private:
	GLuint m_ID = 0;
	std::string m_Path = std::string();
};
