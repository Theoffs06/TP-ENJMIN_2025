#include "pch.h"
#include "World.h"

void World::Generate(const DeviceResources* devRes) {
	for (int z = 0; z < WORLD_SIZE_Z; ++z) {
		for (int y = 0; y < WORLD_SIZE_Y; ++y) {
			for (int x = 0; x < WORLD_SIZE_X; ++x) {
				m_chunks.emplace_back(Vector3(x, y, z)).Generate(devRes);
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