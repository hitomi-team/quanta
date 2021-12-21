#ifndef LEVEL0_RENDER_PRESENTER_H
#define LEVEL0_RENDER_PRESENTER_H

#include "api.h"

struct RenderPresenterSync {
	std::shared_ptr< IRenderSemaphore > imageAvailable, renderFinished;
	std::shared_ptr< IRenderFence > fence;
};

class RenderPresenter {
public:
	RenderPresenter() = delete;

	RenderPresenter(std::shared_ptr< IRenderDevice > device, ESwapchainPresentMode presentMode);
	~RenderPresenter();

	void Recreate(ESwapchainPresentMode presentMode);

	ESwapchainResult AcquireNextImage(RenderPresenterSync &sync, uint32_t &imageIndex);
	ESwapchainResult QueuePresent();
	inline void IncrementFrameCounter() { m_frameCounter = (m_frameCounter + 1) % m_maxImages; }

	inline std::shared_ptr< IRenderPass > GetRenderPass() { return m_renderpass; }
	inline std::shared_ptr< IRenderImage > GetImage(uint32_t imageIndex) { return m_images[imageIndex]; }
	inline std::shared_ptr< IRenderFramebuffer > GetFramebuffer(uint32_t imageIndex) { return m_framebuffers[imageIndex]; }
	inline uint32_t GetFrameCounter() { return m_frameCounter; }
	inline uint32_t GetMaxImages() { return m_maxImages; }
private:
	std::shared_ptr< IRenderDevice > m_device;
	std::shared_ptr< IRenderSwapchain > m_swapchain;
	std::shared_ptr< IRenderPass > m_renderpass;
	std::vector< std::shared_ptr< IRenderImage > > m_images;
	std::vector< std::shared_ptr< IRenderFramebuffer > > m_framebuffers;
	std::vector< std::shared_ptr< IRenderFence > > m_imagesInFlight;
	std::vector< RenderPresenterSync > m_presenterSync;
	uint32_t m_maxImages = 0, m_frameCounter = 0, m_imageIndex = 0;
	bool m_init = false;
};

#endif
