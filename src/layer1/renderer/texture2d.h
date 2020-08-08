#ifndef RENDERER_TEXTURE2D_H
#define RENDERER_TEXTURE2D_H

#include "defs.h"

namespace Renderer {


	// All textures will have to be in RGBA format.
	class Texture2D {
	public:
		Texture2D() { buffer = nullptr; sampler = nullptr; }
		virtual ~Texture2D() {}

		virtual bool SetData(unsigned char *data, unsigned width, unsigned height, SamplerStateDesc samplerstatedesc) { return false; }
		virtual void Release() {}

		inline void *GetView() { return buffer; }
		inline void *GetSampler() { return sampler; }

	protected:
		void *buffer;
		void *sampler;
	};

}

#endif
