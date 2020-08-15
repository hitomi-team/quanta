#ifndef RENDERER_MATERIAL_H
#define RENDERER_MATERIAL_H

#include "subsystems/rhi.h"
#include "shader.h"
#include "texture2d.h"

namespace Renderer {

	// todo: multi-texture handling
	class Material {
	public:
		inline Material() { shader = nullptr; texture = nullptr; }
		inline virtual ~Material() {}

		// Buffers will have to be manipulated elsewhere.
		// ShaderParameterBuffer is setup internally
		bool Setup(Shader *shader, Texture2D *texture, ShaderParameterBuffer *paramBuffer);
		void Release();

		void Bind(RHI *rhi);

		inline ShaderParameterBuffer *getParamBuffer() { return paramBuffer; }

	protected:
		Shader *shader;
		Texture2D *texture;
		ShaderParameterBuffer *paramBuffer;
	};

}

#endif
