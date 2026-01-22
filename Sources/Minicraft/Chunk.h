#pragma once

#include <array>

#include "Minicraft/Block.h"
#include "Engine/Buffer.h"
#include "Engine/VertexLayout.h"

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 16
#define CHUNK_SIZE CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z

class Chunk {
	std::array<BlockId, CHUNK_SIZE> m_data;
	
	Matrix m_mModel;
	VertexBuffer<VertexLayout_PositionUV> m_vBuffer;
	IndexBuffer m_iBuffer;
public:
	Chunk(Vector3 pos);

	void Generate(const DeviceResources* devRes);
	void Draw(const DeviceResources* devRes) const;
	void SetCube(int cx, int cy, int cz, BlockId id);
	BlockId* GetChunkCube(int cx, int cy, int cz);
	const Matrix& GetLocalMatrix() const;
private:
	void PushCube(const Vector3& pos);
	void PushFace(const Vector3& pos, const Vector3& up, const Vector3& right, int texId);
	bool ShouldRenderFace(int cx, int cy, int cz, int dx, int dy, int dz);
};
