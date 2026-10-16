#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>

#include "marchingCubesTables.h"

struct SurfaceVertex { glm::vec3 Position; glm::vec3 Normal; };

struct SurfaceMesh
{
	std::vector<SurfaceVertex> Vertices;
	std::vector<uint32_t> idx;
	void Clear() { Vertices.clear(); idx.clear(); }
};

class MarchingCubesMesher
{
public:
	struct Params
	{
		float cellSize = 0.3f;   // grid spacing; cost scales with 1/cellSize^3
		float influenceRadius = 1.0f;   // kernel radius, about 2x particle spacing (matches H)
		float isoLevel = 0.5f;   // absolute field value, see RestField()
	};

	// Field value inside a fully packed lattice; use isoLevel = fraction * RestField(...)
	static float RestField(float spacing, float R);

	void Build(const std::vector<glm::vec3>& points, const Params& prm, SurfaceMesh& out);

private:
	static constexpr int B = 8;        // cells per block edge
	static constexpr int N = B + 1;    // points per block edge (borders duplicated)

	struct Block
	{
		glm::ivec3 coord{ 0 };
		std::vector<glm::vec4> data;   // xyz = field gradient, w = field value
	};

	static int FloorDiv(int a, int b)
	{
		int q = a / b;
		return (a % b != 0 && ((a < 0) != (b < 0))) ? q - 1 : q;
	}

	static uint64_t Key(int x, int y, int z)
	{
		return (uint64_t(uint32_t(x + (1 << 20)) & 0x1FFFFF) << 42) |
			(uint64_t(uint32_t(y + (1 << 20)) & 0x1FFFFF) << 21) |
			uint64_t(uint32_t(z + (1 << 20)) & 0x1FFFFF);
	}

	void BinParticles(const std::vector<glm::vec3>& pts);

	void FillBlock(Block& blk, const std::vector<int>& list, const std::vector<glm::vec3>& pts) const
	{
		blk.data.assign(N * N * N, glm::vec4(0.0f));   // reuses capacity after the first frame
		for (int i : list) SplatInto(blk, pts[i]);
	}


	void SplatInto(Block& blk, const glm::vec3& p) const;

	void MarchBlock(const Block& blk, SurfaceMesh& out) const;

	std::unordered_map<uint64_t, int> m_Index;
	std::vector<Block> m_Pool;
	std::vector<std::vector<int>> m_Lists;
	std::vector<SurfaceMesh> m_Local;
	int m_Used = 0;
	float m_Cs = 0.3f, m_R = 1.0f, m_Iso = 0.5f;
};