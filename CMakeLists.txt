project(quanta LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

if (WIN32)
	add_definitions(-D__D3D11)
	set(SYSTEM_LIBS d3d11 dxgi dxguid windowscodecs)
	include_directories(inc-win)
endif()

add_definitions(-D__VULKAN)

# TODO: fix this
add_definitions(-D__DEBUG)
if (CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions(-D__DEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES Rel)
	add_definitions(-D__RELEASE)
endif()

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src inc)

set(PCH_FILE src/pch/pch.h)

set(GAME_SRCS
	# core
	src/layer1/entrypoint.cpp
	src/layer1/log.cpp
	src/layer1/perfclock.cpp
	src/layer1/graph/game.cpp
	src/layer1/graph/node.cpp

	# services
	src/layer1/filesystem/runtime.cpp
	src/layer1/renderer/camera.cpp
	src/layer1/renderer/input.cpp
	src/layer1/renderer/mesh.cpp
	src/layer1/renderer/material.cpp
	src/layer1/renderer/materialjson.cpp
	src/layer1/renderer/prop.cpp
	src/layer1/renderer/runtime.cpp
	src/layer1/renderer/shader.cpp
	src/layer1/renderer/transform.cpp
	src/layer1/renderer/stb_image.c
	src/layer1/renderer/imgui/imgui.cpp
	src/layer1/renderer/imgui/imgui_demo.cpp
	src/layer1/renderer/imgui/imgui_draw.cpp
	src/layer1/renderer/imgui/imgui_impl_sdl.cpp
	src/layer1/renderer/imgui/imgui_widgets.cpp

	# renderer
	src/layer1/renderer/subsystems/null/nullrenderer.cpp
)

set(GAME_D3D11_SRCS
	src/layer1/renderer/subsystems/d3d11/d3d11indexbuffer.cpp
	src/layer1/renderer/subsystems/d3d11/d3d11inputlayout.cpp
	src/layer1/renderer/subsystems/d3d11/d3d11parameterbuffer.cpp
	src/layer1/renderer/subsystems/d3d11/d3d11renderer.cpp
	src/layer1/renderer/subsystems/d3d11/d3d11rendertarget.cpp
	src/layer1/renderer/subsystems/d3d11/d3d11shader.cpp
	src/layer1/renderer/subsystems/d3d11/d3d11texture2d.cpp
	src/layer1/renderer/subsystems/d3d11/d3d11vertexbuffer.cpp
	src/layer1/renderer/subsystems/d3d11/imgui_impl_d3d11.cpp
)

set(GAME_VULKAN_SRCS
	src/layer1/renderer/subsystems/vulkan/imgui_impl_vulkan.cpp
	src/layer1/renderer/subsystems/vulkan/vk_mem_alloc.cpp
	src/layer1/renderer/subsystems/vulkan/vulkanbuffer.cpp
	src/layer1/renderer/subsystems/vulkan/vulkanctx.cpp
	src/layer1/renderer/subsystems/vulkan/vulkanimage.cpp
	src/layer1/renderer/subsystems/vulkan/vulkanindexbuffer.cpp
	src/layer1/renderer/subsystems/vulkan/vulkaninputlayout.cpp
	src/layer1/renderer/subsystems/vulkan/vulkanparameterbuffer.cpp
	src/layer1/renderer/subsystems/vulkan/vulkanrenderer.cpp
	src/layer1/renderer/subsystems/vulkan/vulkanrendertarget.cpp
	src/layer1/renderer/subsystems/vulkan/vulkanshader.cpp
	src/layer1/renderer/subsystems/vulkan/vulkantexture2d.cpp
	src/layer1/renderer/subsystems/vulkan/vulkanvertexbuffer.cpp
	src/layer1/renderer/subsystems/vulkan/volk.c
)

set(DEP_SRCS
	# fmt
	src/layer1/dep/fmt/format.cc
	src/layer1/dep/fmt/os.cc

	# physfs
	src/layer1/dep/physfs/physfs.c
	src/layer1/dep/physfs/physfs_archiver_7z.c
	src/layer1/dep/physfs/physfs_archiver_dir.c
	src/layer1/dep/physfs/physfs_archiver_grp.c
	src/layer1/dep/physfs/physfs_archiver_hog.c
	src/layer1/dep/physfs/physfs_archiver_iso9660.c
	src/layer1/dep/physfs/physfs_archiver_mvl.c
	src/layer1/dep/physfs/physfs_archiver_qpak.c
	src/layer1/dep/physfs/physfs_archiver_slb.c
	src/layer1/dep/physfs/physfs_archiver_unpacked.c
	src/layer1/dep/physfs/physfs_archiver_vdf.c
	src/layer1/dep/physfs/physfs_archiver_wad.c
	src/layer1/dep/physfs/physfs_archiver_zip.c
	src/layer1/dep/physfs/physfs_byteorder.c
	src/layer1/dep/physfs/physfs_unicode.c
	src/layer1/dep/physfs/physfs_platform_haiku.cpp
	src/layer1/dep/physfs/physfs_platform_os2.c
	src/layer1/dep/physfs/physfs_platform_posix.c
	src/layer1/dep/physfs/physfs_platform_qnx.c
	src/layer1/dep/physfs/physfs_platform_unix.c
	src/layer1/dep/physfs/physfs_platform_windows.c
	src/layer1/dep/physfs/physfs_platform_winrt.cpp
)

set(SRCS ${DEP_SRCS} ${GAME_SRCS})

if (WIN32)
	set(SRCS ${SRCS} ${GAME_VULKAN_SRCS} ${GAME_D3D11_SRCS})
else (UNIX)
	set(SRCS ${SRCS} ${GAME_VULKAN_SRCS})
endif()

add_executable(quanta WIN32 ${SRCS})
target_link_libraries(quanta ${SYSTEM_LIBS} SDL2 SDL2main)
target_precompile_headers(quanta PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/${PCH_FILE}>")
