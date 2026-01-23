#pragma once

#include "Minicraft/Chunk.h"
#include "Minicraft/Block.h"
#include <array>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class World {
	constexpr static int WORLD_SIZE = 32;

	std::array<Chunk, WORLD_SIZE * WORLD_SIZE * WORLD_SIZE> m_chunks;

	struct ChunkData {
		Matrix mModel;
	};

	ConstantBuffer<ChunkData> m_cbModel;
public:
	void Generate();
	void CreateMesh(const DeviceResources* devRes);
	void Draw(const DeviceResources* devRes, ShaderPass shaderPass);
	void ShowImGui(const DeviceResources* devRes);

	BlockId* GetCube(int gx, int gy, int gz);
	void SetCube(int gx, int gy, int gz, BlockId id);
};