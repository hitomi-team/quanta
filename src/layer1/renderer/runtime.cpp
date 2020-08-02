#include "imgui/imgui.h"
#include "runtime.h"

namespace Renderer {

	Runtime::Runtime() : Service("RenderService")
	{
		rhi = nullptr;
	}

	bool Runtime::Setup()
	{
		if (rhi->SetGraphicsMode(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, false, false, false, false, 4) != false)
			Initialized = true;

		return Initialized;
	}

	bool Runtime::Update()
	{
		// Check if window is minimized, if it is then don't draw anything
		if (!rhi->BeginFrame())
			return true;
		
		// for now, we use a test clear to make sure things are working
		// if the window doesn't show pink then it probably means something isn't working.
		rhi->Clear(CLEAR_COLOR | CLEAR_DEPTH, glm::vec4(0.8f, 0.0f, 0.8f, 1.0f), 1.0f);
		__debug_menu();
		rhi->EndFrame();

		return true;
	}

	void Runtime::Release()
	{
		rhi->Close();
	}

	void Runtime::SetRenderer(RHI *rhi)
	{
		if (!rhi)
			return;
		
		this->rhi = rhi;
	}

	void Runtime::__debug_menu()
	{
		// Will add to this later such as hardware info and other profiling info, such as logging and maybe a console?

		rhi->ImGuiNewFrame();
		
		if (!ImGui::Begin("Developer Menu", nullptr, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings)) {
			rhi->ImGuiEndFrame();
			return;
		}

		ImGui::SetWindowSize(ImVec2(275, 250));

		if (ImGui::CollapsingHeader("App Information")) {
			ImGui::Text("Build Date: %s @ %s", __DATE__, __TIME__);
		}

		if (ImGui::CollapsingHeader("Performance")) {
			ImGui::Text("Frametime: %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}

		rhi->ImGuiEndFrame();
	}

}
