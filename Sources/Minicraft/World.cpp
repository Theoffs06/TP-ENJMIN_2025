#include "pch.h"
#include "World.h"
#include "PerlinNoise.hpp"

// World generation parameters
int seed = 192006;

float perlinScaleStone = 0.02f;
int perlinOctaveStone = 4;
float perlinHeightStone = 14.0f;
float perlinPersistenceStone = 0.5;

float perlinScaleDirt = 0.07f;
int perlinOctaveDirt = 2;
float perlinHeightDirt = 8.0f;
float perlinPersistenceDirt = 0.5;

float waterHeight = 14.0f;

void World::Generate(const DeviceResources* devRes) {
	siv::BasicPerlinNoise<float> perlin;
	
	const int GLOBAL_SIZE = WORLD_SIZE * Chunk::CHUNK_SIZE;

	for (int z = 0; z < GLOBAL_SIZE; ++z) {
		for (int x = 0; x < GLOBAL_SIZE; ++x) {
			for (int y = 0; y < GLOBAL_SIZE; ++y)
				SetCube(x, y, z, EMPTY);

			const int yStone = perlin.octave2D_01(x * perlinScaleStone, z * perlinScaleStone, perlinOctaveStone, perlinPersistenceStone) * perlinHeightStone;
			const int yDirt = yStone + perlin.octave2D_01(x * perlinScaleDirt, z * perlinScaleDirt, perlinOctaveDirt, perlinPersistenceDirt) * perlinHeightDirt;

			for (int y = 0; y < yStone; ++y) {
				SetCube(x, y, z, STONE);
			}

			for (int y = yStone; y < yDirt; ++y) {
				SetCube(x, y, z, DIRT);
			}

			if (yDirt + 1 < waterHeight) {
				for (int y = yDirt; y < waterHeight; ++y) {
					SetCube(x, y, z, WATER);
				}
			}
			else {
				SetCube(x, yDirt, z, GRASS);
			}
		}
	}

	for (int z = 0; z < WORLD_SIZE; ++z) {
		for (int y = 0; y < WORLD_SIZE; ++y) {
			for (int x = 0; x < WORLD_SIZE; ++x) {
				m_chunks[x + y * WORLD_SIZE + z * WORLD_SIZE * WORLD_SIZE].SetPosition(this, x, y, z);
				m_chunks[x + y * WORLD_SIZE + z * WORLD_SIZE * WORLD_SIZE].Generate(devRes);
			}
		}
	}

	m_cbModel.Create(devRes);
}

void World::Draw(const DeviceResources* devRes) {
	m_cbModel.ApplyToVS(devRes, 0);

	for (const auto& chunk : m_chunks) {
		const Matrix model = chunk.GetLocalMatrix();
		m_cbModel.data.mModel = model.Transpose();
		m_cbModel.Update(devRes);
		chunk.Draw(devRes);
	}
}

void World::ShowImGui(const DeviceResources* devRes) {
	ImGui::Begin("World gen");

	ImGui::InputInt("seed", &seed);

	ImGui::Text("Stone Settings");
	ImGui::DragFloat("perlinScaleStone", &perlinScaleStone, 0.01f);
	ImGui::DragInt("perlinOctaveStone", &perlinOctaveStone, 0.1f);
	ImGui::DragFloat("perlinPersistenceStone", &perlinPersistenceStone, 0.1f, 0, 1);
	ImGui::DragFloat("perlinHeightStone", &perlinHeightStone, 0.1f);
	
	ImGui::Text("Dirt Settings");
	ImGui::DragFloat("perlinScaleDirt", &perlinScaleDirt, 0.01f);
	ImGui::DragInt("perlinOctaveDirt", &perlinOctaveDirt, 0.1f);
	ImGui::DragFloat("perlinPersistenceDirt", &perlinPersistenceDirt, 0.1f, 0, 1);
	ImGui::DragFloat("perlinHeightDirt", &perlinHeightDirt, 0.1f);

	ImGui::Text("Others Settings");
	ImGui::DragFloat("waterHeight", &waterHeight, 0.1f);

	if (ImGui::Button("Generate!"))
		Generate(devRes);

	ImGui::End();
}


BlockId* World::GetCube(int gx, int gy, int gz) {
	const int cx = gx / Chunk::CHUNK_SIZE;
	const int cy = gy / Chunk::CHUNK_SIZE;
	const int cz = gz / Chunk::CHUNK_SIZE;
	int lx = gx % Chunk::CHUNK_SIZE;
	int ly = gy % Chunk::CHUNK_SIZE;
	int lz = gz % Chunk::CHUNK_SIZE;

	if (cx < 0 || cy < 0 || cz < 0) return nullptr;
	if (cx >= WORLD_SIZE || cy >= WORLD_SIZE || cz >= WORLD_SIZE) return nullptr;

	return m_chunks[cx + cy * WORLD_SIZE + cz * WORLD_SIZE * WORLD_SIZE].GetChunkCube(lx, ly, lz);
}

void World::SetCube(int gx, int gy, int gz, BlockId id) {
	auto cube = GetCube(gx, gy, gz);
	if (!cube) return;
	*cube = id;
}