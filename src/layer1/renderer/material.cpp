#include "pch/pch.h"

#include "material.h"

namespace Renderer {

	bool Material::Setup(Shader *shader, Texture2D *texture)
	{
		if (!shader || !texture)
			return false;

		this->shader = shader;
		this->texture = texture;

		return true;
	}

	void Material::Release()
	{
		if (!shader || !texture)
			return;

		shader->Release();
		texture->Release();
	}

	void Material::Bind(RHI *rhi)
	{
		if (!rhi || !shader || !texture)
			return;

		rhi->SetShaders(shader);
		rhi->SetTexture(0, texture);
	}

}
