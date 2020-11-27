#ifndef RENDERER_INDEXBUFFER_H
#define RENDERER_INDEXBUFFER_H

namespace Renderer {

	class IndexBuffer {
	public:
		inline IndexBuffer() {}
		inline virtual ~IndexBuffer() {}
	
		virtual bool SetData(const uint16_t *indices, unsigned count) = 0;
		virtual void Release() = 0;

		inline unsigned GetCount() { return count; }
		inline void *GetBuffer() { return buffer; }

	protected:
		void *buffer;
		unsigned count;
	};

}

#endif
