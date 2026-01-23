#pragma once

#include "Minicraft/World.h"
#include "Engine/Camera..h"

class Player {
	float m_cameraSpeed = 70.0f;
	float m_cameraSensitivity = 0.2f;

	float m_speed = 20.0f;
	float m_jumpForce = 20.0f;
	float m_gravity = 9.0f;

	Camera& m_camera;
	World& m_world;
public:
	Player(Camera& camera, World& world);
	void Update(double dt, const Keyboard::State& kb, const Mouse::State& ms) const;
	void ShowImGUI();
};