#include "pch.h"
#include "World.h"
#include "PerlinNoise.hpp"

void World::Generate(const DeviceResources* devRes) {
	siv::BasicPerlinNoise<float> perlin;

	constexpr int GLOBAL_SIZE = WORLD_SIZE * Chunk::CHUNK_SIZE;

	for (int z = 0; z < GLOBAL_SIZE; z++) {
		for (int y = 0; y < GLOBAL_SIZE; y++) {
			for (int x = 0; x < GLOBAL_SIZE; x++) {
				const float test = perlin.octave3D_01(x / (float) GLOBAL_SIZE * 0.8f, y / (float) GLOBAL_SIZE * 0.8f, z / (float) GLOBAL_SIZE * 0.8f, 5);
				
				if (test >  0.3f && test < 0.6f) {
					*GetCube(x, y, z) = LOG;
				}
			}
		}
	}

	for (int z = 0; z < WORLD_SIZE; z++) {
		for (int y = 0; y < WORLD_SIZE; y++) {
			for (int x = 0; x < WORLD_SIZE; x++) {
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