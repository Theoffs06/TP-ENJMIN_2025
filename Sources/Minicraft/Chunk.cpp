#include "pch.h"
#include "Chunk.h"
#include "World.h"

void Chunk::SetPosition(World* world, int cx, int cy, int cz) {
	m_mModel = Matrix::CreateTranslation(Vector3(cx, cy, cz) * CHUNK_SIZE);
	m_cx = cx;
	m_cy = cy;
	m_cz = cz;
	m_world = world;
}

void Chunk::Generate(const DeviceResources* deviceRes) {
	m_vBuffer.Clear();
	m_iBuffer.Clear();

	for (int z = 0; z < CHUNK_SIZE; ++z) {
		for (int y = 0; y < CHUNK_SIZE; ++y) {
			for (int x = 0; x < CHUNK_SIZE; ++x) {
				PushCube(x, y, z);
			}
		}
	}

	m_vBuffer.Create(deviceRes);
	m_iBuffer.Create(deviceRes);
}

void Chunk::Draw(const DeviceResources* deviceRes) const {
	if (m_iBuffer.Size() == 0) return;
	m_vBuffer.Apply(deviceRes);
	m_iBuffer.Apply(deviceRes);
	deviceRes->GetD3DDeviceContext()->DrawIndexed(m_iBuffer.Size(), 0, 0);
}

bool Chunk::ShouldRenderFace(int lx, int ly, int lz, int dx, int dy, int dz) const {
	const auto blockId = m_world->GetCube(
		m_cx * CHUNK_SIZE + lx + dx, 
		m_cy * CHUNK_SIZE + ly + dy, 
		m_cz * CHUNK_SIZE + lz + dz
	);

	if (!blockId || *blockId == EMPTY) return true;
	return false;
}

BlockId* Chunk::GetChunkCube(int lx, int ly, int lz) {
	if (lx < 0 || ly < 0 || lz < 0) return nullptr;
	if (lx >= CHUNK_SIZE || ly >= CHUNK_SIZE || lz >= CHUNK_SIZE) return nullptr;
	return &m_data[lx + ly * CHUNK_SIZE + lz * CHUNK_SIZE * CHUNK_SIZE];
}

void Chunk::PushCube(int lx, int ly, int lz) {
	const auto blockId = GetChunkCube(lx, ly, lz);
	if (!blockId || *blockId == EMPTY) return;
	auto& blockData = BlockData::Get(*blockId);

	Vector3 offset = Vector3(lx, ly, lz);
	if (ShouldRenderFace(lx, ly, lz, 0, 0, 1)) PushFace(offset + Vector3::Zero, Vector3::Up, Vector3::Right, blockData.texIdSide);
	if (ShouldRenderFace(lx, ly, lz, 1, 0, 0)) PushFace(offset + Vector3::Right, Vector3::Up, Vector3::Forward, blockData.texIdSide);
	if (ShouldRenderFace(lx, ly, lz, 0, 0, -1)) PushFace(offset + Vector3::Right + Vector3::Forward, Vector3::Up, Vector3::Left, blockData.texIdSide);
	if (ShouldRenderFace(lx, ly, lz, -1, 0, 0)) PushFace(offset + Vector3::Forward, Vector3::Up, Vector3::Backward, blockData.texIdSide);
	if (ShouldRenderFace(lx, ly, lz, 0, 1, 0)) PushFace(offset + Vector3::Up, Vector3::Forward, Vector3::Right, blockData.texIdTop);
	if (ShouldRenderFace(lx, ly, lz, 0, -1, 0)) PushFace(offset + Vector3::Right + Vector3::Forward, Vector3::Left, Vector3::Backward, blockData.texIdBottom);
}

void Chunk::PushFace(const Vector3& pos, const Vector3& up, const Vector3& right, int texId) {
	Vector2 uv(
		texId % 16,
		texId / 16
	);

	// POSITION (xyz) / UV (xy)
	const uint32_t bottomLeft = m_vBuffer.PushVertex({ pos, (uv + Vector2::UnitY) / 16.0f }); //               v0
	const uint32_t bottomRight = m_vBuffer.PushVertex({ pos + right, (uv + Vector2::One) / 16.0f }); //        v1
	const uint32_t upLeft = m_vBuffer.PushVertex({ pos + up, uv / 16.0f }); //                                    v2
	const uint32_t upRight = m_vBuffer.PushVertex({ pos + up + right, (uv + Vector2::UnitX) / 16.0f }); // v3

	m_iBuffer.PushTriangle(bottomLeft, upLeft, upRight); //      tri0 (v0, v2, v3)
	m_iBuffer.PushTriangle(bottomLeft, upRight, bottomRight); // tri1 (v0, v3, v2)
}

const Matrix& Chunk::GetLocalMatrix() const {
	return m_mModel;
}