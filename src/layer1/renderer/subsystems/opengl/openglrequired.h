#ifndef LAYER1_RENDERER_SUBSYSTEM_OPENGL_OPENGLREQUIRED_H
#define LAYER1_RENDERER_SUBSYSTEM_OPENGL_OPENGLREQUIRED_H

#include "layer1/renderer/subsystems/required.h"
#include "glad.h"

namespace Renderer {
	class OpenGLRenderer;
	extern OpenGLRenderer *global_gl;
}

#endif
