#include "glrenderer.h"

namespace Renderer {

	static const unsigned glCmpFunc[] =
	{
		GL_ALWAYS,
		GL_EQUAL,
		GL_NOTEQUAL,
		GL_LESS,
		GL_LEQUAL,
		GL_GREATER,
		GL_GEQUAL
	};

	static const unsigned glSrcBlend[] =
	{
		GL_ONE,
		GL_ONE,
		GL_DST_COLOR,
		GL_SRC_ALPHA,
		GL_SRC_ALPHA,
		GL_ONE,
		GL_ONE_MINUS_DST_ALPHA,
		GL_ONE,
		GL_SRC_ALPHA
	};

	static const unsigned glDestBlend[] =
	{
		GL_ZERO,
		GL_ONE,
		GL_ZERO,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_ONE,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE,
		GL_ONE
	};

	static const unsigned glBlendOp[] =
	{
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_REVERSE_SUBTRACT,
		GL_FUNC_REVERSE_SUBTRACT
	};

	static const unsigned glFillMode[] =
	{
		GL_FILL,
		GL_LINE,
		GL_POINT
	};

	static const unsigned glStencilOps[] =
	{
		GL_KEEP,
		GL_ZERO,
		GL_REPLACE,
		GL_INCR_WRAP,
		GL_DECR_WRAP
	};

	static void GetPrimitiveType(unsigned elementCount, PrimitiveType type, unsigned *primitiveCount, GLenum *glPrimitiveType)
	{
		switch(type) {
		case TRIANGLE_LIST:
			*primitiveCount = elementCount / 3;
			*glPrimitiveType = GL_TRIANGLES;
			break;
		case LINE_LIST:
			*primitiveCount = elementCount / 2;
			*glPrimitiveType = GL_LINES;
			break;

		case POINT_LIST:
			*primitiveCount = elementCount;
			*glPrimitiveType = GL_POINTS;
			break;

		case TRIANGLE_STRIP:
			*primitiveCount = elementCount - 2;
			*glPrimitiveType = GL_TRIANGLE_STRIP;
			break;

		case LINE_STRIP:
			*primitiveCount = elementCount - 1;
			*glPrimitiveType = GL_LINE_STRIP;
			break;

		case TRIANGLE_FAN:
			*primitiveCount = elementCount - 2;
			*glPrimitiveType = GL_TRIANGLE_FAN;
			break;		
		}
	}

	GLRenderer::GLRenderer()
	{
		SDL_Init(SDL_INIT_VIDEO);
	}

	bool GLRenderer::SetGraphicsMode(int width, int height, bool fullscreen, bool borderless, bool resizable, bool vsync, int multisample)
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

		glewExperimental = true;
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			FATAL((const char *)glewGetErrorString(err));
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

	void GLRenderer::Close()
	{
		if (!window)
			return;
		
		if (context)  {
			SDL_GL_DeleteContext(context);
			context = nullptr;
		}

		SDL_ShowCursor(SDL_TRUE);
		SDL_DestroyWindow(window);
		window = nullptr;

		SDL_Quit();
	}

	void GLRenderer::SetBlendMode(BlendMode mode)
	{
		if (mode == BLEND_REPLACE) {
			glDisable(GL_BLEND);
		} else {
			glEnable(GL_BLEND);
			glBlendFunc(glSrcBlend[mode], glDestBlend[mode]);
			glBlendEquation(glBlendOp[mode]);
		}
	}

	void GLRenderer::SetColorWrite(bool enable)
	{
		if (enable) {
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		} else {
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		}
	}

	void GLRenderer::SetCullMode(CullMode mode)
	{
		if (mode == CULL_NONE) {
			glDisable(GL_CULL_FACE);
		} else {
			glEnable(GL_CULL_FACE);
			glCullFace(mode == CULL_CCW ? GL_FRONT : GL_BACK);
		}
	}

	void GLRenderer::SetDepthBias(float constantBias, float slopeScaledBias)
	{
		if (slopeScaledBias != 0.0f) {
			float adjustedSlopeScaled = slopeScaledBias + 1.0f;
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(adjustedSlopeScaled, 0.0f);
		} else {
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}

	void GLRenderer::SetDepthTest(CompareMode mode)
	{
		glDepthFunc(glCmpFunc[mode]);
	}

	void GLRenderer::SetFillMode(FillMode mode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, glFillMode[mode]);
	}

	void GLRenderer::SetViewport(const glm::vec4& rect)
	{
		glViewport(rect.x, rect.y, rect.z, rect.w);
	}

	void GLRenderer::Draw(PrimitiveType type, unsigned vertexStart, unsigned vertexCount)
	{
		if (!vertexCount)
			return;
		
		GLenum glPrimitiveType;
		unsigned primitiveCount;

		GetPrimitiveType(vertexCount, type, &primitiveCount, &glPrimitiveType);
		glDrawArrays(glPrimitiveType, vertexStart, vertexCount);
	}

}
