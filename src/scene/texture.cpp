#include "Texture.h"

#include <iostream>

#include <stb_image\stb_image.h>

Texture::Texture(const glm::vec4& color)
{
	Init();

	// load and create a texture 
	// -------------------------
	Bind();

	// set the texture wrapping parameters
	SetWrap(GL_REPEAT);

	// set texture filtering parameters
	SetFilter(GL_LINEAR);

	const auto data = new unsigned char[4Ui64 * sizeof(unsigned char)];
	data[0] = static_cast<unsigned char>(color.r * 255.0f);
	data[1] = static_cast<unsigned char>(color.g * 255.0f);
	data[2] = static_cast<unsigned char>(color.b * 255.0f);
	data[3] = static_cast<unsigned char>(color.a * 255.0f);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	delete[] data;

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const std::string& filepath) : m_Path(filepath)
{
	Init();

	// load and create a texture 
	// -------------------------
	Bind();

	// set the texture wrapping parameters
	SetWrap(GL_REPEAT, GL_REPEAT);

	// set texture filtering parameters
	SetFilter(GL_LINEAR, GL_LINEAR);

	// load image, create texture and generate mipmaps
	int width, height, channelsN;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channelsN, 0);
	if (data)
	{
		const GLint format = channelsN == 1 ? GL_RED : (channelsN == 4 ? GL_RGBA : GL_RGB);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
}

void Texture::SetWrap(const GLint sWrap, const GLint tWrap) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}

void Texture::SetWrap(const GLint wrap)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
}

void Texture::SetFilter(const GLint minFilter, const GLint maxFilter)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
}

void Texture::SetFilter(const GLint filter)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

void Texture::Use(const int index) const
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}
