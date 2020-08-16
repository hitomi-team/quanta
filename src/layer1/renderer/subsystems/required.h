#ifndef RENDERER_REQUIRED_H
#define RENDERER_REQUIRED_H

#include "layer1/renderer/defs.h"
#include "layer1/renderer/gpuresource.h"
#include "layer1/renderer/indexbuffer.h"
#include "layer1/renderer/inputlayout.h"
#include "layer1/renderer/shaderparameterbuffer.h"
#include "layer1/renderer/rendertarget.h"
#include "layer1/renderer/shader.h"
#include "layer1/renderer/texture2d.h"
#include "layer1/renderer/vertexbuffer.h"
#include "layer1/config.h"
#include "layer1/log.h"
#include "rhi.h"

#define FATAL(x) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Engine Error", x, nullptr); global_log.Error(StringFormat("Renderer: ", x));

#endif
