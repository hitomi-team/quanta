#include "pch/pch.h"

#include "imgui/imgui.h"
#include "runtime.h"

namespace Renderer {

	static const char *apiStr[] = {
		nullptr,
		"D3D11",
		"Vulkan",
		"OpenGL",
	};

	Runtime::Runtime() : Service("RenderService")
	{
		rhi = nullptr;
	}

	bool Runtime::Setup()
	{
		if (rhi->SetGraphicsMode(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, false, false, false, false, 1) != true)
			return false;

		this->time = 0.0f;
		return true;
	}

	bool Runtime::Update()
	{
		this->clock.Begin();

		// Check if window is minimized, if it is then don't draw anything
		if (!rhi->BeginFrame())
			return true;

		// TODO: Update shader parameters

		// for now, we use a test clear to make sure things are working
		// if the window doesn't show pink then it probably means something isn't working.
		rhi->Clear(CLEAR_COLOR, glm::vec4(0.8f, 0.0f, 0.8f, 1.0f), 1.0f);

		for (auto &mat : materials) { // update materials
			mat->getParamBuffer()->Map();
			mat->getParamBuffer()->SetShaderParameter(SHADER_PARAM_TIME, time);
			mat->getParamBuffer()->Unmap();
		}

		for (auto &prop : prop_queue) // TODO: Instancing. We need better perf
			prop->Draw(rhi);

		__debug_menu(); // called last
		rhi->EndFrame();
		this->clock.End();

		this->time += this->clock.deltaf32;

		return true;
	}

	void Runtime::Release()
	{
		rhi->WaitForDevice();

		for (auto &mat : materials) {
			mat->Release();
			delete mat;
		}

		for (auto &mesh : meshes)
			mesh->Release();

		for (auto &prop : prop_queue) {
			prop->Release();
			delete prop;
		}

		rhi->Close();
	}

	void Runtime::SetRenderer(RHI *rhi)
	{
		if (!rhi)
			return;

		if (this->rhi)
			this->rhi->Close();

		this->rhi = rhi;
	}

	Prop *Runtime::AllocateProp(unsigned meshidx, unsigned materialidx)
	{
		if ((meshidx >= meshes.size()) || (materialidx >= materials.size())) {
			global_log.Warn("Tried to allocate invalid prop.");
			return nullptr;
		}

		Prop *prop = new Prop;

		prop->Setup(meshes[meshidx], materials[materialidx]);
		prop_queue.push_back(prop);

		return prop;
	}

	void Runtime::ReleaseProp(Prop *prop)
	{
		if (!prop)
			return;

		// TODO: Put all of this in Prop::Release() and destroy all children.

		prop_queue.erase(std::remove(prop_queue.begin(), prop_queue.end(), prop), prop_queue.end());
		delete prop;
	}

	void Runtime::__debug_menu()
	{
		// Will add to this later such as hardware info and other profiling info, such as logging and maybe a console?

		rhi->ImGuiNewFrame();

		if (!ImGui::Begin("Developer Menu", nullptr, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings)) {
			rhi->ImGuiEndFrame();
			return;
		}

		ImGui::SetWindowSize(ImVec2(300, 275));

		if (ImGui::CollapsingHeader("App Information")) {
			ImGui::Text("Build Date: %s @ %s", __DATE__, __TIME__);
#if defined(__RELEASE)
			ImGui::Text("Build Type: Release");
#elif defined(__DEBUG)
			ImGui::Text("Build Type: Debug");
#else
#error __RELEASE or __DEBUG must be defined!
#endif
			ImGui::Text("Rendering API: %s", apiStr[this->rhi->getRendererType()]);
		}

		if (ImGui::CollapsingHeader("Performance")) {
			ImGui::Text("Frametime: %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
/*			ImGui::Separator();
			ImGui::Text("RHI Information (Low-Level Interface)");
			ImGui::Text("Primitives Drawn: %u", rhi->getPrimitiveCount());
			ImGui::Text("Draw Non-Indexed Calls: %u", rhi->getDrawCount());
			ImGui::Text("Draw Indexed Calls: %u", rhi->getDrawIndexedCount());
			ImGui::Text("Draw Instanced Calls: %u", rhi->getDrawInstancedCount());
			ImGui::Text("Total Draw Calls: %u", rhi->getTotalDrawCallCount());
			ImGui::Separator();
			ImGui::Text("Renderer Information");
			ImGui::Text("Prop Queue: %u", (unsigned int)prop_queue.capacity());
			ImGui::Text("Registered Materials: %u", (unsigned int)materials.capacity());
			ImGui::Text("Registered Meshes: %u", (unsigned int)meshes.capacity());
*/		}

		rhi->ImGuiEndFrame();
	}

}
