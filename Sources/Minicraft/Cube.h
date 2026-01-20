#pragma once

#include "Engine/Buffer.h"
#include "Engine/VertexLayout.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Cube {
	VertexBuffer<VertexLayout_PositionUV> m_vBuffer;
	IndexBuffer m_iBuffer;
	Matrix m_mModel;
public:
	Cube(Vector3 pos);

	void Generate(const DeviceResources* deviceRes);
	void Draw(const DeviceResources* deviceRes) const;

	const Matrix& GetLocalMatrix() const;
private:
	void PushFace(const Vector3& pos, const Vector3& up, const Vector3& right);
};