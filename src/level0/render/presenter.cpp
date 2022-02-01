#include "level0/pch.h"

#include "presenter.h"

RenderPresenter::RenderPresenter(std::shared_ptr< IRenderDevice > device, ESwapchainPresentMode presentMode)
{
	m_device = device;
	m_swapchain = m_device->CreateSwapchain(presentMode);
	this->Recreate(presentMode);
}

RenderPresenter::~RenderPresenter()
{
}

void RenderPresenter::Recreate(ESwapchainPresentMode presentMode)
{
	if (m_init) {
		m_device->WaitIdle();
		m_swapchain->Recreate(presentMode);
		IO_EraseCPPVector(m_imagesInFlight);
		IO_EraseCPPVector(m_presenterSync);
	}

	m_presenterSync.resize(m_swapchain->numImages);
	for (uint32_t i = 0; i < m_swapchain->numImages; i++) {
		m_presenterSync[i].fence = m_device->CreateFence(true);
		m_presenterSync[i].imageAvailable = m_device->CreateSemaphore();
		m_presenterSync[i].renderFinished = m_device->CreateSemaphore();
	}

	m_imagesInFlight.resize(m_swapchain->numImages, nullptr);

	m_frameCounter = 0;
	m_imageIndex = 0;
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
