#include "pch.h"

#include "Cube.h"

Cube::Cube(Vector3 pos, BlockId id) : m_mModel(Matrix::CreateTranslation(pos)), m_data(BlockData::Get(id)) {}

void Cube::Generate(const DeviceResources* deviceRes) {
	PushFace(Vector3::Zero, Vector3::Up, Vector3::Right, m_data.texIdSide);
	PushFace(Vector3::Right, Vector3::Up, Vector3::Forward, m_data.texIdSide);
	PushFace(Vector3::Right + Vector3::Forward, Vector3::Up, Vector3::Left, m_data.texIdSide);
	PushFace(Vector3::Forward, Vector3::Up, Vector3::Backward, m_data.texIdSide);
	PushFace(Vector3::Up, Vector3::Forward, Vector3::Right, m_data.texIdTop);
	PushFace(Vector3::Right + Vector3::Forward, Vector3::Left, Vector3::Backward, m_data.texIdBottom);

	m_vBuffer.Create(deviceRes);
	m_iBuffer.Create(deviceRes);
}

void Cube::PushFace(const Vector3& pos, const Vector3& up, const Vector3& right, int texId) {
	const Vector2 uv(
		texId % 16, 
		texId / 16
	);

	// POSITION (xyz) / UV (xy)
	const uint32_t bottomLeft = m_vBuffer.PushVertex({pos, (uv + Vector2::UnitY) / 16.0f }); //                                     v0
	const uint32_t bottomRight = m_vBuffer.PushVertex({pos + right, (uv + Vector2::One) / 16.0f}); //      v1
	const uint32_t upLeft = m_vBuffer.PushVertex({ pos + up, uv / 16.0f }); //            v2
	const uint32_t upRight = m_vBuffer.PushVertex({ pos + up + right, (uv + Vector2::UnitX) / 16.0f }); // v3

	m_iBuffer.PushTriangle(bottomLeft, upLeft, upRight); //      tri0 (v0, v2, v3)
	m_iBuffer.PushTriangle(bottomLeft, upRight, bottomRight); // tri1 (v0, v3, v2)
}

void Cube::Draw(const DeviceResources* deviceRes) const {
	m_vBuffer.Apply(deviceRes);
	m_iBuffer.Apply(deviceRes);

	deviceRes->GetD3DDeviceContext()->DrawIndexed(m_iBuffer.Size(), 0, 0);
}

const Matrix& Cube::GetLocalMatrix() const {
	return m_mModel;
}
