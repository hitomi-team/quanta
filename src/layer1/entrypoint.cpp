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
	// TODO: Add option to switch between supported renderers
	// Also TODO: Add command arguments to do this which is a lot better
	Renderer::D3D11Renderer d3d11renderer_api;
	Renderer::Runtime renderer;
	renderer.SetRenderer(&d3d11renderer_api);

	Renderer::Input input;

	Graph::Game game;
	game.registerService(&filesystem);
	game.registerService(&renderer);
	game.registerService(&input);
	game.setupServices();
	game.Run();
}
