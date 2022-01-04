#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <memory>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <queue>
#include <iostream>
#include <sstream>
#include <fstream>
#include <array>
#include <bitset>
#include <unordered_map>
#include <exception>
#include <functional>
#include <fmt/format.h>
#include <fmt/compile.h>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "utf8.h"

/* System headers */
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#include <windows.h>
#include <windowsx.h>
#include "snprintf.h"
#elif __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc.h>
#include <objc/objc-runtime.h>
#else
#include <sys/types.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#endif

#include <debugbreak.h>

/* Our own stuff */
#include "common.h"
#include "sys.h"
#include "log.h"
#include "perfclock.h"
