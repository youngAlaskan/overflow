#pragma once

// marchingCubes.h
#pragma once
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include "marchingCubesTables.h"

struct SurfaceVertex { glm::vec3 Position; glm::vec3 Normal; };

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
	static float RestField(float spacing, float R)
	{
		const int n = static_cast<int>(std::ceil(R / spacing));
		const float R2 = R * R;
		float sum = 0.0f;
		for (int i = -n; i <= n; i++)
			for (int j = -n; j <= n; j++)
				for (int k = -n; k <= n; k++)
				{
					const float r2 = spacing * spacing * float(i * i + j * j + k * k);
					if (r2 < R2) { const float t = 1.0f - r2 / R2; sum += t * t * t; }
				}
		return sum;
	}

	void Build(const std::vector<glm::vec3>& points, const Params& prm, std::vector<SurfaceVertex>& out)
	{
		out.clear();
		m_Blocks.clear();
		m_Cs = prm.cellSize;
		m_R = prm.influenceRadius;
		m_Iso = prm.isoLevel;

		for (const auto& p : points) Splat(p);

		std::vector<const Block*> blocks;
		blocks.reserve(m_Blocks.size());
		for (const auto& kv : m_Blocks) blocks.push_back(&kv.second);

		std::vector<std::vector<SurfaceVertex>> local(blocks.size());

#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < static_cast<int>(blocks.size()); i++)
			MarchBlock(*blocks[i], local[i]);

		size_t total = 0;
		for (const auto& v : local) total += v.size();
		out.reserve(total);
		for (const auto& v : local) out.insert(out.end(), v.begin(), v.end());
	}

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

	Block& GetBlock(int bx, int by, int bz)
	{
		auto it = m_Blocks.find(Key(bx, by, bz));
		if (it == m_Blocks.end())
		{
			Block b;
			b.coord = glm::ivec3(bx, by, bz);
			b.data.assign(N * N * N, glm::vec4(0.0f));
			it = m_Blocks.emplace(Key(bx, by, bz), std::move(b)).first;
		}
		return it->second;
	}

	void Splat(const glm::vec3& p)
	{
		const float R2 = m_R * m_R, invR2 = 1.0f / R2;
		const glm::ivec3 pl = glm::ivec3(glm::ceil((p - m_R) / m_Cs));
		const glm::ivec3 ph = glm::ivec3(glm::floor((p + m_R) / m_Cs));
		const glm::ivec3 bl(FloorDiv(pl.x - 1, B), FloorDiv(pl.y - 1, B), FloorDiv(pl.z - 1, B));
		const glm::ivec3 bh(FloorDiv(ph.x, B), FloorDiv(ph.y, B), FloorDiv(ph.z, B));

		for (int bz = bl.z; bz <= bh.z; bz++)
			for (int by = bl.y; by <= bh.y; by++)
				for (int bx = bl.x; bx <= bh.x; bx++)
				{
					Block& blk = GetBlock(bx, by, bz);
					const glm::ivec3 o(bx * B, by * B, bz * B);
					const glm::ivec3 lo = glm::max(pl, o), hi = glm::min(ph, o + B);

					for (int z = lo.z; z <= hi.z; z++)
						for (int y = lo.y; y <= hi.y; y++)
							for (int x = lo.x; x <= hi.x; x++)
							{
								const glm::vec3 d = glm::vec3(x, y, z) * m_Cs - p;
								const float r2 = glm::dot(d, d);
								if (r2 >= R2) continue;

								const float t = 1.0f - r2 * invR2, t2 = t * t;
								const float gk = -6.0f * t2 * invR2;   // d(t^3)/dx = 3 t^2 * (-2 d / R^2)
								glm::vec4& v = blk.data[(x - o.x) + N * ((y - o.y) + N * (z - o.z))];
								v.w += t2 * t;
								v.x += gk * d.x; v.y += gk * d.y; v.z += gk * d.z;
							}
				}
	}

	void MarchBlock(const Block& blk, std::vector<SurfaceVertex>& out) const
	{
		static const glm::ivec3 kCorner[8] = {
			{ 0,0,0 },{ 1,0,0 },{ 1,1,0 },{ 0,1,0 },{ 0,0,1 },{ 1,0,1 },{ 1,1,1 },{ 0,1,1 } };
		static const int kEdgeCorner[12][2] = {
			{ 0,1 },{ 1,2 },{ 2,3 },{ 3,0 },{ 4,5 },{ 5,6 },{ 6,7 },{ 7,4 },{ 0,4 },{ 1,5 },{ 2,6 },{ 3,7 } };

		auto at = [&](int x, int y, int z) -> const glm::vec4& { return blk.data[x + N * (y + N * z)]; };
		const glm::ivec3 base = blk.coord * B;

		for (int z = 0; z < B; z++)
			for (int y = 0; y < B; y++)
				for (int x = 0; x < B; x++)
				{
					const glm::vec4* c[8];
					int idx = 0;
					for (int i = 0; i < 8; i++)
					{
						c[i] = &at(x + kCorner[i].x, y + kCorner[i].y, z + kCorner[i].z);
						if (c[i]->w > m_Iso) idx |= 1 << i;
					}
					const int edges = kEdgeTable[idx];
					if (edges == 0) continue;

					glm::vec3 vp[12], vn[12];
					for (int e = 0; e < 12; e++)
					{
						if (!(edges & (1 << e))) continue;
						const int a = kEdgeCorner[e][0], b = kEdgeCorner[e][1];
						const float fa = c[a]->w, fb = c[b]->w;
						const float t = (m_Iso - fa) / (fb - fa);
						const glm::vec3 pa = glm::vec3(base + glm::ivec3(x, y, z) + kCorner[a]) * m_Cs;
						const glm::vec3 pb = glm::vec3(base + glm::ivec3(x, y, z) + kCorner[b]) * m_Cs;
						vp[e] = glm::mix(pa, pb, t);
						const glm::vec3 g = glm::mix(glm::vec3(*c[a]), glm::vec3(*c[b]), t);
						const float len = glm::length(g);
						vn[e] = len > 1e-8f ? -g / len : glm::vec3(0.0f, 1.0f, 0.0f);   // outward = down the gradient
					}

					for (int i = 0; kTriTable[idx][i] != -1; i += 3)
					{
						const int e0 = kTriTable[idx][i];
						int e1 = kTriTable[idx][i + 1], e2 = kTriTable[idx][i + 2];
						const glm::vec3 n = glm::cross(vp[e1] - vp[e0], vp[e2] - vp[e0]);
						if (glm::dot(n, vn[e0] + vn[e1] + vn[e2]) < 0.0f) std::swap(e1, e2);
						out.push_back({ vp[e0], vn[e0] });
						out.push_back({ vp[e1], vn[e1] });
						out.push_back({ vp[e2], vn[e2] });
					}
				}
	}

	std::unordered_map<uint64_t, Block> m_Blocks;
	float m_Cs = 0.3f, m_R = 1.0f, m_Iso = 0.5f;
};