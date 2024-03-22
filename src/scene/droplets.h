#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "../glObjects.h"

constexpr float TAU = 6.283185307179586f; // 2 * pi

class Droplets
{
public:
	Droplets(std::vector<glm::vec3> centers) { SetVAO(centers); }

public:
	std::vector<glm::vec3> m_Vertices = std::vector<glm::vec3>();
	std::vector<glm::vec3> m_Centers = std::vector<glm::vec3>();
	std::shared_ptr<VAO> m_VAO = std::make_shared<VAO>();

private:
	void CreateSphere();

	void SetVAO(std::vector<glm::vec3> centers);
};
