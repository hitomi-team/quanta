#include <string>
#include <stdio.h>

#include "renderer/subsystems/opengl/glrenderer.h"
#include "renderer/runtime.h"
#include "graph/game.h"

#include "filesystem/runtime.h"

int main()
{
	// Setup filesystem service
	Filesystem::Runtime filesystem;

	// Setup render service
	Renderer::GLRenderer *glrenderer = new Renderer::GLRenderer;
	Renderer::Runtime renderer;
	renderer.SetRenderer(glrenderer);

	Graph::Game game;
	game.registerService(&filesystem);
	game.registerService(&renderer);
	game.Run();

	delete glrenderer;
}
