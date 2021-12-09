# Quanta Engine

## Overview
WIP

### Third-Party Libraries and Dependencies
- [SDL2](https://www.libsdl.org/)
- [volk](https://github.com/zeux/volk)
- [Vulkan Memory Allocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [debugbreak](https://github.com/scottt/debugbreak)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [RapidJSON](https://github.com/Tencent/rapidjson)
- [physfs](https://www.icculus.org/physfs/)
- [Branchless UTF-8 Decoder](https://github.com/skeeto/branchless-utf8)
- [fmt](https://github.com/fmtlib/fmt)
- [OpenGL Mathematics](https://github.com/g-truc/glm)
- [CMake](https://cmake.org/)

### Compilation
Ninja build tool recommended

This section is a WIP, but essentially:
```
git clone https://github.com/koukuno/interspace
cd interspace
cmake .
python3 tools/compile-shaders.py
python3 tools/build-data-pack.py
```

### Project Structure
```
├── data: Default game assets
├── include: Include headers
├── shaders: Shader sources
├── src: Game engine source code
└── tools: Tools for the game engine
```

### License
[BSD-2-Clause License](LICENSE.txt)
