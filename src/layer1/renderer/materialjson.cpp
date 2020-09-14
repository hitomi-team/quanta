#include "pch/pch.h"

#include "materialjson.h"

/*

{
	"shaders" : [
		{
			"shader" : "tri_vs",
			"type"   : "vertex",

			"defaultparams" : {
				"mvp" : "mat4"
			}
		},
		{
			"shader": "tri_fs",
			"type"  : "fragment",
			
			"defaultparams" : {
				"albedo" : "/textures/pfp.png"
			},

			"customparams" : {
				"time"  : "float"
			}

		}
	]
}

*/

namespace Renderer {

	Shader *MaterialJSON::parseShaderBytecode(Filesystem::Runtime &fsRuntime, Renderer::Runtime &rsRuntime, const std::string &vs_filePath, const std::string &fs_filePath)
	{
		auto h = fsRuntime.FindFile(vs_filePath);
		if (h == -1) {
			global_log.Error(StringFormat("Cannot find Vertex Shader: ", vs_filePath));
			return nullptr;
		}

		size_t  vs_size = fsRuntime.GetFileSize(h);
		char *vs_bytecode = new char[vs_size + 1];
		fsRuntime.ReadFile(h, vs_bytecode, vs_size);

		h = fsRuntime.FindFile(fs_filePath);
		if (h == -1) {
			global_log.Error(StringFormat("Cannot find Fragment Shader: ", fs_filePath));
			return nullptr;
		}

		size_t  fs_size = fsRuntime.GetFileSize(h);
		char *fs_bytecode = new char[fs_size + 1];
		fsRuntime.ReadFile(h, fs_bytecode, fs_size);

		Shader *shader = rsRuntime.GetRenderer()->CreateShader((unsigned char *)vs_bytecode, vs_size, (unsigned char *)fs_bytecode, fs_size);
		if (!shader)
			shader = nullptr;
		
		delete[] vs_bytecode;
		delete[] fs_bytecode;

		return shader;
	}

	Texture2D *MaterialJSON::loadTexture(Filesystem::Runtime &fsRuntime, Renderer::Runtime &rsRuntime, const std::string &filePath)
	{
		auto h = fsRuntime.FindFile(filePath);
		if (h == -1) {
			global_log.Error(StringFormat("Cannot find Texture: ", filePath));
			return nullptr;
		}

		size_t image_size = fsRuntime.GetFileSize(h);
		unsigned char *image = new unsigned char[image_size];
		fsRuntime.ReadFile(h, (char *)image, image_size);

		int x, y, channels;
		unsigned char *pixels = stbi_load_from_memory((unsigned char *)image, (int)image_size, &x, &y, &channels, 4);

		SamplerStateDesc desc = {};
		desc.Filter = FILTER_NEAREST;
		desc.AddressModeU = ADDRESS_MIRROR;
		desc.AddressModeV = ADDRESS_MIRROR;
		desc.AddressModeW = ADDRESS_MIRROR;
		desc.ComparisonFunc = TCF_LESSEQUAL;
		desc.MipLODBias = 0;
		desc.MaxLOD = 0;
		desc.MinLOD = 0;
		desc.MaxAniso = 0;

		Texture2D *texture = rsRuntime.GetRenderer()->CreateTexture2D(pixels, x, y, desc);

		stbi_image_free(pixels);
		delete[] image;

		return texture;
	}

