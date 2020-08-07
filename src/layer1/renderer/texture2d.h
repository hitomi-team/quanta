#ifndef RENDERER_TEXTURE2D_H
#define RENDERER_TEXTURE2D_H

namespace Renderer {


	// All textures will have to be in RGBA format.
	class Texture {
	public:
		Texture() {}
		bool SetData(unsigned char *data, unsigned width, unsigned height, int channels);
	
	protected:
		void *buffer;
	};

}

#endif
