#include "pch.h"
#include "World.h"
#include "PerlinNoise.hpp"

float perlinScaleStone = 0.4f;
int perlinOctaveStone = 1;
float perlinHeightStone = 8.0f;
float perlinScaleDirt = 0.1f;
int perlinOctaveDirt = 1;
float perlinHeightDirt = 8.0f;


void World::Generate(const DeviceResources* devRes) {
	siv::BasicPerlinNoise<float> perlin;

	constexpr int GLOBAL_SIZE = WORLD_SIZE * Chunk::CHUNK_SIZE;

	for (int z = 0; z < GLOBAL_SIZE; ++z) {
		for (int x = 0; x < GLOBAL_SIZE; ++x) {
			int yStone = perlin.octave2D_01(x * perlinScaleStone, z * perlinScaleStone, perlinOctaveStone) * perlinHeightStone;
			int yDirt = yStone + perlin.octave2D_01(x * perlinScaleDirt, z * perlinScaleDirt, perlinOctaveDirt) * perlinHeightDirt;

			for (int y = 0; y < yStone; ++y) {
				
			}

			for (int y = 0; y < yDirt; ++y) {

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
	ImGui::End();

	ImGui::DragFloat("perlinScale", &perlinScaleStone);
	ImGui::DragInt("perlinScale", &perlinOctaveStone);
	ImGui::DragFloat("perlinScale", &perlinScaleStone);
	ImGui::DragFloat("perlinScale", &perlinScaleStone);
	ImGui::DragFloat("perlinScale", &perlinScaleStone);
	ImGui::DragFloat("perlinScale", &perlinScaleStone);
	ImGui::DragFloat("perlinScale", &perlinScaleStone);

	if (ImGui::Button("Generate!")) {
		Generate(devRes);
	}
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
	auto 
}
