#include "pch.h"

#include "World.h"

using namespace DirectX::SimpleMath;

void World::Generate(DeviceResources* res) {
	for (int z = 0; z < WORLD_SIZE; z++) {
		for (int x = 0; x < WORLD_SIZE; x++) {
			for (int y = 0; y < 3; y++) {
				auto blockId = GetCube(x, y, z);
				*blockId = STONE;
			}

			for (int y = 3; y < 6; y++) {
				auto blockId = GetCube(x, y, z);
				*blockId = DIRT;
			}

			{
				auto blockId = GetCube(x, 6, z);
				*blockId = GRASS;
			}
		}
	}

	for (int z = 0; z < WORLD_SIZE; z++) {
		for (int y = 0; y < WORLD_SIZE; y++) {
			for (int x = 0; x < WORLD_SIZE; x++) {
				auto blockId = GetCube(x, y, z);
				if (blockId == nullptr || *blockId == EMPTY) continue;
				cubes.emplace_back(Vector3( (float)x, (float)y, (float)z ), *blockId).Generate(res);
			}
		}
	}
	cbModel.Create(res);
}

void World::Draw(DeviceResources* res) {
	cbModel.ApplyToVS(res, 0);

	for (auto& cube : cubes) {
		Matrix model = cube.GetLocalMatrix();
		cbModel.data.mModel = model.Transpose();
		cbModel.Update(res);
		cube.Draw(res);
	}
}

BlockId* World::GetCube(int gx, int gy, int gz) {
	if (gx < 0) return nullptr;
	if (gy < 0) return nullptr;
	if (gz < 0) return nullptr;
	if (gx >= WORLD_SIZE) return nullptr;
	if (gy >= WORLD_SIZE) return nullptr;
	if (gz >= WORLD_SIZE) return nullptr;
	return &data[gx + gy * WORLD_SIZE + gz * WORLD_SIZE * WORLD_SIZE];
}
