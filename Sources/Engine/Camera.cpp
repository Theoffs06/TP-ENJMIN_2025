#include "pch.h"
#include "Camera..h"

Camera::Camera(float fov, float aspectRatio) : m_fov(fov) {
	UpdateAspectRatio(aspectRatio);
	UpdateViewMatrix();
}

void Camera::Create(const DeviceResources* deviceRes) {
	m_cbCamera.Create(deviceRes);
}

void Camera::Apply(const DeviceResources* deviceRes) {
	m_cbCamera.ApplyToVS(deviceRes, 1);

	m_cbCamera.data.mView = m_view.Transpose();
	m_cbCamera.data.mProjection = m_projection.Transpose();
	m_cbCamera.Update(deviceRes);
}

void Camera::UpdateAspectRatio(float aspectRatio) {
	m_projection = Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(m_fov),
		aspectRatio,
		m_near,
		m_far
	);
}

void Camera::UpdateViewMatrix() {
	const Vector3 forward = Vector3::Transform(Vector3::Forward, m_rotation);
	const Vector3 up = Vector3::Transform(Vector3::Up, m_rotation);

	m_view = Matrix::CreateLookAt(
		m_position,
		m_position + forward,
		up
	);
}

void Camera::SetPosition(const Vector3& pos) {
	m_position = pos;
	UpdateViewMatrix();
}

void Camera::SetRotation(const Quaternion& rot) {
	m_rotation = rot;
	UpdateViewMatrix();
}

Vector3 Camera::Forward() const {
	return Vector3::TransformNormal(Vector3::Forward, m_view.Invert());
}

Vector3 Camera::Right() const {
	return Vector3::TransformNormal(Vector3::Right, m_view.Invert());
}

Vector3 Camera::Up() const {
	return Vector3::TransformNormal(Vector3::Up, m_view.Invert());
}

Matrix Camera::GetInverseViewMatrix() const {
	return m_view.Invert();
}

Vector3 Camera::GetPosition() const {
	return  m_position;
}

Quaternion Camera::GetRotation() const {
	return m_rotation;
}
