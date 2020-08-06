#ifndef RENDERER_RUNTIME_H
#define RENDERER_RUNTIME_H

#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "../graph/game.h"
#include "../log.h"
#include "imgui/imgui_impl_sdl.h"
#include "subsystems/rhi.h"
#include "material.h"

#if defined(__VULKAN)
#include "subsystems/vulkan/vulkanrenderer.h"
#elif defined(__D3D11)
#include "subsystems/d3d11/d3d11renderer.h"
#else
#error A renderer must be defined
#endif

namespace Renderer {

	class Runtime : public Graph::Service {
	public:
		Runtime();

		// overridden service functions
		bool Setup();
		bool Update();
		void Release();

		void SetRenderer(RHI *rhi);
	
		void __debug_menu();

	private:
		RHI *rhi;

		std::vector<Material> materials;
	};

}

#endif
