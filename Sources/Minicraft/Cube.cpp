#include "pch.h"

#include "Cube.h"

Cube::Cube(Vector3 pos) : m_mModel(Matrix::CreateTranslation(pos)) {}

void Cube::Generate(const DeviceResources* deviceRes) {
	PushFace(Vector3::Zero, Vector3::Up, Vector3::Right);
	PushFace(Vector3::Right, Vector3::Up, Vector3::Forward);
	PushFace(Vector3::Right + Vector3::Forward, Vector3::Up, Vector3::Left);
	PushFace(Vector3::Forward, Vector3::Up, Vector3::Backward);
	PushFace(Vector3::Up, Vector3::Forward, Vector3::Right);
	PushFace(Vector3::Right + Vector3::Forward, Vector3::Left, Vector3::Backward);

	m_vBuffer.Create(deviceRes);
	m_iBuffer.Create(deviceRes);
}

void Cube::PushFace(const Vector3& pos, const Vector3& up, const Vector3& right) {
	// POSITION (xyz) / UV (xy)
	const uint32_t bottomLeft = m_vBuffer.PushVertex({pos, {0.0f, 0.0f}}); //                v0
	const uint32_t bottomRight = m_vBuffer.PushVertex({pos + right, { 1.0f, 0.0f }}); //     v1
	const uint32_t upLeft = m_vBuffer.PushVertex({ pos + up, {0, 1.0f} }); //                v2
	const uint32_t upRight = m_vBuffer.PushVertex({ pos + up + right, {1.0f,1.0f} }); // v3

	m_iBuffer.PushTriangle(bottomLeft, upLeft, upRight); //      tri0
	m_iBuffer.PushTriangle(bottomLeft, upRight, bottomRight); // tri1
}

void Cube::Draw(const DeviceResources* deviceRes) const {
	m_vBuffer.Apply(deviceRes);
	m_iBuffer.Apply(deviceRes);

	deviceRes->GetD3DDeviceContext()->DrawIndexed(m_iBuffer.Size(), 0, 0);
}

const Matrix& Cube::GetLocalMatrix() const {
	return m_mModel;
}
