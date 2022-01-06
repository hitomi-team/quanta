#ifndef LEVEL0_RENDERER_SERVICE_H
#define LEVEL0_RENDERER_SERVICE_H

#include "level0/game/game_service.h"

#include "api.h"
#include "presenter.h"

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

	std::unique_ptr< RenderPresenter > m_presenter;
	std::shared_ptr< IRenderCommandPool > m_commandpool;
	std::vector< std::shared_ptr< IRenderCommandBuffer > > m_commandbufs;

	std::shared_ptr< IRenderImGui > m_imgui;
	std::shared_ptr< IRenderPass > m_imguiRenderPass;

	PerfClock m_pc;
	double m_lag = 0.;

	void RedoCommandBuffers();
};

#endif
