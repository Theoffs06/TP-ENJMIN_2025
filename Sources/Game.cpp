//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "PerlinNoise.hpp"
#include "Engine/VertexLayout.h"
#include "Engine/Shader.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

// Global stuff
Shader* basicShader;

ComPtr<ID3D11Buffer> vertexBuffer;
ComPtr<ID3D11Buffer> indexBuffer;

struct ModelData {
	Matrix mModel;
};
struct CameraData {
	Matrix mView;
	Matrix mProj;
};

Matrix mProjection;
ComPtr<ID3D11Buffer> cbModel;
ComPtr<ID3D11Buffer> cbCamera;

// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT, 2);
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game() {
	delete basicShader;
	g_inputLayouts.clear();
}

void Game::Initialize(HWND window, int width, int height) {
	// Create input devices
	m_gamePad = std::make_unique<GamePad>();
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);

	// Initialize the Direct3D resources
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	basicShader = new Shader(L"Basic");
	basicShader->Create(m_deviceResources.get());

	mProjection = Matrix::CreatePerspectiveFieldOfView(
		60 * XM_PI / 180.0f,
		(float)width / (float)height,
		0.01f, 
		100.0f
	);

	auto device = m_deviceResources->GetD3DDevice();

	GenerateInputLayout<VertexLayout_Position>(m_deviceResources.get(), basicShader);

	{ // VERTEX BUFFER INIT
		std::vector<VertexLayout_Position> vbData = {
			{{-0.5f,  0.5f, 0.0f}},
			{{ 0.5f, -0.5f, 0.0f}},
			{{-0.5f, -0.5f, 0.0f}},
			{{ 0.5f,  0.5f, 0.0f}}
		};
		CD3D11_BUFFER_DESC desc(
			sizeof(VertexLayout_Position) * vbData.size(),
			D3D11_BIND_VERTEX_BUFFER
		);
		D3D11_SUBRESOURCE_DATA initialData = {};
		initialData.pSysMem = vbData.data();

		device->CreateBuffer(
			&desc,
			&initialData,
			vertexBuffer.ReleaseAndGetAddressOf()
		);
	}

	{ // INDEX BUFFER INIT
		std::vector<uint32_t> ibData = {
			 0,  1, 2,
			 0,  3, 1,
		};
		CD3D11_BUFFER_DESC desc(
			sizeof(uint32_t) * ibData.size(),
			D3D11_BIND_INDEX_BUFFER
		);
		D3D11_SUBRESOURCE_DATA initialData = {};
		initialData.pSysMem = ibData.data();

		device->CreateBuffer(
			&desc,
			&initialData,
			indexBuffer.ReleaseAndGetAddressOf()
		);
	}

	{ // CONSTANT BUFFER MODEL INIT
		CD3D11_BUFFER_DESC desc(
			sizeof(ModelData),
			D3D11_BIND_CONSTANT_BUFFER
		);
		device->CreateBuffer(
			&desc,
			NULL,
			cbModel.ReleaseAndGetAddressOf()
		);
	}
	{ // CONSTANT BUFFER CAMERA INIT
		CD3D11_BUFFER_DESC desc(
			sizeof(CameraData),
			D3D11_BIND_CONSTANT_BUFFER
		);
		device->CreateBuffer(
			&desc,
			NULL,
			cbCamera.ReleaseAndGetAddressOf()
		);
	}
}

void Game::Tick() {
	// DX::StepTimer will compute the elapsed time and call Update() for us
	// We pass Update as a callback to Tick() because StepTimer can be set to a "fixed time" step mode, allowing us to call Update multiple time in a row if the framerate is too low (useful for physics stuffs)
	m_timer.Tick([&]() { Update(m_timer); });

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
	auto const kb = m_keyboard->GetState();
	auto const ms = m_mouse->GetState();
	
	// add kb/mouse interact here
	
	if (kb.Escape)
		ExitGame();

	auto const pad = m_gamePad->GetState(0);
}

// Draws the scene.
void Game::Render() {
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
		return;

	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();
	auto const viewport = m_deviceResources->GetScreenViewport();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	ApplyInputLayout<VertexLayout_Position>(m_deviceResources.get());

	basicShader->Apply(m_deviceResources.get());

	ID3D11Buffer* vbs[] = { vertexBuffer.Get() };
	UINT strides[] = { sizeof(VertexLayout_Position) };
	UINT offsets[] = { 0 };
	context->IASetVertexBuffers(0, 1, vbs, strides, offsets);

	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	ID3D11Buffer* cbs[] = { cbModel.Get(), cbCamera.Get() };
	context->VSSetConstantBuffers(0, 2, cbs);

	CameraData cameraData = {};
	cameraData.mView = Matrix::CreateLookAt(
		Vector3::Backward * 5,
		Vector3::Zero,
		Vector3::Up
	).Transpose();
	cameraData.mProj = mProjection.Transpose();
	context->UpdateSubresource(cbCamera.Get(), 0, NULL, &cameraData, 0, 0);

		ModelData modelData = {};
		Matrix model = Matrix::CreateRotationZ(m_timer.GetTotalSeconds());
		model *= Matrix::CreateTranslation(
			cos(m_timer.GetTotalSeconds()),
			sin(m_timer.GetTotalSeconds()),
			0);

		modelData.mModel = model.Transpose();
		context->UpdateSubresource(cbModel.Get(), 0, NULL, &modelData, 0, 0);

		context->DrawIndexed(6, 0, 0);

	// envoie nos commandes au GPU pour etre afficher � l'�cran
	m_deviceResources->Present();
}


#pragma region Message Handlers
void Game::OnActivated() {}

void Game::OnDeactivated() {}

void Game::OnSuspending() {}

void Game::OnResuming() {
	m_timer.ResetElapsedTime();
}

void Game::OnWindowMoved() {
	auto const r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange() {
	m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height) {
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	mProjection = Matrix::CreatePerspectiveFieldOfView(
		60 * XM_PI / 180.0f,
		(float)width / (float)height,
		0.01f,
		100.0f
	);
	// The windows size has changed:
	// We can realloc here any resources that depends on the target resolution (post processing etc)
}

void Game::OnDeviceLost() {
	// We have lost the graphics card, we should reset resources [TODO]
}

void Game::OnDeviceRestored() {
	// We have a new graphics card context, we should realloc resources [TODO]
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept {
	width = 800;
	height = 600;
}

#pragma endregion
