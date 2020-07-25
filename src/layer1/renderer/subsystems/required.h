#ifndef RENDERER_REQUIRED_H
#define RENDERER_REQUIRED_H

#include <cstdlib>
#include <string>
#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "defs.h"
#include "../gpuresource.h"
#include "../indexbuffer.h"
#include "../inputlayout.h"
#include "../rendertarget.h"
#include "../shader.h"
#include "../texture.h"
#include "../vertexbuffer.h"
#include "../../config.h"

#define FATAL(x) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Engine Error", x, nullptr);

#endif