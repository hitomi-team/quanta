#ifndef RENDERER_MATERIAL_H
#define RENDERER_MATERIAL_H

#include "shader.h"
#include "texture.h"

namespace Renderer {

	class Material {
	public:
		Material() {}
		virtual ~Material() {}

		void SetShader();
		void SetTexture();
	};

}

#endif
