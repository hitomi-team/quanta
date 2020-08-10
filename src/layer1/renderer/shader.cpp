#include "pch/pch.h"

#include "shader.h"

namespace Renderer {

	void *Shader::GetShaderObject(ShaderType type)
	{
		switch (type) {
		case VS:
			return objects[0];
		case FS:
			return objects[1];
		case GS:
			return objects[2];
		default:
			break;
		}

		return nullptr;
	}

}
