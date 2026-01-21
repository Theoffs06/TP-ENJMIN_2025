#pragma once

#include "Block.h"
#include "Cube.h"
#include <array>

class World {
	constexpr static int WORLD_SIZE = 16;
	std::array<BlockId, WORLD_SIZE * WORLD_SIZE * WORLD_SIZE> data;
	std::vector<Cube> cubes;

	struct CubeData {
		Matrix mModel;
	};
	ConstantBuffer<CubeData> cbModel;
public:
	void Generate(DeviceResources* res);
	void Draw(DeviceResources* res);

	BlockId* GetCube(int gx, int gy, int gz);
	void SetCube(int gx, int gy, int gz, BlockId id);
};