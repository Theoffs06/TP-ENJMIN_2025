#include "pch.h"
#include "World.h"

void World::Generate(const DeviceResources* devRes) {
	for (int z = 0; z < WORLD_SIZE_Z; ++z) {
		for (int x = 0; x < WORLD_SIZE_X; ++x) {

		}
	}

	for (int z = 0; z < WORLD_SIZE_Z; ++z) {
		for (int y = 0; y < WORLD_SIZE_Y; ++y) {
			for (int x = 0; x < WORLD_SIZE_X; ++x) {
				const auto blockId = GetCube(x, y, z);
				if (blockId == nullptr || *blockId == EMPTY) continue;
				m_cubes.emplace_back(Vector3(x, y, z), *blockId).Generate(devRes);
			}
		}
	}

	m_cbModel.Create(devRes);
}

void World::Draw(const DeviceResources* devRes) {
	m_cbModel.ApplyToVS(devRes, 0);

	for (const auto& cube : m_cubes) {
		const Matrix model = cube.GetLocalMatrix();
		m_cbModel.data.mModel = model.Transpose();
		m_cbModel.Update(devRes);

		cube.Draw(devRes);
	}
}

BlockId* World::GetCube(int gx, int gy, int gz) {
	if (gx < 0 || gy < 0 || gz < 0) return nullptr;
	if (gx >= WORLD_SIZE_X || gy >= WORLD_SIZE_Y || gz >= WORLD_SIZE_Z) return nullptr;
	return &m_data[gx + gy * WORLD_SIZE_Y + gz * WORLD_SIZE_Z * WORLD_SIZE_Z];
}