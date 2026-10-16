#include "marchingCubes.h"

// Field value inside a fully packed lattice; use isoLevel = fraction * RestField(...)
float MarchingCubesMesher::RestField(float spacing, float R)
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

void MarchingCubesMesher::BinParticles(const std::vector<glm::vec3>& pts)
{
	m_Index.clear();
	for (int i = 0; i < m_Used; i++) m_Lists[i].clear();
	m_Used = 0;

	for (int i = 0; i < static_cast<int>(pts.size()); i++)
	{
		const glm::vec3& p = pts[i];
		const glm::ivec3 pl = glm::ivec3(glm::ceil((p - m_R) / m_Cs));
		const glm::ivec3 ph = glm::ivec3(glm::floor((p + m_R) / m_Cs));
		const glm::ivec3 bl(FloorDiv(pl.x - 1, B), FloorDiv(pl.y - 1, B), FloorDiv(pl.z - 1, B));
		const glm::ivec3 bh(FloorDiv(ph.x, B), FloorDiv(ph.y, B), FloorDiv(ph.z, B));

		for (int bz = bl.z; bz <= bh.z; bz++)
			for (int by = bl.y; by <= bh.y; by++)
				for (int bx = bl.x; bx <= bh.x; bx++)
				{
					const uint64_t key = Key(bx, by, bz);
					auto it = m_Index.find(key);
					int b;
					if (it == m_Index.end())
					{
						b = m_Used++;
						if (b >= static_cast<int>(m_Pool.size())) { m_Pool.emplace_back(); m_Lists.emplace_back(); }
						m_Pool[b].coord = glm::ivec3(bx, by, bz);
						m_Index.emplace(key, b);
					}
					else b = it->second;
					m_Lists[b].push_back(i);
				}
	}
}

void MarchingCubesMesher::SplatInto(Block& blk, const glm::vec3& p) const
{
	const float R2 = m_R * m_R, invR2 = 1.0f / R2;
	const glm::ivec3 o = blk.coord * B;
	const glm::ivec3 pl = glm::ivec3(glm::ceil((p - m_R) / m_Cs));
	const glm::ivec3 ph = glm::ivec3(glm::floor((p + m_R) / m_Cs));
	const glm::ivec3 lo = glm::max(pl, o), hi = glm::min(ph, o + B);

	for (int z = lo.z; z <= hi.z; z++)
	{
		const float dz = z * m_Cs - p.z, dz2 = dz * dz;
		if (dz2 >= R2) continue;
		for (int y = lo.y; y <= hi.y; y++)
		{
			const float dy = y * m_Cs - p.y;
			const float rem = R2 - dz2 - dy * dy;
			if (rem <= 0.0f) continue;

			const float s = std::sqrt(rem);
			const int xl = std::max(lo.x, static_cast<int>(std::ceil((p.x - s) / m_Cs)));
			const int xh = std::min(hi.x, static_cast<int>(std::floor((p.x + s) / m_Cs)));
			const int rowBase = (xl - o.x) + N * ((y - o.y) + N * (z - o.z));

			for (int x = xl; x <= xh; x++)
			{
				const float dx = x * m_Cs - p.x;
				const float r2 = dx * dx + dy * dy + dz2;
				if (r2 >= R2) continue;
				const float t = 1.0f - r2 * invR2, t2 = t * t;
				const float gk = -6.0f * t2 * invR2;
				glm::vec4& v = blk.data[rowBase + (x - xl)];
				v.w += t2 * t;
				v.x += gk * dx; v.y += gk * dy; v.z += gk * dz;
			}
		}
	}
}

