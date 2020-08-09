#ifndef RENDERER_MESH_H
#define RENDERER_MESH_H

#include "subsystems/rhi.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"

namespace Renderer {
	
	enum MeshType {
		MESH_2D = 0,
		MESH_3D,
		MESH_TEXT,
		MESH_UNDEFINED
	};

	class Mesh {
	public:
		inline Mesh() { type = MESH_UNDEFINED; ibuffer = nullptr; vbuffer = nullptr; indexDraw = false; }

		bool Setup(RHI *rhi, Vertex *vertices, unsigned count, MeshType type);
		bool Setup(RHI *rhi, Vertex *vertices, unsigned vcount, unsigned *indices, unsigned icount, MeshType type);
		void Release();

		void Bind(RHI *rhi);
		void Draw(RHI *rhi);
		void DrawInstanced(RHI *rhi, unsigned instances);

		inline MeshType getType() { return type; }
		inline unsigned getIndexCount() { return ibuffer->GetCount(); }
		inline unsigned getVertexCount() { return vbuffer->GetCount(); }
		inline bool getIndexDraw() { return indexDraw; }

	protected:
		MeshType type;
		IndexBuffer *ibuffer;
		VertexBuffer *vbuffer;
		bool indexDraw;
	};

}

#endif
