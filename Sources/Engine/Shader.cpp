#include "pch.h"

#include "Shader.h"

std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>> g_inputLayouts = {};

void Shader::Create(DeviceResources* deviceRes) {
	auto d3dDevice = deviceRes->GetD3DDevice();
	vsBytecode = DX::ReadData((std::wstring(L"Shaders/Compiled/") + shaderName + L"_vs.cso").c_str());
	psBytecode = DX::ReadData((std::wstring(L"Shaders/Compiled/") + shaderName + L"_ps.cso").c_str());

	d3dDevice->CreateVertexShader(vsBytecode.data(), vsBytecode.size(), nullptr, vertexShader.ReleaseAndGetAddressOf());
	d3dDevice->CreatePixelShader(psBytecode.data(), psBytecode.size(), nullptr, pixelShader.ReleaseAndGetAddressOf());
}

void Shader::Apply(DeviceResources* deviceRes) {
	auto d3dContext = deviceRes->GetD3DDeviceContext();
	d3dContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	d3dContext->PSSetShader(pixelShader.Get(), nullptr, 0);
}
