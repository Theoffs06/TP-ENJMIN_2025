#include "pch.h"
#include "Player.h"
#include "World.h"
#include <array>
#include <map>

using namespace DirectX;
using ButtonState = DirectX::Mouse::ButtonStateTracker::ButtonState;

Player::Player(Camera& camera, World& world) : m_camera(camera), m_world(world) {
	m_position = camera.GetPosition();
}

std::vector<std::array<int, 3>> Raycast(Vector3 pos, Vector3 dir, float maxDistance) {
	std::map<float, std::array<int, 3>> cubes;

	if (dir.x != 0) {
		float deltaY = dir.y / dir.x;
		float deltaZ = dir.z / dir.x;
		float offsetY = pos.y - pos.x * deltaY;
		float offsetZ = pos.z - pos.x * deltaZ;

		float currentX = dir.x > 0 ? ceil(pos.x) : floor(pos.x);
		do {
			Vector3 collision = Vector3(
				currentX,
				offsetY + currentX * deltaY,
				offsetZ + currentX * deltaZ
			);
			float dist = Vector3::Distance(pos, collision);
			if (dist > maxDistance) break;
			
			cubes[dist] = {
				(int) (currentX - (dir.x < 0 ? 1 : 0)),
				(int) floor(collision.y),
				(int) ceil(collision.z)
			};
			currentX += dir.x > 0 ? 1 : -1;
		} while (true);
	}

	if (dir.y != 0) {
		float deltaX = dir.x / dir.y;
		float deltaZ = dir.z / dir.y;
		float offsetX = pos.x - pos.y * deltaX;
		float offsetZ = pos.z - pos.y * deltaZ;

		float currentY = dir.y > 0 ? ceil(pos.y) : floor(pos.y);
		do {
			Vector3 collision = Vector3(
				offsetX + currentY * deltaX,
				currentY,
				offsetZ + currentY * deltaZ
			);
			float dist = Vector3::Distance(pos, collision);
			if (dist > maxDistance) break;

			cubes[dist] = {
				(int) floor(collision.x),
				(int) (currentY - (dir.y < 0 ? 1 : 0)),
				(int) ceil(collision.z)
			};

			currentY += dir.y > 0 ? 1 : -1;
		} while (true);
	}

	if (dir.z != 0) {
		float deltaX = dir.x / dir.z;
		float deltaY = dir.y / dir.z;
		float offsetX = pos.x - pos.z * deltaX;
		float offsetY = pos.y - pos.z * deltaY;

		float currentZ = dir.z > 0 ? ceil(pos.z) : floor(pos.z);
		do {
			Vector3 collision = Vector3(
				offsetX + currentZ * deltaX,
				offsetY + currentZ * deltaY,
				currentZ
			);
			float dist = Vector3::Distance(pos, collision);
			if (dist > maxDistance) break;
			
			cubes[dist] = {
				(int)floor(collision.x),
				(int)floor(collision.y),
				(int)(currentZ - ((dir.z > 0) ? 1 : 0)),
			};

			currentZ += dir.z > 0 ? 1 : -1;
		} while (true);
	}

	std::vector<std::array<int, 3>> result;
	for (auto& cube : cubes)
		result.push_back(cube.second);

	return result;
}

std::vector<Vector3> CollisionPoints = {
	{-0.3f, 0.5f, 0},
	{0, 0.5f, -0.3f},
	{0.3f, 0.5f, 0},
	{0, 0.5f, 0.3f},
	{-0.3f, 1.5f, 0},
	{0, 1.5f, -0.3f},
	{0.3f, 1.5f, 0},
	{0, 1.5f, 0.3f}
};

void Player::UpdatePlayer(double dt, const Keyboard::State& kb, const Mouse::State& ms) {
	m_kbTracker.Update(kb);
	m_msTracker.Update(ms);

	Vector3 delta = Vector3::Zero;
	if (kb.Z) delta += m_camera.Forward();
	if (kb.S) delta -= m_camera.Forward();
	if (kb.Q) delta -= m_camera.Right();
	if (kb.D) delta += m_camera.Right();
	
	delta.y = 0.0f;
	delta.Normalize();

	m_position += delta * m_speed * dt;

	// TODO physics
	m_velocity -= Vector3(0, m_gravity, 0) * dt;

	BlockId* block = m_world.GetCube(m_position.x, m_position.y + m_velocity.y, m_position.z);
	if (block) {
		if (*block != EMPTY) {
			m_velocity.y = 0.0f;
			m_position.y -= m_position.y - round(m_position.y);
			
			if (m_kbTracker.IsKeyPressed(Keyboard::Keys::Space))
				m_velocity.y = m_jumpForce;
		}
	}

	for (auto& collisionPoint : CollisionPoints) {
		const Vector3 colPos = m_position + m_velocity + collisionPoint;
		BlockId* block = m_world.GetCube(colPos.x, colPos.y, colPos.z + 1);

		if (block) {
			if (*block != EMPTY) {
				if (collisionPoint.z == 0)
					m_position.x -= colPos.x - round(colPos.x);
				else if (collisionPoint.x == 0)
					m_position.z -= colPos.z - round(colPos.z);
				else
					m_position.y -= colPos.y - round(colPos.y);
			}
		}
	}

	m_position += m_velocity;

	if (m_msTracker.leftButton == ButtonState::PRESSED) {
		const auto cubes = Raycast(m_camera.GetPosition(), m_camera.Forward(), 5);
		
		for (auto& cube : cubes) {
			BlockId* block = m_world.GetCube(cube[0], cube[1], cube[2]);
			if (block && *block != EMPTY) {
				m_world.SetCube(cube[0], cube[1], cube[2], EMPTY);
				m_world.regen = true; // TODO remove this
				break;
			}
		}
	}

	m_pitch -= ms.x * dt * m_cameraSensitivity;
	m_yaw -= ms.y * dt * m_cameraSensitivity;
	m_yaw = std::clamp(m_yaw, -1.4f, 1.4f);
	Quaternion rot = Quaternion::CreateFromAxisAngle(Vector3::Right, m_yaw);
	rot *= Quaternion::CreateFromAxisAngle(Vector3::Up, m_pitch);

	m_camera.SetPosition(m_position + Vector3(0, 1.5, 0));
	m_camera.SetRotation(rot);
}

void Player::UpdateFreeLook(double dt, const Keyboard::State& kb, const Mouse::State& ms) {
	Vector3 delta = Vector3::Zero;
	if (kb.Z) delta += m_camera.Forward();
	if (kb.S) delta -= m_camera.Forward();
	if (kb.Q) delta -= m_camera.Right();
	if (kb.D) delta += m_camera.Right();
	if (kb.Space) delta += m_camera.Up();
	if (kb.LeftShift) delta -= m_camera.Up();

	m_position = m_camera.GetPosition() + delta * m_cameraSpeed * dt;
	m_camera.SetPosition(m_position);

	m_pitch -= ms.x * dt * m_cameraSensitivity;
	m_yaw -= ms.y * dt * m_cameraSensitivity;
	m_yaw = std::clamp(m_yaw, -1.4f, 1.4f);

	Quaternion rot = Quaternion::CreateFromAxisAngle(Vector3::Right, m_yaw);
	rot *= Quaternion::CreateFromAxisAngle(Vector3::Up, m_pitch);
	m_camera.SetRotation(rot);
}

void Player::ShowImGui() {
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
