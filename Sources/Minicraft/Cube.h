#pragma once

#include "Engine/Buffer.h"
#include "Engine/VertexLayout.h"
#include "Block.h"

using namespace DirectX::SimpleMath;

class Cube {
	VertexBuffer<VertexLayout_PositionUV> vBuffer;
	IndexBuffer iBuffer;
	Matrix mModel;
	BlockId id;
public:
	Cube(Vector3 pos, BlockId id);

	void Generate(DeviceResources* deviceRes);
	void Draw(DeviceResources* deviceRes);
	const Matrix& GetLocalMatrix() const { return mModel; }

private:
	void PushFace(Vector3 pos, Vector3 up, Vector3 right, int texId);
};