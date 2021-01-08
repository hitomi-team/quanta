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

#include <SDL.h>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/os.h>

#include "physfs.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif

#ifndef PCH_ALIGN__
#define PCH_ALIGN__

template< typename T, typename U = T >
static constexpr T PCH_ALIGN(T what, U to)
{
	return (what + to - 1) & ~(to - 1);
}

template< typename T, typename U = T >
static constexpr T PCH_ALIGN_DOWN(T what, U to)
{
	return (what / to) * to;
}

template< typename T >
static constexpr void PCH_ERASE_VECTOR(T &vec)
{
	T().swap(vec);
}

#endif
