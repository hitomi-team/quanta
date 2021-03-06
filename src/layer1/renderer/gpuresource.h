#ifndef RENDERER_GPU_RESOURCE_H
#define RENDERER_GPU_RESOURCE_H

namespace Renderer {

	class GPUResource {
	public:
		inline GPUResource() {}
		inline virtual ~GPUResource() {}

		virtual void Release() = 0;
		virtual void ClearData() = 0;
		virtual bool isDataLost() const = 0;
		virtual bool HasPendingData() const = 0;
	};

}

#endif