void MarchingCubesMesher::MarchBlock(const Block& blk, SurfaceMesh& out) const
{
	out.Clear();
	constexpr int N2 = N * N, N3 = N * N * N;
	const glm::vec4* d = blk.data.data();

	// Skip blocks with no surface: everything below the iso level, or everything above it
	float mn = d[0].w, mx = d[0].w;
	for (int i = 1; i < N3; i++) { mn = std::min(mn, d[i].w); mx = std::max(mx, d[i].w); }
	if (mx <= m_Iso || mn > m_Iso) return;

	uint8_t inside[N3];
	for (int i = 0; i < N3; i++) inside[i] = d[i].w > m_Iso ? 1 : 0;

	// One vertex slot per grid edge (+x, +y, +z from each point); -1 = not created yet
	int32_t cache[3 * N3];
	std::fill(cache, cache + 3 * N3, -1);

	static const int kEdgeAxis[12] = { 0, 1, 0, 1, 0, 1, 0, 1, 2, 2, 2, 2 };
	static const int kEdgeBase[12][3] = {
		{ 0,0,0 },{ 1,0,0 },{ 0,1,0 },{ 0,0,0 },{ 0,0,1 },{ 1,0,1 },{ 0,1,1 },{ 0,0,1 },
		{ 0,0,0 },{ 1,0,0 },{ 1,1,0 },{ 0,1,0 } };

	const glm::ivec3 base = blk.coord * B;

	auto edgeVertex = [&](int cx, int cy, int cz, int e) -> uint32_t
		{
			const int axis = kEdgeAxis[e];
			const int bx = cx + kEdgeBase[e][0], by = cy + kEdgeBase[e][1], bz = cz + kEdgeBase[e][2];
			const int p0 = bx + N * (by + N * bz);
			int32_t& slot = cache[axis * N3 + p0];
			if (slot >= 0) return static_cast<uint32_t>(slot);

			const int p1 = p0 + (axis == 0 ? 1 : axis == 1 ? N : N2);
			const glm::vec4& a = d[p0];
			const glm::vec4& b = d[p1];
			const float t = (m_Iso - a.w) / (b.w - a.w);   // a and b are on opposite sides, so never 0

			glm::vec3 pa = glm::vec3(base + glm::ivec3(bx, by, bz)) * m_Cs;
			glm::vec3 pb = pa;
			pb[axis] += m_Cs;

			SurfaceVertex v;
			v.Position = glm::mix(pa, pb, t);
			const glm::vec3 g = glm::mix(glm::vec3(a), glm::vec3(b), t);
			const float len = glm::length(g);
			v.Normal = len > 1e-8f ? -g / len : glm::vec3(0.0f, 1.0f, 0.0f);

			slot = static_cast<int32_t>(out.Vertices.size());
			out.Vertices.push_back(v);
			return static_cast<uint32_t>(slot);
		};

	for (int z = 0; z < B; z++)
		for (int y = 0; y < B; y++)
			for (int x = 0; x < B; x++)
			{
				const int p = x + N * (y + N * z);
				const int cubeIdx =
					inside[p] | (inside[p + 1] << 1) | (inside[p + 1 + N] << 2) | (inside[p + N] << 3) |
					(inside[p + N2] << 4) | (inside[p + 1 + N2] << 5) | (inside[p + 1 + N + N2] << 6) | (inside[p + N + N2] << 7);
				if (cubeIdx == 0 || cubeIdx == 255) continue;

				const auto* tri = kTriTable[cubeIdx];
				for (int i = 0; tri[i] != -1; i += 3)
				{
					const uint32_t i0 = edgeVertex(x, y, z, tri[i]);
					uint32_t i1 = edgeVertex(x, y, z, tri[i + 1]);
					uint32_t i2 = edgeVertex(x, y, z, tri[i + 2]);

					const SurfaceVertex& v0 = out.Vertices[i0];
					const SurfaceVertex& v1 = out.Vertices[i1];
					const SurfaceVertex& v2 = out.Vertices[i2];
					const glm::vec3 n = glm::cross(v1.Position - v0.Position, v2.Position - v0.Position);
					if (glm::dot(n, v0.Normal + v1.Normal + v2.Normal) < 0.0f) std::swap(i1, i2);

					out.idx.push_back(i0);
					out.idx.push_back(i1);
					out.idx.push_back(i2);
				}
			}
}

void MarchingCubesMesher::Build(const std::vector<glm::vec3>& points, const Params& prm, SurfaceMesh& out)
{
	out.Clear();
	m_Cs = prm.cellSize;
	m_R = prm.influenceRadius;
	m_Iso = prm.isoLevel;

	BinParticles(points);
	if (static_cast<int>(m_Local.size()) < m_Used) m_Local.resize(m_Used);

#pragma omp parallel for schedule(dynamic, 4)
	for (int b = 0; b < m_Used; b++)
		FillBlock(m_Pool[b], m_Lists[b], points);

#pragma omp parallel for schedule(dynamic, 4)
	for (int b = 0; b < m_Used; b++)
		MarchBlock(m_Pool[b], m_Local[b]);

	size_t nv = 0, ni = 0;
	for (int b = 0; b < m_Used; b++) { nv += m_Local[b].Vertices.size(); ni += m_Local[b].idx.size(); }
	out.Vertices.reserve(nv);
	out.idx.reserve(ni);
	for (int b = 0; b < m_Used; b++)
	{
		const uint32_t off = static_cast<uint32_t>(out.Vertices.size());
		out.Vertices.insert(out.Vertices.end(), m_Local[b].Vertices.begin(), m_Local[b].Vertices.end());
		for (uint32_t i : m_Local[b].idx) out.idx.push_back(i + off);
	}
}