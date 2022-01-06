#include "level0/pch.h"

#include "presenter.h"

RenderPresenter::RenderPresenter(std::shared_ptr< IRenderDevice > device, ESwapchainPresentMode presentMode)
{
	m_device = device;
	m_swapchain = m_device->CreateSwapchain(presentMode, DEVICE_QUEUE_GRAPHICS);
	this->Recreate(presentMode);
}

RenderPresenter::~RenderPresenter()
{
}

void RenderPresenter::Recreate(ESwapchainPresentMode presentMode)
{
	if (m_init)
		m_swapchain->Recreate(presentMode, DEVICE_QUEUE_GRAPHICS);

	m_device->WaitIdle();

	IO_EraseCPPVector(m_imagesInFlight);
	IO_EraseCPPVector(m_presenterSync);
	IO_EraseCPPVector(m_framebuffers);
	IO_EraseCPPVector(m_images);

	m_images = m_swapchain->GetImages();

	m_renderpass = m_device->CreateRenderPass(
		[this](){
			std::vector< RenderAttachmentDescription > attachments;
			RenderAttachmentDescription attachment = {};
			attachment.format = m_images[0]->GetFormat();
			attachment.loadOp = ATTACHMENT_LOAD_OP_DONT_CARE;
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

	m_framebuffers.resize(m_images.size());
	for (size_t i = 0; i < m_images.size(); i++)
		m_framebuffers[i] = m_device->CreateFramebuffer(m_renderpass, m_images[i], [&]() -> RenderExtent2D {
			auto extent = m_images[i]->GetExtent();
			return RenderExtent2D { extent.width, extent.height };
		}());

	m_presenterSync.resize(m_images.size());
	for (size_t i = 0; i < m_images.size(); i++) {
		m_presenterSync[i].fence = m_device->CreateFence(true);
		m_presenterSync[i].imageAvailable = m_device->CreateSemaphore();
		m_presenterSync[i].renderFinished = m_device->CreateSemaphore();
	}

	m_imagesInFlight.resize(m_images.size(), nullptr);

	m_frameCounter = 0;
	m_imageIndex = 0;
	m_maxImages = static_cast< uint32_t >(m_images.size());
	m_init = true;
}

ESwapchainResult RenderPresenter::AcquireNextImage(RenderPresenterSync &sync, uint32_t &imageIndex)
{
	m_presenterSync[m_frameCounter].fence->Wait(UINT64_MAX);

	ESwapchainResult result = m_swapchain->GetAvailableImage(m_presenterSync[m_frameCounter].imageAvailable, nullptr, UINT64_MAX, m_imageIndex);

	if (m_imagesInFlight[m_imageIndex] != nullptr)
		m_imagesInFlight[m_imageIndex]->Wait(UINT64_MAX);

	m_imagesInFlight[m_imageIndex] = m_presenterSync[m_frameCounter].fence;
	m_presenterSync[m_frameCounter].fence->Reset();

	imageIndex = m_imageIndex;
	sync = m_presenterSync[m_frameCounter];

	return result;
}

ESwapchainResult RenderPresenter::QueuePresent()
{
	ESwapchainResult result = m_swapchain->PresentImage(m_presenterSync[m_frameCounter].renderFinished, m_imageIndex);
	return result;
}
