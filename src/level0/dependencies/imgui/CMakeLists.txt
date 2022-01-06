add_sources(
	imgui.cpp
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_impl_sdl.cpp
	imgui_impl_vulkan.cpp
	imgui_tables.cpp
	imgui_widgets.cpp
)

if (WIN32)
	add_sources(imgui_impl_dx12.cpp)
endif()
