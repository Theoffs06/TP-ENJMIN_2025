#pragma once

#include "Minicraft/World.h"
#include "Engine/Camera.h"

class World;

class Player {
	float m_cameraSpeed = 70.0f;
	float m_cameraSensitivity = 0.2f;
	float m_pitch = 0;
	float m_yaw = 0;

	float m_speed = 10.0f;
	float m_jumpForce = 0.3f;
	float m_gravity = 0.8f;
	
	Vector3 m_position;
	Vector3 m_velocity;

	Keyboard::KeyboardStateTracker m_kbTracker;
	Mouse::ButtonStateTracker m_msTracker;

	Camera& m_camera;
	World& m_world;
public:
	Player(Camera& camera, World& world);
	void UpdatePlayer(double dt, const Keyboard::State& kb, const Mouse::State& ms);
	void UpdateFreeLook(double dt, const Keyboard::State& kb, const Mouse::State& ms);
	void ShowImGui();
};