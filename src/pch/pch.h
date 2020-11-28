// This is meant to be included in every .cpp file, first line!

#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <mutex>
#include <vector>

#include <atomic>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdint>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/os.h>

#include "physfs.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif
