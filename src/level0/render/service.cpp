#include "level0/pch.h"
#include "level0/log.h"

#include "level0/cvar.h"
#include "level0/game/game.h"

#include "defs.h"
#include "vulkan/api.h"

#include "service.h"

CVAR_INIT(r_fpsmax, "300", "300");

RenderService::RenderService() : GameService("RenderService")
{
	CVar_Add(&CVAR_VAR(r_fpsmax));

	this->renderAPI = std::make_unique< VulkanAPI >(GAME_TITLE_NAME);
	g_VulkanAPI = dynamic_cast< VulkanAPI * >(this->renderAPI.get());

	m_device = g_VulkanAPI->physicalDevices[0]->CreateLogicalDevice();
	m_commandpool = m_device->CreateCommandPool(DEVICE_QUEUE_GRAPHICS, COMMAND_POOL_USAGE_NORMAL);

	m_presenter = std::make_unique< RenderPresenter >(m_device, PRESENT_MODE_IMMEDIATE);
	this->RedoCommandBuffers();

	m_imguiRenderPass = m_device->CreateRenderPass(
		[this](){
			std::vector< RenderAttachmentDescription > attachments;
			RenderAttachmentDescription attachment = {};
			attachment.format = m_presenter->GetImage(0)->GetFormat();
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

	m_imgui = m_device->CreateImGui(m_imguiRenderPass, m_presenter->GetMaxImages());

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
}

void RenderService::Update()
{
	m_pc.Calc();

	m_lag += m_pc.dt_f64;

	double limit = 1./CVAR_VAR(r_fpsmax).GetFloat();

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
		m_imgui->NewFrame();

		if (!ImGui::Begin("Developer Menu", nullptr, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings))
			g_Game->Abort("RenderService: Unable to make developer menu!");

		ImGui::SetWindowSize(ImVec2(300, 275));

		if (ImGui::CollapsingHeader("App Information"))
			ImGui::Text("Build Date: %s @ %s", __DATE__, __TIME__);

		if (ImGui::CollapsingHeader("Performance"))
			ImGui::Text("Frametime: %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();

		m_imgui->Draw(m_presenter->GetFramebuffer(imageIndex), imageIndex);

		m_device->Submit(DEVICE_QUEUE_GRAPHICS, m_imgui->GetCommandBuffer(imageIndex), sync.imageAvailable, PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT, sync.renderFinished, sync.fence);

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

	m_commandbufs.resize(m_presenter->GetMaxImages());
	m_commandpool->AllocateBulk(m_commandbufs, COMMAND_BUFFER_LEVEL_PRIMARY);

	for (size_t i = 0; i < m_commandbufs.size(); i++) {
		m_commandbufs[i]->Begin(COMMAND_BUFFER_USAGE_NORMAL);
		m_commandbufs[i]->BeginRenderPass(m_presenter->GetRenderPass(), m_presenter->GetFramebuffer(i), [&]() -> RenderRectangle {
			auto extent = m_presenter->GetImage(i)->GetExtent();
			return RenderRectangle { RenderOffset2D { 0, 0 }, RenderExtent2D { extent.width, extent.height } };
		}(), std::vector< RenderClearValue >(), SUBPASS_CONTENTS_INLINE);
		m_commandbufs[i]->EndRenderPass();
		m_commandbufs[i]->End();
	}
}
