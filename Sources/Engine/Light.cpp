#include "pch.h"
#include "Light.h"

float imDirection[3];
float imAmbientColor[3];
float imDiffuseColor[3];

Light::Light(Vector3 dir, Vector3 ambientColor, Vector3 diffuseColor) {
	m_direction = dir;
	imDirection[0] = dir.x;
	imDirection[1] = dir.y;
	imDirection[2] = dir.z;

	m_ambientColor = ambientColor;
	imAmbientColor[0] = ambientColor.x;
	imAmbientColor[1] = ambientColor.y;
	imAmbientColor[2] = ambientColor.z;

	m_diffuseColor = diffuseColor;
	imDiffuseColor[0] = diffuseColor.x;
	imDiffuseColor[1] = diffuseColor.y;
	imDiffuseColor[2] = diffuseColor.z;
}

void Light::Create(const DeviceResources* devRes) {
	m_cbLight.Create(devRes);
}

void Light::Apply(const DeviceResources* devRes) {
	m_cbLight.ApplyToPS(devRes, 0);

	m_cbLight.data.direction = m_direction;
	m_cbLight.data.ambientColor = m_ambientColor;
	m_cbLight.data.diffuseColor = m_diffuseColor;
	m_cbLight.Update(devRes);
}

void Light::ShowImGui() {
	ImGui::Begin("Light");

	ImGui::InputFloat3("Direction", imDirection);
	SetDirection({ imDirection[0], imDirection[1], imDirection[2] });

	ImGui::ColorEdit3("Ambient Color", imAmbientColor);
	SetAmbientColor({ imAmbientColor[0], imAmbientColor[1], imAmbientColor[2] });

	ImGui::ColorEdit3("Diffuse Color", imDiffuseColor);
	SetDiffuseColor({ imDiffuseColor[0], imDiffuseColor[1], imDiffuseColor[2] });

	ImGui::End();
}

void Light::SetDirection(const Vector3 dir) {
	m_direction = dir;
}

void Light::SetAmbientColor(const Vector3 ambientColor) {
	m_ambientColor = ambientColor;
}

void Light::SetDiffuseColor(const Vector3 diffuseColor) {
	m_diffuseColor = diffuseColor;
}
