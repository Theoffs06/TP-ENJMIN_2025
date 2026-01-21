#include "pch.h"

#include "Cube.h"

Cube::Cube(Vector3 pos, BlockId id) : mModel(Matrix::CreateTranslation(pos)), id(id) {
}

void Cube::Generate(DeviceResources* deviceRes) {
	auto& blockData = BlockData::Get(id);

	PushFace(Vector3::Zero, Vector3::Up, Vector3::Right, blockData.texIdSide);
	PushFace(Vector3::Right, Vector3::Up, Vector3::Forward, blockData.texIdSide);
	PushFace(Vector3::Right + Vector3::Forward, Vector3::Up, Vector3::Left, blockData.texIdSide);
	PushFace(Vector3::Forward, Vector3::Up, Vector3::Backward, blockData.texIdSide);
	PushFace(Vector3::Up, Vector3::Forward, Vector3::Right, blockData.texIdTop);
	PushFace(Vector3::Right + Vector3::Forward, Vector3::Left, Vector3::Backward, blockData.texIdBottom);
	vBuffer.Create(deviceRes);
	iBuffer.Create(deviceRes);
}

void Cube::Draw(DeviceResources* deviceRes) {
	vBuffer.Apply(deviceRes);
	iBuffer.Apply(deviceRes);
	deviceRes->GetD3DDeviceContext()->DrawIndexed(iBuffer.Size(), 0, 0);
}

void Cube::PushFace(Vector3 pos, Vector3 up, Vector3 right, int texId) {
	Vector2 uv(
		texId % 16,
		texId / 16
	);
	uint32_t bottomLeft = vBuffer.PushVertex(VertexLayout_PositionUV(pos, (uv + Vector2::UnitY) / 16.0f));
	uint32_t bottomRight = vBuffer.PushVertex(VertexLayout_PositionUV(pos + right, (uv + Vector2::One) / 16.0f));
	uint32_t upLeft = vBuffer.PushVertex(VertexLayout_PositionUV(pos + up, uv / 16.0f));
	uint32_t upRight = vBuffer.PushVertex(VertexLayout_PositionUV(pos + up + right, (uv + Vector2::UnitX) / 16.0f));
	iBuffer.PushTriangle(bottomLeft, upLeft, upRight);
	iBuffer.PushTriangle(bottomLeft, upRight, bottomRight);
}