	Material *MaterialJSON::Load(Filesystem::Runtime &fsRuntime, Renderer::Runtime &rsRuntime, const std::string &jsonPath)
	{
		rapidjson::Document doc;

		// Shader Properties

		auto h = fsRuntime.FindFile(jsonPath);
		if (h == -1) {
			global_log.Error(StringFormat("Cannot find Material: ", jsonPath));
			return nullptr;
		}

		size_t jsonLength = fsRuntime.GetFileSize(h);
		char *jsonData = new char[jsonLength + 1];
		fsRuntime.ReadFile(h, jsonData, jsonLength);

		doc.Parse(jsonData, jsonLength);
		if (doc.HasParseError()) {
			global_log.Error(StringFormat("Failed to parse JSON: ", jsonPath));
			return nullptr;
		}

		delete[] jsonData;

		const rapidjson::Value &shaders = doc["shaders"];
		std::string filepath;
		std::vector<ShaderParameterElement> uniforms;
		Texture2D *albedo = nullptr;
		Shader *shader = nullptr;
		
		switch (rsRuntime.GetRenderer()->getRendererType()) {
		case RENDERER_D3D11:
			filepath = "/materials/shadersd3d11/";
			break;
		case RENDERER_VULKAN:
			filepath = "/materials/shadersvulkan/";
			break;
		case RENDERER_NULL:
		default:
			global_log.Error("Unsupported Renderer... How did you even launch this!?");
			return nullptr;
		}

		std::string vsPath(filepath);
		std::string fsPath(filepath);

		// Parse each shader!
		for (rapidjson::SizeType i = 0; i < shaders.Size(); i++) {
			const rapidjson::Value &shaderType = shaders[i];			
			const std::string shaderTypeStr = shaderType["type"].GetString();
			const std::string shaderName = shaderType["shader"].GetString();

			const rapidjson::Value &uniformValues = shaderType["params"];
			for (rapidjson::SizeType i = 0; i < uniformValues.Size(); i++) {
				ShaderParameterElement element = {};

				if (uniformValues[i].HasMember("mvp")) {
					glm::mat4 matrix;

					element.data = (char *)&matrix; // fill with weird data. will set it to default vals later
					element.dataSize = sizeof(glm::mat4);
					element.usage = SHADER_PARAM_MVP;
				} else if (uniformValues[i].HasMember("time")) {
					float time = 0.f;

					element.data = (char *)&time;
					element.dataSize = sizeof(float);
					element.usage = SHADER_PARAM_TIME;
				} else if (uniformValues[i].HasMember("albedo")) {
					const std::string albedoPath = uniformValues[i]["albedo"].GetString();
					albedo = loadTexture(fsRuntime, rsRuntime, albedoPath);
				
					if (!albedo) {
						global_log.Error(StringFormat("Cannot load albedo texture: \"", albedoPath, "\" requested by Material: \"", jsonPath, "\""));
						return nullptr;
					}
				} else {
					global_log.Warn(StringFormat("Unsupported Shader Parameter: \"", uniformValues[i].GetString(), "\""));
				}

				if (element.dataSize)
					uniforms.push_back(element);

			}

			// Process filename to retrieve bytecode
			// Shaders will always be in:
			// /materials/shadersd3d11/[shadername].dxbc

			if (shaderTypeStr == "vertex") {
				vsPath = StringFormat(filepath, shaderName, ".dxbc");
			} else if (shaderTypeStr == "fragment") {
				fsPath = StringFormat(filepath, shaderName, ".dxbc");
			} else {
				global_log.Error(StringFormat("Unknown Shader Type: \"", shaderTypeStr, "\" in Material: \"", jsonPath, "\""));
			}
		}

		shader = parseShaderBytecode(fsRuntime, rsRuntime, vsPath, fsPath);

		if (!shader) {
			if (albedo)
				albedo->Release();
			
			return nullptr;
		}

		ShaderParameterBuffer *paramBuffer = rsRuntime.GetRenderer()->CreateShaderParameterBuffer(uniforms);

		if (!paramBuffer) {
			if (albedo)
				albedo->Release();
			
			shader->Release();

			global_log.Error("paramBuffer is null");

			return nullptr;
		}

		Material *mat = new Material;
		mat->Setup(shader, albedo, paramBuffer);

		if (!mat) {
			if (albedo)
				albedo->Release();
			
			shader->Release();
			paramBuffer->Release();

			global_log.Error("mat is null");
		}

		return mat;

	}

}
