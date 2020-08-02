#include <string>
#include <stdio.h>

#include "renderer/subsystems/opengl/glrenderer.h"
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
	Renderer::GLRenderer glrenderer;
	Renderer::Runtime renderer;
	renderer.SetRenderer(&glrenderer);

	Renderer::Input input;

	Graph::Game game;
	game.registerService(&filesystem);
	game.registerService(&renderer);
	game.registerService(&input);
	game.setupServices();
	game.Run();
}
