#include "glrenderer.h"

namespace Renderer {

	GLRenderer::GLRenderer()
	{
		SDL_Init(SDL_INIT_VIDEO);
	}

	bool GLRenderer::SetGraphicsMode(int width, int height, bool fullscreen, bool borderless, bool resizable, bool vsync, bool triplebuffer, int multisample)
	{
		SDL_GL_SetSwapInterval(vsync ? 1 : 0);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

		if (multisample > 1) {
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, multisample);
		}

		int x = fullscreen ? 0 : SDL_WINDOWPOS_CENTERED;
		int y = fullscreen ? 0 : SDL_WINDOWPOS_CENTERED;

		unsigned flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		if (fullscreen) {
			SDL_GetCurrentDisplayMode(0, &display);

			width = display.w;
			height = display.h;

			flags |= SDL_WINDOW_FULLSCREEN;
		} else if (resizable) {
			flags |= SDL_WINDOW_RESIZABLE;
		} else if (borderless) {
			flags |= SDL_WINDOW_BORDERLESS;
		}

		window = SDL_CreateWindow(APP_TITLE, x, y, width, height, flags);
		if (!window) {
			FATAL(SDL_GetError());
			return false;
		}

		context = SDL_GL_CreateContext(window);
		if (!context) {
			FATAL(SDL_GetError());
			return false;
		}

		Clear(CLEAR_COLOR);
		SDL_GL_SwapWindow(window);

		return true;
	}

	void GLRenderer::Clear(unsigned flags, const glm::vec4& color, float depth, unsigned stencil)
	{
		unsigned glFlags = 0;
		if (flags & CLEAR_COLOR) {
			glFlags |= GL_COLOR_BUFFER_BIT;
			glClearColor(color.x, color.y, color.z, color.w);
		}
		if (flags & CLEAR_DEPTH) {
			glFlags |= GL_DEPTH_BUFFER_BIT;
			glClearDepth(depth);
		}
		if (flags & CLEAR_STENCIL) {
			glFlags |= GL_STENCIL_BUFFER_BIT;
			glClearStencil(stencil);
		}

		glClear(glFlags);
	}

}
