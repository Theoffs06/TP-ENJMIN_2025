#include "pch.h"
#include "Chunk.h"

Chunk::Chunk(Vector3 pos) : m_mModel(Matrix::CreateTranslation(pos)) {
}

void Chunk::Generate(const DeviceResources* devRes) {
	for (int z = 0; z < CHUNK_SIZE_Z; z++) {
		for (int x = 0; x < CHUNK_SIZE_X; x++) {
			for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
				if (y < CHUNK_SIZE_Y / 2) SetCube(x, y, z, STONE);
				else if (y < CHUNK_SIZE_Y - 1) SetCube(x, y, z, DIRT);
				else SetCube(x, y, z, GRASS);
			}
		}
	}

	for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
		for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
			for (int x = 0; x < CHUNK_SIZE_X; ++x) {
				PushCube(Vector3(x,y,z));
			}
		}
	}

	m_vBuffer.Create(devRes);
	m_iBuffer.Create(devRes);
}

void Chunk::Draw(const DeviceResources* devRes) const {
	m_vBuffer.Apply(devRes);
	m_iBuffer.Apply(devRes);

	devRes->GetD3DDeviceContext()->DrawIndexed(m_iBuffer.Size(), 0, 0);
}

void Chunk::PushCube(const Vector3& pos) {
	const auto blockId = GetChunkCube(pos.x, pos.y, pos.z);
	if (blockId == nullptr || *blockId == EMPTY) return;
	auto& blockData = BlockData::Get(*blockId);
	if (blockData.id == EMPTY) return;

	PushFace(Vector3::Zero + pos, Vector3::Up, Vector3::Right, blockData.texIdSide);
	PushFace(Vector3::Right + pos, Vector3::Up, Vector3::Forward, blockData.texIdSide);
	PushFace(Vector3::Right + Vector3::Forward + pos, Vector3::Up, Vector3::Left, blockData.texIdSide);
	PushFace(Vector3::Forward + pos, Vector3::Up, Vector3::Backward, blockData.texIdSide);
	PushFace(Vector3::Up + pos, Vector3::Forward, Vector3::Right, blockData.texIdTop);
	PushFace(Vector3::Right + Vector3::Forward + pos, Vector3::Left, Vector3::Backward, blockData.texIdBottom);
}


void Chunk::PushFace(const Vector3& pos, const Vector3& up, const Vector3& right, int texId) {
	const Vector3 normal = right.Cross(up);
	if (!ShouldRenderFace(pos.x, pos.y, pos.z, normal.x, normal.y, normal.z)) return;

	const Vector2 uv(
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

bool Chunk::ShouldRenderFace(int cx, int cy, int cz, int dx, int dy, int dz) {
	const auto blockId = GetChunkCube(cx + dx, cy + dy, cz + dz);
	return blockId == nullptr || *blockId == EMPTY;
}

void Chunk::SetCube(int cx, int cy, int cz, BlockId id) {
	if (cx < 0 || cy < 0 || cz < 0) return;
	if (cx >= CHUNK_SIZE_X || cy >= CHUNK_SIZE_Y || cz >= CHUNK_SIZE_Z) return;
	m_data[cx + cy * CHUNK_SIZE_Y + cz * CHUNK_SIZE_Z * CHUNK_SIZE_Z] = id;
}

BlockId* Chunk::GetChunkCube(int cx, int cy, int cz) {
	if (cx < 0 || cy < 0 || cz < 0) return nullptr;
	if (cx >= CHUNK_SIZE_X || cy >= CHUNK_SIZE_Y || cz >= CHUNK_SIZE_Z) return nullptr;
	return &m_data[cx + cy * CHUNK_SIZE_Y + cz * CHUNK_SIZE_Z * CHUNK_SIZE_Z];
}

const Matrix& Chunk::GetLocalMatrix() const {
	return m_mModel;
}
