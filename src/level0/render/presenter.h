#ifndef LEVEL0_RENDER_PRESENTER_H
#define LEVEL0_RENDER_PRESENTER_H

#include "api.h"

struct RenderPresenterSync {
	std::shared_ptr< IRenderSemaphore > imageAvailable, renderFinished;
	std::shared_ptr< IRenderFence > fence;
};

class RenderPresenter {
public:
	std::shared_ptr< IRenderSwapchain > m_swapchain;

	RenderPresenter() = delete;

	RenderPresenter(std::shared_ptr< IRenderDevice > device, ESwapchainPresentMode presentMode);
	~RenderPresenter();

	void Recreate(ESwapchainPresentMode presentMode);

	ESwapchainResult AcquireNextImage(RenderPresenterSync &sync, uint32_t &imageIndex);
	ESwapchainResult QueuePresent();
	inline void IncrementFrameCounter() { m_frameCounter = (m_frameCounter + 1) % m_swapchain->numImages; }
	inline uint32_t GetFrameCounter() { return m_frameCounter; }
private:
	std::shared_ptr< IRenderDevice > m_device;
	std::vector< std::shared_ptr< IRenderFence > > m_imagesInFlight;
	std::vector< RenderPresenterSync > m_presenterSync;
	uint32_t m_frameCounter = 0, m_imageIndex = 0;
	bool m_init = false;
};

#endif
