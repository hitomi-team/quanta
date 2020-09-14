#ifndef RENDERER_RENDERTARGET_H
#define RENDERER_RENDERTARGET_H

#include "defs.h"

namespace Renderer {

	class RenderTarget {
	public:
		inline RenderTarget() { view = nullptr; }
		virtual ~RenderTarget() {}
	
		virtual bool Setup(unsigned width, unsigned height, TextureUsage usage) = 0;
		virtual void Release() = 0;

		inline void *getView() { return view; }

	protected:
		void *view;
	};

}

#endif
