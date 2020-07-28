#include "shader.h"

namespace Renderer {

	void *Shader::GetObject(ShaderType type)
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
