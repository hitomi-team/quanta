#ifndef RENDERER_GPU_RESOURCE_H
#define RENDERER_GPU_RESOURCE_H

namespace Renderer {

	class GPUResource {
	public:
		GPUResource() {}
		virtual ~GPUResource() {}

		virtual void Release() {}
		virtual void ClearData() {}
		virtual bool isDataLost() const = 0;
		virtual bool HasPendingData() const = 0;
	};

}

#endif
