#include "vulkanrenderer.h"

namespace Renderer {

	VulkanRenderer::VulkanRenderer()
	{
		SDL_Init(SDL_INIT_VIDEO);
		ResetCache();
	}

	bool VulkanRenderer::SetGraphicsMode(int width, int height, bool fullscreen, bool borderless, bool resizable, bool vsync, int multisample)
	{
		if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO)
			SDL_Init(SDL_INIT_VIDEO);
		
		int x = fullscreen ? 0 : SDL_WINDOWPOS_CENTERED;
		int y = fullscreen ? 0 : SDL_WINDOWPOS_CENTERED;

		unsigned flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

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

		// init device and other stuff

		return true;
	}

	bool VulkanRenderer::BeginFrame()
	{
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
			return false;
		
		// do prerender stuff here

		return true;
	}

	void VulkanRenderer::EndFrame()
	{
		// present swapchain
	}

	void VulkanRenderer::Close()
	{
		// ImGui_ImplVulkan_Shutdown()

		// vulkan object releasing stuff

		ResetCache(); // just in case we want to initialize this renderer again

		SDL_ShowCursor(SDL_TRUE);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void VulkanRenderer::ResetCache()
	{

	}

}