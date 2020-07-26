#include <string>
#include <stdio.h>

#include "renderer/subsystems/opengl/glrenderer.h"
#include "renderer/subsystems/rhi.h"

int main()
{
	Renderer::RHI *rhi = new Renderer::GLRenderer;

	rhi->SetGraphicsMode(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, true, false, false, false, 0);
	SDL_Delay(1000);
}
