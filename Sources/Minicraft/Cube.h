#pragma once

#include "Minicraft/Block.h"
#include "Engine/Buffer.h"
#include "Engine/VertexLayout.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Cube {
	VertexBuffer<VertexLayout_PositionUV> m_vBuffer;
	IndexBuffer m_iBuffer;
	Matrix m_mModel;
	BlockData m_data;
public:
	Cube(Vector3 pos, BlockId id);

	void Generate(const DeviceResources* deviceRes);
	void Draw(const DeviceResources* deviceRes) const;

	const Matrix& GetLocalMatrix() const;
private:
	void PushFace(const Vector3& pos, const Vector3& up, const Vector3& right, int texId);
};