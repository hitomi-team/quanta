#ifndef RENDERER_RUNTIME_H
#define RENDERER_RUNTIME_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "../graph/service.h"
#include "subsystems/rhi.h"

namespace Renderer {

	// TODO: Write interface to use D3D and OpenGL renderer subsystems

	class Runtime : public Graph::Service {
	public:
		Runtime();

		// overridden service functions
		bool Setup();
		bool Update();
		void Release();

		void SetRenderer(RHI *rhi);
	
	private:
		RHI *rhi;
	};

}

#endif
