#pragma once

#include "Buffer.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Light {
	Vector3 m_direction;
	Vector3 m_ambientColor;
	Vector3 m_diffuseColor;

	struct alignas(16) LightData {
		Vector3 direction;
		Vector3 ambientColor;
		Vector3 diffuseColor;
	};

	ConstantBuffer<LightData> m_cbLight;
public:
	Light(Vector3 dir, Vector3 ambientColor, Vector3 diffuseColor);
	
	void Create(const DeviceResources* devRes);
	void Apply(const DeviceResources* devRes);
	void ShowImGui();

	void SetDirection(Vector3 dir);
	void SetAmbientColor(Vector3 ambientColor);
	void SetDiffuseColor(Vector3 diffuseColor);
};
