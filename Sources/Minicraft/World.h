#pragma once

#include <array>

#include "Minicraft/Cube.h"
#include "Minicraft/Block.h"

#define WORLD_SIZE_X 16
#define WORLD_SIZE_Y 16
#define WORLD_SIZE_Z 16
#define WORLD_SIZE WORLD_SIZE_X * WORLD_SIZE_Y * WORLD_SIZE_Z

using namespace DirectX;
using namespace DirectX::SimpleMath;

class World {
	std::array<BlockId, WORLD_SIZE> m_data;
	std::vector<Cube> m_cubes;

	struct CubeData {
		Matrix mModel;
	};

	ConstantBuffer<CubeData> m_cbModel;
public:
	void Generate(const DeviceResources* devRes);
	void Draw(const DeviceResources* devRes);

	BlockId* GetCube(int gx, int gy, int gz);
};
