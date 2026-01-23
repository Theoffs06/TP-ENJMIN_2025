#pragma once

#include "Engine/Camera.h"

class World;

class Player {
	World* world;
	Camera camera = Camera(60, 1.0f);

	Vector3 position = Vector3(25, 25, 25);
	Vector3 velocity;

	float yaw;
	float pitch;

	Keyboard::KeyboardStateTracker kbTracker;
	Mouse::ButtonStateTracker msTracker;
public:
	void SetWorld(World* world) { this->world = world; }
	void Update(float dt, const Keyboard::State& kb, const Mouse::State& ms);

	Camera& GetCamera() { return camera; }
};