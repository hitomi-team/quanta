#ifndef RENDERER_RUNTIME_H
#define RENDERER_RUNTIME_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "imgui/imgui_impl_sdl.h"
#include "../graph/game.h"
#include "subsystems/rhi.h"

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
	};

}

#endif
