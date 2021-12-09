#ifndef LEVEL0_RENDERER_SERVICE_H
#define LEVEL0_RENDERER_SERVICE_H

#include "level0/game/game_service.h"

#include "api.h"

class RenderService : public GameService {
public:
	std::unique_ptr< RenderAPI > renderAPI = nullptr;

	RenderService();
	RenderService(int api);
	~RenderService();

	void Update();
	inline double GetDeltaTime() { return m_pc.dt_f64; }

	void TryResizeSwapchain();

private:
	bool m_init = false;

	std::shared_ptr< IRenderDevice > m_device;
	std::shared_ptr< IRenderSwapchain > m_swapchain;
	std::shared_ptr< IRenderCommandPool > m_commandpool;
	std::shared_ptr< IRenderPass > m_renderpass;
	std::vector< std::shared_ptr< IRenderImage > > m_swapchainImages;
	std::vector< std::shared_ptr< IRenderFence > > m_swapchainInFlightFences, m_imagesInFlight;
	std::vector< std::shared_ptr< IRenderSemaphore > > m_swapchainImageAvailable, m_renderFinished;
	std::vector< std::shared_ptr< IRenderCommandBuffer > > m_commandbufs;
	std::vector< std::shared_ptr< IRenderFramebuffer > > m_swapchainFramebuffers;

	PerfClock m_pc;
	double m_lag = 0.;

	size_t m_frameCounter = 0;

	void ReinitSwapchain();

};

#endif
