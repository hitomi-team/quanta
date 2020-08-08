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
		if (!instance.Load(window))
			return false;
		
		device = QueryDevice(instance);
		surface.Load(window, instance, device, APP_TITLE, width, height);
		surface.Clear(0.0, 0.0, 0.0, 0.0);
		surface.Present();

		// init imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().DisplaySize.x = width;
		ImGui::GetIO().DisplaySize.y = height;
		ImGui::GetIO().Fonts->AddFontDefault();
		ImGui::GetIO().Fonts->Build();
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForVulkan(window);

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
		surface.Present();
		// present swapchain
	}

	void VulkanRenderer::Clear(unsigned flags, const glm::vec4& color, float depth, unsigned stencil)
	{
		(void)flags;
		(void)depth;
		(void)stencil;

		surface.Clear(color.r, color.g, color.b, color.a);
	}

	void VulkanRenderer::Close()
	{
//		ImGui_ImplVulkan_Shutdown()

		// vulkan object releasing stuff
		surface.Release(instance);
		device.Release();
		instance.Release();

		ResetCache(); // just in case we want to initialize this renderer again

		SDL_ShowCursor(SDL_TRUE);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void VulkanRenderer::ResetCache()
	{

	}

	void VulkanRenderer::ImGuiNewFrame()
	{
//		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();
	}

	void VulkanRenderer::ImGuiEndFrame()
	{
		ImGui::End();
		ImGui::Render();
//		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());
	}

}
