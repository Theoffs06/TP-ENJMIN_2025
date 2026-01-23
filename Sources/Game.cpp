//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "PerlinNoise.hpp"
#include "Engine/VertexLayout.h"
#include "Engine/Shader.h"
#include "Engine/Texture.h"
#include "Engine/Camera..h"
#include "Minicraft/Player.h"
#include "Minicraft/World.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

// Global stuff
Shader basicShader(L"basic");
Texture terrain(L"terrain");
Camera camera(60, 1.0);
World world;

Player player(camera, world);


Shader lineShader(L"Line");
VertexBuffer<VertexLayout_PositionColor> debugLine;

// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, DXGI_FORMAT_D32_FLOAT, 2);
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	g_inputLayouts.clear();
}

void Game::Initialize(HWND window, int width, int height) {
	// Create input devices
	m_gamePad = std::make_unique<GamePad>();
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
	m_mouse->SetMode(Mouse::MODE_RELATIVE);

	// Initialize the Direct3D resources
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	basicShader.Create(m_deviceResources.get());

	camera.UpdateAspectRatio((float) width / (float) height);
	camera.Create(m_deviceResources.get());

	m_commonStates = std::make_unique<CommonStates>(m_deviceResources->GetD3DDevice());

	GenerateInputLayout<VertexLayout_PositionNormalUV>(m_deviceResources.get(), &basicShader);

	lineShader.Create(m_deviceResources.get());
	GenerateInputLayout<VertexLayout_PositionColor>(m_deviceResources.get(), &lineShader);
	debugLine.PushVertex(VertexLayout_PositionColor({ 0, 0, 0 }, { 1,0,0,1 }));
	debugLine.PushVertex(VertexLayout_PositionColor({ 10, 10, 10 }, { 1,0,0,1 }));
	debugLine.Create(m_deviceResources.get());

	world.Generate(m_deviceResources.get());
	terrain.Create(m_deviceResources.get());

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // IF using Docking Branch

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());
}

void Game::Tick() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// DX::StepTimer will compute the elapsed time and call Update() for us
	// We pass Update as a callback to Tick() because StepTimer can be set to a "fixed time" step mode, allowing us to call Update multiple time in a row if the framerate is too low (useful for physics stuffs)
	m_timer.Tick([&]() { Update(m_timer); });

	Render();
}

bool imGuiMode = false;

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
	auto const kb = m_keyboard->GetState();
	auto const ms = m_mouse->GetState();
	auto const pad = m_gamePad->GetState(0);
	const double dt = timer.GetElapsedSeconds();

	if (kb.Escape) ExitGame();
	if (kb.IsKeyDown(Keyboard::P)) imGuiMode = true;
	if (kb.IsKeyDown(Keyboard::M)) imGuiMode = false;

	if (imGuiMode) {
		m_mouse->SetMode(Mouse::MODE_ABSOLUTE);
		world.ShowImGui(m_deviceResources.get());
		player.ShowImGUI();
	} 
	else {
		m_mouse->SetMode(Mouse::MODE_RELATIVE);
		player.Update(dt, kb, ms);
	}
}

// Draws the scene.
void Game::Render() {
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
		return;

	const auto context = m_deviceResources->GetD3DDeviceContext();
	const auto renderTarget = m_deviceResources->GetRenderTargetView();
	const auto depthStencil = m_deviceResources->GetDepthStencilView();
	auto const viewport = m_deviceResources->GetScreenViewport();

	context->ClearRenderTargetView(renderTarget, ColorsLinear::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	ApplyInputLayout<VertexLayout_PositionNormalUV>(m_deviceResources.get());

	basicShader.Apply(m_deviceResources.get());
	terrain.Apply(m_deviceResources.get());
	camera.Apply(m_deviceResources.get());

	context->OMSetBlendState(m_commonStates->Opaque(), nullptr, 0xffffffff);
	world.Draw(m_deviceResources.get(), SP_OPAQUE);
	
	context->OMSetBlendState(m_commonStates->AlphaBlend(), nullptr, 0xffffffff);
	world.Draw(m_deviceResources.get(), SP_TRANSPARENT);

	context->OMSetBlendState(m_commonStates->Opaque(), NULL, 0xffffffff);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	ApplyInputLayout<VertexLayout_PositionColor>(m_deviceResources.get());
	lineShader.Apply(m_deviceResources.get());
	debugLine.Apply(m_deviceResources.get());
	context->Draw(2, 0);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

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

	camera.UpdateAspectRatio((float) width / (float) height);

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
