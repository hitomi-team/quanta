#ifndef RENDERER_D3D11PARAMETERBUFFER_H
#define RENDERER_D3D11PARAMETERBUFFER_H

#include "d3d11required.h"
#include "layer1/renderer/material.h"

namespace Renderer {

	class D3D11ShaderParameterBuffer : public ShaderParameterBuffer {
	public:
		D3D11ShaderParameterBuffer() { mappedMemory = nullptr; constantBuffer = nullptr; }

		bool Setup(std::vector<ShaderParameterElement> elements);
		void Release();

		void Apply();
		void Map();
		void Flush();
		void Unmap();

		void SetShaderParameter(ShaderParameterUsage param, float value); // Map() must be called before this can be used.
		
		void SetShaderParameter(ShaderParameterUsage param, const glm::mat4& matrix);

	protected:
		unsigned int mappedMemorySize;
		char *mappedMemory;
		ID3D11Buffer *constantBuffer;
	};

}

#endif
