#include <string>
#include <stdio.h>


#include "renderer/runtime.h"
#include "renderer/input.h"
#include "graph/game.h"
#include "log.h"

#include "filesystem/runtime.h"

int main()
{
	// Setup filesystem service
	Filesystem::Runtime filesystem;

	// Setup render service
#if defined(__OPENGL)
	Renderer::GLRenderer renderer_api;
#elif defined(__D3D11)
	Renderer::D3D11Renderer renderer_api;
#endif

	Renderer::Runtime renderer;
	renderer.SetRenderer(&renderer_api);

	Renderer::Input input;

	Graph::Game game;
	game.registerService(&filesystem);
	game.registerService(&renderer);
	game.registerService(&input);
	game.setupServices();
	game.Run();
}
