#include "pch/pch.h"

#include "nullrenderer.h"

namespace Renderer {

	bool NullRenderer::SetGraphicsMode(int width, int height, bool fullscreen, bool borderless, bool resizable, bool vsync, int multisample)
	{
		(void)width;
		(void)height;
		(void)fullscreen;
		(void)borderless;
		(void)resizable;
		(void)vsync;
		(void)multisample;
		return true;
	}

	bool NullRenderer::BeginFrame()
	{
		return true;
	}

	void NullRenderer::EndFrame()
	{
	}

	void NullRenderer::Clear(unsigned flags, const glm::vec4& color, float depth, unsigned stencil)
	{
		(void)flags;
		(void)color;
		(void)depth;
		(void)stencil;
	}


	glm::vec2 NullRenderer::GetRenderTargetDimensions()
	{
		return glm::vec2(0, 0);
	}

	void NullRenderer::CreateRendererCapabilities()
	{
	}

	std::vector<int> NullRenderer::GetMultiSampleLevels()
	{
		std::vector< int > a;
		return a;
	}


	void NullRenderer::Close()
	{
	}


	void NullRenderer::AddGpuResource(GPUResource* object)
	{
		(void)object;
	}

	void NullRenderer::RemoveGpuResource(GPUResource* object)
	{
		(void)object;
	}


	void NullRenderer::ResetCache()
	{
	}


	bool NullRenderer::IsDeviceLost()
	{
		return false;
	}


	VertexBuffer* NullRenderer::CreateVertexBuffer(const Vertex *vertices, unsigned count)
	{
		(void)vertices;
		(void)count;
		return nullptr;
	}

	IndexBuffer* NullRenderer::CreateIndexBuffer(const uint16_t *indices, unsigned count)
	{
		(void)indices;
		(void)count;
		return nullptr;
	}

	InputLayout* NullRenderer::CreateInputLayout(unsigned char *vsbytecode, unsigned vsbytecodelen)
	{
		(void)vsbytecode;
		(void)vsbytecodelen;
		return nullptr;
	}

	Shader *NullRenderer::CreateShader(unsigned char *vs_bytecode, unsigned int vs_size,
				     unsigned char *fs_bytecode, unsigned int fs_size)
	{
		(void)vs_bytecode;
		(void)vs_size;
		(void)fs_bytecode;
		(void)fs_size;
		return nullptr;
	}

	Texture2D *NullRenderer::CreateTexture2D(unsigned char *data, unsigned width, unsigned height, SamplerStateDesc samplerstatedesc)
	{
		(void)data;
		(void)width;
		(void)height;
		(void)samplerstatedesc;
		return nullptr;
	}

	ShaderParameterBuffer *NullRenderer::CreateShaderParameterBuffer(std::vector<ShaderParameterElement> elements)
	{
		(void)elements;
		return nullptr;
	}

	void NullRenderer::SetShaders(Shader *shader)
	{
		(void)shader;
	}

	void NullRenderer::SetVertexBuffer(VertexBuffer* buffer)
	{
		(void)buffer;
	}

	void NullRenderer::SetIndexBuffer(IndexBuffer* buffer)
	{
		(void)buffer;
	}

	bool NullRenderer::SetVertexBuffers(const std::vector<VertexBuffer*>& buffers, const std::vector<unsigned>& elementMasks, unsigned instanceOffset)
	{
		(void)buffers;
		(void)elementMasks;
		(void)instanceOffset;
		return true;
	}


	void NullRenderer::SetFlushGPU(bool flushGpu)
	{
		(void)flushGpu;
	}

	void NullRenderer::SetBlendMode(BlendMode mode)
	{
		(void)mode;
	}

	void NullRenderer::SetColorWrite(bool enable)
	{
		(void)enable;
	}

	void NullRenderer::SetCullMode(CullMode mode)
	{
		(void)mode;
	}

	void NullRenderer::SetDepthBias(float constantBias, float slopeScaledBias)
	{
		(void)constantBias;
		(void)slopeScaledBias;
	}

	void NullRenderer::SetDepthTest(CompareMode mode)
	{
		(void)mode;
	}

	void NullRenderer::SetDepthWrite(bool enable)
	{
		(void)enable;
	}

	void NullRenderer::SetFillMode(FillMode mode)
	{
		(void)mode;
	}

	void NullRenderer::SetScissorTest(bool enable, const glm::vec2& rect)
	{
		(void)enable;
		(void)rect;
	}

	void NullRenderer::SetStencilTest(bool enable, CompareMode mode, StencilOp pass, StencilOp fail, StencilOp zFail, unsigned stencilRef, unsigned compareMask, unsigned writeMask)
	{
		(void)enable;
		(void)mode;
		(void)pass;
		(void)fail;
		(void)zFail;
		(void)stencilRef;
		(void)compareMask;
		(void)writeMask;
	}


	void NullRenderer::SetTexture(unsigned index, Texture2D* texture)
	{
		(void)index;
		(void)texture;
	}


	void NullRenderer::SetRenderTarget(unsigned index, RenderTarget* renderTarget)
	{
		(void)index;
		(void)renderTarget;
	}

	void NullRenderer::SetDepthStencil(RenderTarget* depthStencil)
	{
		(void)depthStencil;
	}


	void NullRenderer::SetViewport(const glm::vec4& rect)
	{
		(void)rect;
	}


	void NullRenderer::Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount)
	{
		(void)type;
		(void)vertexStart;
		(void)vertexCount;
	}

	void NullRenderer::DrawIndexed(PrimitiveType type, unsigned indexStart, unsigned indexCount)
	{
		(void)type;
		(void)indexStart;
		(void)indexCount;
	}

	void NullRenderer::DrawInstanced(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned instanceCount)
	{
		(void)type;
		(void)indexStart;
		(void)indexCount;
		(void)instanceCount;
	}

	SDL_Window *NullRenderer::GetWindow()
	{
		return nullptr;
	}


	void NullRenderer::ImGuiNewFrame()
	{
	}

	void NullRenderer::ImGuiEndFrame()
	{
	}

	RendererType NullRenderer::getRendererType()
	{
		return RENDERER_NULL;
	}

	void NullRenderer::WaitForDevice()
	{
	}

}
