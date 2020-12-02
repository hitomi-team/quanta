#include "pch/pch.h"

#include "prop.h"

namespace Renderer {

	void Prop::Setup(Mesh *mesh, Material *material)
	{
		this->mesh = mesh;
		this->material = material;
	}

	void Prop::Release()
	{
		// Don't destroy mesh or material, those are managed by the renderer.
	}

	void Prop::Draw(RHI *rhi)
	{
		/* TODO: Reduce amount of function calls

		rhi->SetShaders(shader);
		rhi->SetTexture(0, texture);

		if (!rhi || !vbuffer)
			return;

		rhi->SetVertexBuffer(vbuffer);
		rhi->SetIndexBuffer(ibuffer);

		if (indexDraw) {
			rhi->DrawIndexed(TRIANGLE_LIST, 0, getIndexCount());
		} else {
			rhi->Draw(TRIANGLE_LIST, 0, getVertexCount());
		}

		*/

		material->Bind(rhi);
		mesh->Bind(rhi);
		mesh->Draw(rhi);
	}

}
