#pragma once

#include "Minicraft/Chunk.h"

#define WORLD_SIZE_X 20
#define WORLD_SIZE_Y 1
#define WORLD_SIZE_Z 20
#define WORLD_SIZE WORLD_SIZE_X * WORLD_SIZE_Y * WORLD_SIZE_Z

using namespace DirectX;
using namespace DirectX::SimpleMath;

class World {
	std::vector<Chunk> m_chunks;

	struct ChunkData {
		Matrix mModel;
	};

	ConstantBuffer<ChunkData> m_cbModel;
public:
	void Generate(const DeviceResources* devRes);
	void Draw(const DeviceResources* devRes);
};