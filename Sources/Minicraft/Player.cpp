#include "pch.h"
#include "Player.h"

Player::Player(Camera& camera, World& world) : m_camera(camera), m_world(world) {
}

void Player::Update(double dt, const Keyboard::State& kb, const Mouse::State& ms) const {
	Vector3 delta = Vector3::Zero;
	if (kb.Z) delta += m_camera.Forward();
	if (kb.S) delta -= m_camera.Forward();
	if (kb.Q) delta -= m_camera.Right();
	if (kb.D) delta += m_camera.Right();
	if (kb.Space) delta += m_camera.Up();
	if (kb.LeftShift) delta -= m_camera.Up();
	m_camera.SetPosition(m_camera.GetPosition() + delta * m_cameraSpeed * dt);

	Quaternion rot = m_camera.GetRotation();
	rot *= Quaternion::CreateFromAxisAngle(m_camera.Right(), -ms.y * dt * m_cameraSensitivity);
	rot *= Quaternion::CreateFromAxisAngle(Vector3::Up, -ms.x * dt * m_cameraSensitivity);
	m_camera.SetRotation(rot);
}

void Player::ShowImGUI() {
	ImGui::Begin("Player");

	if (ImGui::CollapsingHeader("Free Look Camera")) {
		ImGui::DragFloat("Camera Speed", &m_cameraSpeed, 0.1f, 0);
		ImGui::DragFloat("Camera Sensitivity", &m_cameraSensitivity, 0.1f, 0);
	}

	if (ImGui::CollapsingHeader("Player Camera")) {
		ImGui::DragFloat("Player Speed", &m_speed, 0.1f, 0);
		ImGui::DragFloat("Player Jump Force", &m_jumpForce, 0.1f, 0);
		ImGui::DragFloat("Player Gravity", &m_gravity, 0.1f, 0);
	}

	ImGui::End();
}
