#ifndef RENDERER_RUNTIME_H
#define RENDERER_RUNTIME_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "../graph/node.h"

namespace Renderer {

	// TODO: Write interface to use D3D and OpenGL renderer subsystems

	class Runtime : public Graph::Node {
	public:
		Runtime() : Node("RendererService") {  }
	};

}

#endif
