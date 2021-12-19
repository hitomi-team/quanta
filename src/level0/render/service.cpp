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

	m_presenter = std::make_unique< Presenter >(m_device, PRESENT_MODE_FIFO_VSYNC);
	this->RedoCommandBuffers();

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
		PresenterSync sync;

		ESwapchainResult result = m_presenter->AcquireNextImage(sync, imageIndex);
		if (result == SWAPCHAIN_RESULT_SUBOPTIMAL) {
			m_presenter->Recreate(PRESENT_MODE_FIFO_VSYNC);
			return;
		} else if (result == SWAPCHAIN_RESULT_NOT_READY) {
			return;
		} else if (result != SWAPCHAIN_RESULT_SUCCESS) {
			g_Game->Abort("RenderService: Unable to acquire next image!");
		}

		m_device->Submit(DEVICE_QUEUE_GRAPHICS, m_commandbufs[imageIndex], sync.imageAvailable, PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT, sync.renderFinished, sync.fence);

		result = m_presenter->QueuePresent();
		if (result == SWAPCHAIN_RESULT_SUBOPTIMAL) {
			m_presenter->Recreate(PRESENT_MODE_FIFO_VSYNC);
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
	m_presenter->Recreate(PRESENT_MODE_FIFO_VSYNC);
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
		}(), [](){
			static const std::array< float, 4 > clearColorValues { 0.f, 0.2f, 0.f, 1.f };
			RenderClearValue value = {};
			std::copy(clearColorValues.begin(), clearColorValues.end(), value.color.f32);
			return std::vector< RenderClearValue >(1, value);
		}(), SUBPASS_CONTENTS_INLINE);
		m_commandbufs[i]->EndRenderPass();
		m_commandbufs[i]->End();
	}
}
