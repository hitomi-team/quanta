#ifndef RENDERER_MATERIAL_H
#define RENDERER_MATERIAL_H

#include "subsystems/rhi.h"
#include "shader.h"
#include "texture2d.h"

namespace Renderer {

	// todo: multi-texture handling
	class Material {
	public:
		Material() { shader = nullptr; texture = nullptr; }
		virtual ~Material() {}

		bool Setup(Shader *shader, Texture2D *texture);
		void Release();

		void Bind(RHI *rhi);

	protected:
		
		Shader *shader;
		Texture2D *texture;
	};

}

#endif
