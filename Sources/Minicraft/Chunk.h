#pragma once

#include "Engine/Buffer.h"
#include "Engine/VertexLayout.h"
#include "Minicraft/Block.h"
#include <array>

class World;

class Chunk {
public:
	constexpr static int CHUNK_SIZE = 8;
private:
	std::array<BlockId, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> m_data;
	VertexBuffer<VertexLayout_PositionUV> m_vBuffer;
	IndexBuffer m_iBuffer;
	Matrix m_mModel;
	World* m_world;
	int m_cx, m_cy, m_cz;
public:
	Chunk() = default;

	void SetPosition(World* world, int cx, int cy, int cz);
	void Generate(const DeviceResources* deviceRes);
	void Draw(const DeviceResources* deviceRes) const;

	BlockId* GetChunkCube(int cx, int cy, int cz);
	const Matrix& GetLocalMatrix() const;
private:
	bool ShouldRenderFace(int cx, int cy, int cz, int dx, int dy, int dz) const;
	void PushCube(int cx, int cy, int cz);
	void PushFace(const Vector3& pos, const Vector3& up, const Vector3& right, int texId);
};