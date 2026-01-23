#pragma once

#include "Engine/Buffer.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Camera {
	float m_near = 0.01f;
	float m_far = 500.0f;
	float m_fov = 60.0f;

	Quaternion m_rotation = Quaternion::Identity;
	Vector3 m_position = Vector3::Backward * 5;
	Matrix m_projection;
	Matrix m_view;

	struct CameraData {
		Matrix mView;
		Matrix mProjection;
	};

	ConstantBuffer<CameraData> m_cbCamera;
public:
	Camera(float fov, float aspectRatio);

	void Create(const DeviceResources* deviceRes);
	void Apply(const DeviceResources* deviceRes);
	void UpdateAspectRatio(float aspectRatio);

	void SetPosition(const Vector3& pos);
	void SetRotation(const Quaternion& rot);

	Vector3 Forward() const;
	Vector3 Right() const;
	Vector3 Up() const;
	Matrix GetInverseViewMatrix() const;
	Vector3 GetPosition() const;
	Quaternion GetRotation() const;
private:
	void UpdateViewMatrix();
};