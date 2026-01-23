#include "pch.h"
#include "World.h"
#include "PerlinNoise.hpp"
#include "Engine/Random.h"

// WORLD GENERATION PARAMETERS
int seed = 192006;

// Stone Parameters
float perlinScaleStone = 0.005f;
int perlinOctaveStone = 3;
float perlinPersistenceStone = 0.5;
float perlinHeightStone = 50.0f;

// Dirt Parameters
float perlinScaleDirt = 0.07f;
int perlinOctaveDirt = 1;
float perlinPersistenceDirt = 0.5;
float perlinHeightDirt = 8.0f;

// Cave Parameters
float perlinScaleCave = 0.005f;
int perlinOctaveCave = 3;
float perlinPersistenceCave = 0.5;

// Ores Parameters
float coalChance = 0.1f;
float ironChance = 0.01f;
float goldChance = 0.005f;
float redstoneChance = 0.003f;
float diamondChance = 0.001f;

// Tree Parameters
int treeMinSize = 4;
int treeMaxSize = 8;
float treeChance = 0.001f;

// Others Parameters
float waterHeight = 15.0f;

void World::Generate(const DeviceResources* devRes) {
	constexpr siv::BasicPerlinNoise<float> perlin;
	constexpr int GLOBAL_SIZE = WORLD_SIZE * Chunk::CHUNK_SIZE;

	Random::SetSeed(seed);
	const float randomOffset = Random::RangeFloat(-500.0f, 500.0f);

	for (int z = 0; z < GLOBAL_SIZE; ++z) {
		for (int x = 0; x < GLOBAL_SIZE; ++x) {
			for (int y = 0; y < GLOBAL_SIZE; ++y) {
				SetCube(x, y, z, EMPTY);
			}
		}
	}

	for (int z = 0; z < GLOBAL_SIZE; ++z) {
		for (int x = 0; x < GLOBAL_SIZE; ++x) {
			const int yStone = perlin.octave2D_01(
				x * perlinScaleStone + randomOffset, 
				z * perlinScaleStone + randomOffset, 
				perlinOctaveStone, 
				perlinPersistenceStone
			) * perlinHeightStone;

			const int yDirt = yStone + perlin.octave2D_01(
				x * perlinScaleDirt + randomOffset,
				z * perlinScaleDirt + randomOffset, 
				perlinOctaveDirt, 
				perlinPersistenceDirt
			) * perlinHeightDirt;

			for (int y = 0; y < yStone; ++y) {
				if (Random::Chance(diamondChance)) SetCube(x, y, z, DIAMOND_ORE);
				else if (Random::Chance(redstoneChance)) SetCube(x, y, z, REDSTONE_ORE);
				else if (Random::Chance(goldChance)) SetCube(x, y, z, GOLD_ORE);
				else if (Random::Chance(ironChance)) SetCube(x, y, z, IRON_ORE);
				else if (Random::Chance(coalChance)) SetCube(x, y, z, COAL);
				else SetCube(x, y, z, STONE);
			}

			for (int y = yStone; y < yDirt; ++y) {
				SetCube(x, y, z, DIRT);
			}

			if (yDirt < waterHeight) {
				SetCube(x, yDirt, z, SAND);

				for (int y = yDirt + 1; y < waterHeight; ++y) {
					SetCube(x, y, z, WATER);
				}
			}
			else if (yDirt == waterHeight || yDirt == waterHeight - 1) {
				SetCube(x, yDirt, z, SAND);
			}
			else {
				SetCube(x, yDirt, z, GRASS);

				if (Random::Chance(treeChance)) {
					const int treeSize = Random::RangeInt(treeMinSize, treeMaxSize);

					for (int y = yDirt + 1; y < yDirt + treeSize; ++y) {
						SetCube(x, y, z, LOG);
					}

					for (int y = yDirt + treeSize; y < yDirt + treeSize + 3; ++y) {
						SetCube(x, y, z, TNT);
						SetCube(x + 1, y, z, TNT);
						SetCube(x - 1, y, z, TNT);
						SetCube(x, y, z + 1, TNT);
						SetCube(x, y, z - 1, TNT);
						SetCube(x + 1, y, z + 1, TNT);
						SetCube(x - 1, y, z - 1, TNT);
						SetCube(x + 1, y, z - 1, TNT);
						SetCube(x - 1, y, z + 1, TNT);
					}
				}
			}
		}
	}

	for (int z = 0; z < GLOBAL_SIZE; ++z) {
		for (int x = 0; x < GLOBAL_SIZE; ++x) {
			for (int y = 0; y < GLOBAL_SIZE; ++y) {
				const float perlinCave = perlin.octave3D_01(
					x * perlinScaleCave + randomOffset / (float) GLOBAL_SIZE, 
					y * perlinScaleCave + randomOffset / (float) GLOBAL_SIZE, 
					z * perlinScaleStone + randomOffset / (float) GLOBAL_SIZE,
					perlinOctaveCave,
					perlinPersistenceCave
				);
				
				if (perlinCave > 0.3f && perlinCave < 0.6f) {
					SetCube(x, y, z, EMPTY);
				}
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

void World::Draw(const DeviceResources* devRes, const ShaderPass shaderPass) {
	m_cbModel.ApplyToVS(devRes, 0);

	for (const auto& chunk : m_chunks) {
		const Matrix model = chunk.GetLocalMatrix();
		m_cbModel.data.mModel = model.Transpose();
		m_cbModel.Update(devRes);
		chunk.Draw(devRes, shaderPass);
	}
}

void World::ShowImGui(const DeviceResources* devRes) {
	ImGui::Begin("World gen");

	ImGui::InputInt("Seed", &seed);

	if (ImGui::Button("Automatic Seed"))
		seed = std::random_device{}();

	if (ImGui::CollapsingHeader("Stone Settings")) {
		ImGui::DragFloat("Perlin Scale Stone", &perlinScaleStone, 0.01f, 0);
		ImGui::DragInt("Perlin Octave Stone", &perlinOctaveStone, 0.1f, 0);
		ImGui::DragFloat("Perlin Persistence Stone", &perlinPersistenceStone, 0.1f, 0, 1);
		ImGui::DragFloat("Perlin Height Stone", &perlinHeightStone, 0.1f, 0);
	}

	if (ImGui::CollapsingHeader("Dirt Settings")) {
		ImGui::DragFloat("Perlin Scale Dirt", &perlinScaleDirt, 0.01f, 0);
		ImGui::DragInt("Perlin Octave Dirt", &perlinOctaveDirt, 0.1f, 0);
		ImGui::DragFloat("Perlin Persistence Dirt", &perlinPersistenceDirt, 0.1f, 0, 1);
		ImGui::DragFloat("Perlin Height Dirt", &perlinHeightDirt, 0.1f, 0);
	}

	if (ImGui::CollapsingHeader("Cave Settings")) {
		ImGui::DragFloat("Perlin Scale Cave", &perlinScaleCave, 0.01f, 0);
		ImGui::DragInt("Perlin Octave Cave", &perlinOctaveCave, 0.1f, 0);
		ImGui::DragFloat("Perlin Persistence Cave", &perlinPersistenceCave, 0.1f, 0, 1);
	}

	if (ImGui::CollapsingHeader("Ores Settings")) {
		ImGui::DragFloat("Coal Chance", &coalChance, 0.1f, 0, 1);
		ImGui::DragFloat("Iron Chance", &ironChance, 0.1f, 0, 1);
		ImGui::DragFloat("Gold Chance", &goldChance, 0.1f, 0, 1);
		ImGui::DragFloat("Redstone Chance", &redstoneChance, 0.1f, 0, 1);
		ImGui::DragFloat("Diamond Chance", &diamondChance, 0.1f, 0, 1);
	}

	if (ImGui::CollapsingHeader("Tree Settings")) {
		ImGui::DragInt("Tree Min Height", &treeMinSize, 0.1f, 0, 10);
		ImGui::DragInt("Tree Max Height", &treeMaxSize, 0.1f, treeMinSize, 10);
		ImGui::DragFloat("Tree Chance", &treeChance, 0.1f, 0, 1);
	}

	if (ImGui::CollapsingHeader("Others Settings")) {
		ImGui::DragFloat("Water Height", &waterHeight, 0.1f, 0);
	}

	if (ImGui::Button("Generate!")) 
		Generate(devRes);

	ImGui::End();
}

BlockId* World::GetCube(int gx, int gy, int gz) {
	const int cx = gx / Chunk::CHUNK_SIZE;
	const int cy = gy / Chunk::CHUNK_SIZE;
	const int cz = gz / Chunk::CHUNK_SIZE;
	const int lx = gx % Chunk::CHUNK_SIZE;
	const int ly = gy % Chunk::CHUNK_SIZE;
	const int lz = gz % Chunk::CHUNK_SIZE;

	if (cx < 0 || cy < 0 || cz < 0) return nullptr;
	if (cx >= WORLD_SIZE || cy >= WORLD_SIZE || cz >= WORLD_SIZE) return nullptr;

	return m_chunks[cx + cy * WORLD_SIZE + cz * WORLD_SIZE * WORLD_SIZE].GetChunkCube(lx, ly, lz);
}

void World::SetCube(int gx, int gy, int gz, BlockId id) {
	const auto cube = GetCube(gx, gy, gz);
	if (!cube) return;
	*cube = id;
}