#include "pch/pch.h"

#include "d3d11parameterbuffer.h"

namespace Renderer {

	bool D3D11ShaderParameterBuffer::Setup(std::vector<ShaderParameterElement> elements)
	{
		if (!elements.size())
			return false;
		
		this->elements = elements;
		
		mappedMemorySize = 0;
		for (auto &i : this->elements) {
			if (i.dataSize < 16)
				i.dataSize = 16; // Cheaply align it to 16-bytes
			
			mappedMemorySize += i.dataSize;
		}

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = mappedMemorySize;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hr = d3d11_global_device->CreateBuffer(&bufferDesc, nullptr, &constantBuffer);
		if (D3D_FAILED(hr)) {
			Release();
			return false;
		}
		
		Map();
		Flush();
		Unmap();

		return true;
	}

	void D3D11ShaderParameterBuffer::Release()
	{
		if (mappedMemory) {
			d3d11_global_context->Unmap(constantBuffer, 0);
			mappedMemory = nullptr;
		}
		
		D3D_SAFE_RELEASE(constantBuffer);
	}

	void D3D11ShaderParameterBuffer::Apply()
	{
		d3d11_global_context->VSSetConstantBuffers(0, 1, &constantBuffer);
		d3d11_global_context->PSSetConstantBuffers(0, 1, &constantBuffer);
	}

	void D3D11ShaderParameterBuffer::Map()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		d3d11_global_context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		mappedMemory = (char *)mappedResource.pData;
	}
	
	void D3D11ShaderParameterBuffer::Flush()
	{
		size_t stride = 0;

		for (auto &i : elements) {
			memcpy(mappedMemory + stride, i.data, i.dataSize);
			stride += i.dataSize;
		}
	}

	void D3D11ShaderParameterBuffer::Unmap()
	{
		if (mappedMemory) {
			d3d11_global_context->Unmap(constantBuffer, 0);
			mappedMemory = nullptr;
		}
	}

	void D3D11ShaderParameterBuffer::SetShaderParameter(ShaderParameterUsage param, float value)
	{
		size_t stride = 0;
		for (auto &i : elements) {
			if (i.usage == param) {
				*(float *)i.data = value;
				memcpy(mappedMemory + stride, &value, i.dataSize);
				break;
			}
			stride += i.dataSize;
		}
	}

	void D3D11ShaderParameterBuffer::SetShaderParameter(ShaderParameterUsage param, const glm::mat4& matrix)
	{
		size_t stride = 0;
		for (auto &i : elements) {
			if (i.usage == param) {
				*(glm::mat4 *)i.data = matrix;
				memcpy(mappedMemory + stride, &matrix[0], i.dataSize);
				break;
			}
			stride += i.dataSize;
		}
	}

}

