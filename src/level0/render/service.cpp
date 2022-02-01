#include "level0/pch.h"
#include "level0/log.h"

#include "level0/cvarcmd/service.h"
#include "level0/game/game.h"

#include "defs.h"
#include "vulkan/api.h"

#include "service.h"

RenderService::RenderService() : GameService("RenderService")
{
	auto cvarCmdService = g_Game->GetService< CVarCmdService >();
	cvarCmdService->CreateCVar< double >("r_fpsmax", "Max FPS Target for FPS Limiter", 144., 30., 300., 144.);

	this->renderAPI = std::make_unique< VulkanAPI >(GAME_TITLE_NAME);
	g_VulkanAPI = dynamic_cast< VulkanAPI * >(this->renderAPI.get());

	m_device = g_VulkanAPI->physicalDevices[0]->CreateLogicalDevice();
	m_commandpool = m_device->CreateCommandPool(DEVICE_QUEUE_GRAPHICS, COMMAND_POOL_USAGE_NORMAL);

	m_presenter = std::make_unique< RenderPresenter >(m_device, PRESENT_MODE_IMMEDIATE);
	this->RedoCommandBuffers();

	m_imguiRenderPass = m_device->CreateRenderPass(
		[](){
			std::vector< RenderAttachmentDescription > attachments;
			RenderAttachmentDescription attachment = {};
			attachment.format = eRenderImageFormat::B8G8R8A8_UNORM;
			attachment.loadOp = ATTACHMENT_LOAD_OP_CLEAR;
			attachment.storeOp = ATTACHMENT_STORE_OP_STORE;
			attachment.stencilLoadOp = ATTACHMENT_LOAD_OP_DONT_CARE;
			attachment.stencilStoreOp = ATTACHMENT_STORE_OP_DONT_CARE;
			attachment.initialLayout = IMAGE_LAYOUT_UNDEFINED;
			attachment.finalLayout = IMAGE_LAYOUT_PRESENT_SRC;
			attachments.push_back(attachment);
			return attachments;
		}(),
		[](){
			std::vector< RenderSubpassDescription > subpasses;
			RenderAttachmentReference ref;
			ref.attachment = 0;
			ref.layout = IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			RenderSubpassDescription subpass = {};
			subpass.colorAttachments.push_back(ref);
			subpasses.push_back(subpass);
			return subpasses;
		}(),
		[](){
			std::vector< RenderSubpassDependency > dependencies;
			RenderSubpassDependency dependency = {};
			dependency.sourceSubpass = ~0;
			dependency.destSubpass = 0;
			dependency.sourceStageMask = PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
			dependency.sourceAccessMask = RESOURCE_ACCESS_NONE;
			dependency.destStageMask = PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
			dependency.destAccessMask = RESOURCE_ACCESS_COLOR_ATTACHMENT_WRITE;
			dependencies.push_back(dependency);
			return dependencies;
		}()
	);

	m_imgui = m_device->CreateImGui(m_imguiRenderPass, m_presenter->m_swapchain->minNumImages, m_presenter->m_swapchain->numImages);

	m_init = true;
}

/*
RenderService::RenderService(int api) : GameService("RenderService")
{
	g_Game->Abort("RenderService::RenderService(int api) - Unimplemented");
}
*/

RenderService::~RenderService()
{
	if (!m_init)
		return;

	m_device->WaitIdle();

	m_imguiRenderPass.reset();
	m_imgui.reset();

	IO_EraseCPPVector(m_commandbufs);
	m_commandpool.reset();
	m_presenter.release();
	m_device.reset();

	this->renderAPI.release();
}

void RenderService::Update()
{
	auto cvarCmdService = g_Game->GetService< CVarCmdService >();

	double limit = 1./cvarCmdService->GetCVarValue< double >([]() -> uint64_t {
		static constexpr uint64_t hash = UtilStringHash("r_fpsmax");
		return hash;
	}());

	m_pc.Calc();

	m_lag += m_pc.dt_f64;

	while (m_lag >= limit) {
		uint32_t imageIndex;
		RenderPresenterSync sync;

		ESwapchainResult result = m_presenter->AcquireNextImage(sync, imageIndex);
		if (result == SWAPCHAIN_RESULT_SUBOPTIMAL) {
			this->TryResizeSwapchain();
			return;
		} else if (result == SWAPCHAIN_RESULT_NOT_READY) {
			return;
		} else if (result != SWAPCHAIN_RESULT_SUCCESS) {
			g_Game->Abort("RenderService: Unable to acquire next image!");
		}

		// TODO: move all imgui stuff into their own functions
/*
		m_imgui->NewFrame();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(300, 275));
		ImGui::Begin("Developer Menu", nullptr, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
		ImGui::PopStyleVar();

		if (ImGui::CollapsingHeader("App Information"))
			ImGui::Text("Build Date: %s @ %s", __DATE__, __TIME__);

		if (ImGui::CollapsingHeader("Performance"))
			ImGui::Text("Frametime: %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		if (ImGui::CollapsingHeader("Console")) {
			ImGui::BeginChild("Log", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

			for (const auto &line : g_Log.buffer)
				ImGui::TextUnformatted(line.c_str());

			if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);

			ImGui::EndChild();

			if (ImGui::InputText("Input", m_imguiConsoleTextInput.data(), m_imguiConsoleTextInput.size(), ImGuiInputTextFlags_EnterReturnsTrue, nullptr, m_imguiConsoleTextInput.data())) {
				cvarCmdService->Exec(m_imguiConsoleTextInput.data());
				std::fill(m_imguiConsoleTextInput.begin(), m_imguiConsoleTextInput.end(), '\0');
				ImGui::SetKeyboardFocusHere(-1);
			}

			ImGui::SameLine(); if (ImGui::Button("Submit")) {
				cvarCmdService->Exec(m_imguiConsoleTextInput.data());
				std::fill(m_imguiConsoleTextInput.begin(), m_imguiConsoleTextInput.end(), '\0');
			}

			ImGui::SameLine(); if (ImGui::Button("Clear"))
				g_Log.ClearBuffer();
		}

		ImGui::End();

		m_imgui->Draw(m_presenter->GetFramebuffer(imageIndex), imageIndex);
*/
		m_device->Submit(DEVICE_QUEUE_GRAPHICS, m_commandbufs[imageIndex], sync.imageAvailable, PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT, sync.renderFinished, sync.fence);

		result = m_presenter->QueuePresent();
		if (result == SWAPCHAIN_RESULT_SUBOPTIMAL) {
			this->TryResizeSwapchain();
			return;
		} else if (result != SWAPCHAIN_RESULT_SUCCESS) {
			g_Game->Abort("RenderService: Unable to present image!");
		}

		m_presenter->IncrementFrameCounter();
		m_lag -= std::max(limit, m_pc.dt_f64);
	}
}

void RenderService::TryResizeSwapchain()
{
	m_presenter->Recreate(PRESENT_MODE_IMMEDIATE);
	this->RedoCommandBuffers();
}

void RenderService::RedoCommandBuffers()
{
	IO_EraseCPPVector(m_commandbufs);

	m_commandbufs.resize(m_presenter->m_swapchain->numImages);
	m_commandpool->AllocateBulk(m_commandbufs, COMMAND_BUFFER_LEVEL_PRIMARY);

	for (size_t i = 0; i < m_commandbufs.size(); i++) {
		m_commandbufs[i]->Begin(COMMAND_BUFFER_USAGE_NORMAL);
		m_presenter->m_swapchain->BeginRenderPass(m_commandbufs[i], i);
		m_commandbufs[i]->EndRenderPass();
		m_commandbufs[i]->End();
	}
}
