#pragma once

#include "pch/pch.h"
#include "defs.h"

namespace Renderer {

	struct ShaderParameterElement {
		char *data;
		size_t dataSize;
		ShaderParameterUsage usage;
	};

	class ShaderParameterBuffer {
	public:
		ShaderParameterBuffer() {}
		virtual ~ShaderParameterBuffer() {}

		virtual bool Setup(std::vector<ShaderParameterElement> elements) { (void)elements; return true; }
		virtual void Release() = 0;
		virtual void Apply() = 0;
		
		virtual void Map() = 0;
		virtual void Flush() = 0;
		virtual void Unmap() = 0;

		virtual void SetShaderParameter(ShaderParameterUsage param, float value) = 0;
//		virtual void SetShaderParameter(ShaderParameterUsage param, bool value) = 0;
//		virtual void SetShaderParameter(ShaderParameterUsage param, const glm::vec2& vector) = 0;
//		virtual void SetShaderParameter(ShaderParameterUsage param, const glm::mat3& matrix) = 0;
//		virtual void SetShaderParameter(ShaderParameterUsage param, const glm::vec3& vector) = 0;
		virtual void SetShaderParameter(ShaderParameterUsage param, const glm::mat4& matrix) = 0;
//		virtual void SetShaderParameter(ShaderParameterUsage param, const glm::vec4& vector) = 0;

	protected:

		std::vector<ShaderParameterElement> elements;
	};

}
