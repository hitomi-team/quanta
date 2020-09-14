#include "pch/pch.h"

#include "layer1/log.h"
#include "layer1/graph/game.h"

#include "layer1/filesystem/runtime.h"

#include "layer1/renderer/stb_image.h"
#include "layer1/renderer/runtime.h"
#include "layer1/renderer/input.h"
#include "layer1/renderer/material.h"
#include "layer1/renderer/mesh.h"

#include "layer1/renderer/materialjson.h"

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

	Renderer::MaterialJSON parser;

	Renderer::Material *newmat = parser.Load(filesystem, renderer, "/materials/tri.json");
	if (!newmat) {
		return 0;
	}

	Renderer::Mesh newmesh;
	if (!newmesh.Setup(renderer.GetRenderer(), vertices, 4, indices, 6, Renderer::MESH_2D))
		return 0;

	renderer.RegisterMesh(&newmesh);
	renderer.RegisterMaterial(newmat);

	Renderer::Prop *newprop = renderer.AllocateProp(0, 0);

	// Finally we can run the game

	game.Run();

	newprop->Release();
	newmat->Release();
	newmesh.Release();

	return 0;
}
