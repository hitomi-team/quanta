#ifndef RENDERER_MATERIALJSON_H
#define RENDERER_MATERIALJSON_H

#include "layer1/filesystem/runtime.h"
#include "layer1/renderer/runtime.h"

#include "stb_image.h"

namespace Renderer {

	// Material Loader - Supposed to be used outside of the renderer.
	// Filesystem service is required to parse textures.
	class MaterialJSON {
	public:
		MaterialJSON() {}

		Material *Load(Renderer::Runtime &rsRuntime, const std::string &jsonPath);
	
	private:
		Shader *parseShaderBytecode(Renderer::Runtime &rsRuntime, const std::string &vs_filePath, const std::string &fs_filePath);
		Texture2D *loadTexture(Renderer::Runtime &rsRuntime, const std::string &filePath);
	};

}

#endif
