#include "mesh.h"

namespace Renderer {

	bool Mesh::Setup(RHI *rhi, Vertex *vertices, unsigned count, MeshType type)
	{
		(void)type;

		if (!rhi || !vertices || !count)
			return false;
		
		vbuffer = rhi->CreateVertexBuffer(vertices, count);

		if (!vbuffer)
			return false;
		
		return true;
	}

	bool Mesh::Setup(RHI *rhi, Vertex *vertices, unsigned vcount, unsigned *indices, unsigned icount, MeshType type)
	{
		(void)type;

		if (!rhi || !vertices || !vcount || !indices || !icount)
			return false;
		
		vbuffer = rhi->CreateVertexBuffer(vertices, vcount);
		ibuffer = rhi->CreateIndexBuffer(indices, icount);

		if (!vbuffer || !ibuffer)
			return false;
		
		indexDraw = true;

		return true;
	}

	void Mesh::Release()
	{
		if (ibuffer) {
			ibuffer->Release();
			ibuffer = nullptr;
			indexDraw = false;
		}

		if (vbuffer) {
			vbuffer->Release();
			vbuffer = nullptr;
		}
	}

	void Mesh::Bind(RHI *rhi)
	{
		if (!rhi || !vbuffer)
			return;
		
		rhi->SetVertexBuffer(vbuffer);
		rhi->SetIndexBuffer(ibuffer); // This is safe even if ibuffer is nullptr
	}

	void Mesh::Draw(RHI *rhi)
	{
		if (indexDraw) {
			if (ibuffer == nullptr)
				return;

			rhi->DrawIndexed(TRIANGLE_LIST, 0, getIndexCount());
		} else {
			if (vbuffer == nullptr)
				return;

			rhi->Draw(TRIANGLE_LIST, 0, getVertexCount());
		}
	}

	void Mesh::DrawInstanced(RHI *rhi, unsigned instances)
	{
		rhi->DrawInstanced(TRIANGLE_LIST, 0, getIndexCount(),  instances);
	}

}
