#include "runtime.h"

namespace Renderer {

	Runtime::Runtime() : Service("RenderService")
	{
		rhi = nullptr;
	}

	bool Runtime::Setup()
	{
		if (rhi->SetGraphicsMode(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, false, false, false, true, 4) != false)
			Initialized = true;

		
		
		return Initialized;
	}

	bool Runtime::Update()
	{
		return true;
	}

	void Runtime::Release()
	{
		rhi->Close();
	}

	void Runtime::SetRenderer(RHI *rhi)
	{
		if (!rhi)
			return;
		
		this->rhi = rhi;
	}

}
