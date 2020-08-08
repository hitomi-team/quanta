#ifndef RENDERER_INDEXBUFFER_H
#define RENDERER_INDEXBUFFER_H

namespace Renderer {

	class IndexBuffer {
	public:
		IndexBuffer() {}
		virtual ~IndexBuffer() {}
	
		virtual bool SetData(unsigned *indices, unsigned count) { (void)indices; this->count = count; return true; }
		virtual void Release() {}

		inline unsigned GetCount() { return count; }
		inline void *GetBuffer() { return buffer; }

	protected:
		void *buffer;
		unsigned count;
	};

}

#endif
