#pragma once

#include "Block.h"
#include "Cube.h"
#include "Chunk.h"
#include <array>

class World {
	constexpr static int WORLD_SIZE = 16;
	std::array<Chunk, WORLD_SIZE * WORLD_SIZE * WORLD_SIZE> chunks;

	struct CubeData {
		Matrix mModel;
	};
	ConstantBuffer<CubeData> cbModel;
public:
	bool regen;
	void Generate();
	void CreateMesh(DeviceResources* res);
	void Draw(DeviceResources* res, ShaderPass pass);

	BlockId* GetCube(int gx, int gy, int gz);
	void SetCube(int gx, int gy, int gz, BlockId id);

	void ShowImGui(DeviceResources* res);
};