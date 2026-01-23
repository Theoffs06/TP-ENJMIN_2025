#pragma once

using Microsoft::WRL::ComPtr;

template<typename TVertex>
class VertexBuffer {
	std::vector<TVertex> m_data;
	ComPtr<ID3D11Buffer> m_buffer;
public:
	uint32_t PushVertex(const TVertex& vtx) {
		m_data.push_back(vtx);
		return m_data.size() - 1;
	}

	void Clear() {
		m_data.clear();
	}

	void Create(const DeviceResources* deviceRes) {
		if (m_data.empty()) return;
		const CD3D11_BUFFER_DESC desc(
			sizeof(TVertex) * m_data.size(),
			D3D11_BIND_VERTEX_BUFFER
		);

		D3D11_SUBRESOURCE_DATA initialData = {};
		initialData.pSysMem = m_data.data();

		deviceRes->GetD3DDevice()->CreateBuffer(
			&desc,
			&initialData,
			m_buffer.ReleaseAndGetAddressOf()
		);
	}

	void Apply(const DeviceResources* deviceRes, const int slot = 0) const {
		ID3D11Buffer* vbs[] = { m_buffer.Get() };
		const UINT strides[] = { sizeof(TVertex) };
		constexpr UINT offsets[] = { 0 };

		deviceRes->GetD3DDeviceContext()->IASetVertexBuffers(
			slot, 
			1, 
			vbs, 
			strides, 
			offsets
		);
	}
};

class IndexBuffer {
	std::vector<uint32_t> m_data;
	ComPtr<ID3D11Buffer> m_buffer;
public:
	void PushTriangle(const uint32_t a, const uint32_t b, const uint32_t c) {
		m_data.push_back(a);
		m_data.push_back(b);
		m_data.push_back(c);
	}

	void Clear() {
		m_data.clear();
	}

	uint32_t Size() const {
		return (uint32_t) m_data.size();
	}

	void Create(const DeviceResources* deviceRes) {
		if (m_data.empty()) return;
		const CD3D11_BUFFER_DESC desc(
			sizeof(uint32_t) * m_data.size(),
			D3D11_BIND_INDEX_BUFFER
		);

		D3D11_SUBRESOURCE_DATA initialData = {};
		initialData.pSysMem = m_data.data();

		deviceRes->GetD3DDevice()->CreateBuffer(
			&desc,
			&initialData,
			m_buffer.ReleaseAndGetAddressOf()
		);
	}

	void Apply(const DeviceResources* deviceRes) const {
		deviceRes->GetD3DDeviceContext()->IASetIndexBuffer(
			m_buffer.Get(), 
			DXGI_FORMAT_R32_UINT, 
			0
		);
	}
};

template<typename  TData>
class ConstantBuffer {
	ComPtr<ID3D11Buffer> m_buffer;
public:
	TData data;

	void Create(const DeviceResources* deviceRes) {
		const CD3D11_BUFFER_DESC desc(
			sizeof(TData),
			D3D11_BIND_CONSTANT_BUFFER
		);

		deviceRes->GetD3DDevice()->CreateBuffer(
			&desc,
			nullptr, 
			m_buffer.ReleaseAndGetAddressOf()
		);
	}

	void Update(const DeviceResources* deviceRes) {
		deviceRes->GetD3DDeviceContext()->UpdateSubresource(
			m_buffer.Get(), 
			0, 
			nullptr, 
			&data, 
			0, 
			0
		);
	}

	void ApplyToVS(const DeviceResources* deviceRes, const int slot) const {
		ID3D11Buffer* cbs[] = { m_buffer.Get() };
		deviceRes->GetD3DDeviceContext()->VSSetConstantBuffers(slot, 1, cbs);
	}

	void ApplyToPS(const DeviceResources* deviceRes, const int slot) const {
		ID3D11Buffer* cbs[] = { m_buffer.Get() };
		deviceRes->GetD3DDeviceContext()->PSSetConstantBuffers(slot, 1, cbs);
	}
};