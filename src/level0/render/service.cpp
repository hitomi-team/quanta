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

	m_swapchain = m_device->CreateSwapchain(PRESENT_MODE_FIFO_VSYNC, DEVICE_QUEUE_GRAPHICS);
	this->ReinitSwapchain();

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
		m_swapchainInFlightFences[m_frameCounter]->Wait(UINT64_MAX);

		uint32_t imageIndex;
		if (!m_swapchain->GetAvailableImage(m_swapchainImageAvailable[m_frameCounter], nullptr, UINT64_MAX, imageIndex)) {
			m_swapchain->Recreate(PRESENT_MODE_FIFO_VSYNC, DEVICE_QUEUE_GRAPHICS);
			this->ReinitSwapchain();
			return;
		}

		if (m_imagesInFlight[imageIndex] != VK_NULL_HANDLE)
			m_imagesInFlight[imageIndex]->Wait(UINT64_MAX);

		m_imagesInFlight[imageIndex] = m_swapchainInFlightFences[m_frameCounter];

		m_swapchainInFlightFences[m_frameCounter]->Reset();

		m_device->Submit(DEVICE_QUEUE_GRAPHICS, m_commandbufs[imageIndex], m_swapchainImageAvailable[m_frameCounter], PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT, m_renderFinished[m_frameCounter], m_swapchainInFlightFences[m_frameCounter]);
		if (!m_swapchain->PresentImage(m_renderFinished[m_frameCounter], imageIndex)) {
			m_swapchain->Recreate(PRESENT_MODE_FIFO_VSYNC, DEVICE_QUEUE_GRAPHICS);
			this->ReinitSwapchain();
			return;
		}

		m_frameCounter = (m_frameCounter + 1) % m_swapchainImages.size();
		m_lag -= std::max(limit, m_pc.dt_f64);
	}
}

void RenderService::TryResizeSwapchain()
{
	m_swapchain->Recreate(PRESENT_MODE_FIFO_VSYNC, DEVICE_QUEUE_GRAPHICS);
	this->ReinitSwapchain();
}

void RenderService::ReinitSwapchain()
{
	for (auto &fence : m_swapchainInFlightFences)
		fence->Wait(UINT64_MAX);

	IO_EraseCPPVector(m_swapchainInFlightFences);
	IO_EraseCPPVector(m_imagesInFlight);
	IO_EraseCPPVector(m_swapchainImageAvailable);
	IO_EraseCPPVector(m_renderFinished);
	IO_EraseCPPVector(m_swapchainFramebuffers);
	IO_EraseCPPVector(m_swapchainImages);

	m_swapchainImages = m_swapchain->GetImages();

	m_renderpass = m_device->CreateRenderPass(
		[this](){
			std::vector< RenderAttachmentDescription > attachments;
			RenderAttachmentDescription attachment = {};
			attachment.format = m_swapchainImages[0]->GetFormat();
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

	m_swapchainFramebuffers.resize(m_swapchainImages.size());
	for (size_t i = 0; i < m_swapchainImages.size(); i++)
		m_swapchainFramebuffers[i] = m_device->CreateFramebuffer(m_renderpass, m_swapchainImages[i], m_swapchainImages[i]->GetExtent());

	m_swapchainInFlightFences.resize(m_swapchainImages.size());
	m_swapchainImageAvailable.resize(m_swapchainImages.size());
	m_renderFinished.resize(m_swapchainImages.size());
	for (size_t i = 0; i < m_swapchainImages.size(); i++) {
		m_swapchainInFlightFences[i] = m_device->CreateFence(true);
		m_swapchainImageAvailable[i] = m_device->CreateSemaphore();
		m_renderFinished[i] = m_device->CreateSemaphore();
	}

	m_imagesInFlight.resize(m_swapchainImages.size(), nullptr);

	IO_EraseCPPVector(m_commandbufs);

	m_commandbufs.resize(m_swapchainImages.size());
	m_commandpool->AllocateBulk(m_commandbufs, COMMAND_BUFFER_LEVEL_PRIMARY);

	for (size_t i = 0; i < m_commandbufs.size(); i++) {
		m_commandbufs[i]->Begin(COMMAND_BUFFER_USAGE_NORMAL);
		m_commandbufs[i]->BeginRenderPass(m_renderpass, m_swapchainFramebuffers[i], [&]() -> RenderRectangle {
			auto extent = m_swapchainImages[i]->GetExtent();
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

	m_frameCounter = 0;
	m_lag = 0.;
}
