#include "pch/pch.h"

#include "layer1/log.h"
#include "layer1/graph/game.h"

#include "layer1/filesystem/runtime.h"

#include "layer1/renderer/stb_image.h"
#include "layer1/renderer/runtime.h"
#include "layer1/renderer/input.h"
#include "layer1/renderer/material.h"
#include "layer1/renderer/mesh.h"

// SDL2main
int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	// Setup filesystem service
	Filesystem::Runtime filesystem;

	// Setup render service
	// TODO: Add option to switch between supported renderers
	// Also TODO: Add command arguments to do this which is a lot better
#if defined(__D3D11)
	Renderer::D3D11Renderer renderer_api;
#else	// use vulkan on lunix
	Renderer::VulkanRenderer renderer_api;
#endif
	Renderer::Runtime renderer;
	renderer.SetRenderer(&renderer_api);

	Renderer::Input input;

	Graph::Game game;
	game.registerService(&filesystem);
	game.registerService(&renderer);
	game.registerService(&input);
	game.setupServices();

	Renderer::Vertex vertices[] = {
		Renderer::Vertex(glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		Renderer::Vertex(glm::vec3( 1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		Renderer::Vertex(glm::vec3( 1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
		Renderer::Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f))
	};

	unsigned indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	// Shader Setup

	auto h = filesystem.FindFile("/shaders/tri_vs.dxbc");
	size_t vs_size = filesystem.GetFileSize(h);
	char *vs_bytecode = new char[vs_size + 1];
	filesystem.ReadFile(h, vs_bytecode, vs_size);
	h = filesystem.FindFile("/shaders/tri_fs.dxbc");
	size_t fs_size = filesystem.GetFileSize(h);
	char *fs_bytecode = new char[fs_size + 1];
	filesystem.ReadFile(h, fs_bytecode, fs_size);

	Renderer::Shader *shader = renderer_api.CreateShader((unsigned char *)vs_bytecode, vs_size, (unsigned char *)fs_bytecode, fs_size);

	delete[] vs_bytecode;
	delete[] fs_bytecode;

	// Texture Setup

	int x, y, channels;
	h = filesystem.FindFile("/textures/pfp.png");
	size_t len = filesystem.GetFileSize(h);
	char *tex_buf = new char[len];
	filesystem.ReadFile(h, tex_buf, len);
	unsigned char *pix_data = stbi_load_from_memory((unsigned char *)tex_buf, (int)len, &x, &y, &channels, 4);

	Renderer::SamplerStateDesc desc = {};
	desc.Filter = Renderer::FILTER_NEAREST;
	desc.AddressModeU = Renderer::ADDRESS_MIRROR;
	desc.AddressModeV = Renderer::ADDRESS_MIRROR;
	desc.AddressModeW = Renderer::ADDRESS_MIRROR;
	desc.ComparisonFunc = Renderer::TCF_LESSEQUAL;
	desc.MipLODBias = 0;
	desc.MaxLOD = 0;
	desc.MinLOD = 0;
	desc.MaxAniso = 0;

	Renderer::Texture2D *tex = renderer_api.CreateTexture2D(pix_data, x, y, desc);

	stbi_image_free(pix_data);
	delete[] tex_buf;

	// Actual Renderer Usage

	Renderer::Material newmat;
	newmat.Setup(shader, tex);

	Renderer::Mesh newmesh;
	newmesh.Setup(renderer.GetRenderer(), vertices, 4, indices, 6, Renderer::MESH_2D);

	renderer.RegisterMesh(&newmesh);
	renderer.RegisterMaterial(&newmat);

	Renderer::Prop *newprop = renderer.AllocateProp(0, 0);

	// Finally we can run the game

	game.Run();

	newprop->Release();
	newmat.Release();
	newmesh.Release();

	return 0;
}
