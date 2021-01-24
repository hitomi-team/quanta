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
	// TODO: put this somewhere else
	struct {
		Renderer::RendererType type;
	} options = {
#if defined(__D3D11)
		Renderer::RENDERER_D3D11
#else
		Renderer::RENDERER_VULKAN
#endif
	};

	for (int i = 1; i < argc; i++) {
		if (std::strcmp(argv[i], "-vulkan") == 0) {
			options.type = Renderer::RENDERER_VULKAN;
		}// else if (std::strcmp(argv[i], "-opengl") == 0) {
		//	options.type = Renderer::RENDERER_OPENGL;
		//}
#if defined(__D3D11)
		else if (std::strcmp(argv[i], "-d3d11") == 0) {
			options.type = Renderer::RENDERER_D3D11;
		}
#endif
	}


	// Setup filesystem service
	Filesystem::Runtime filesystem;
	filesystem.argv0 = argv[0];

	// Setup render service
#if defined(__D3D11)
	Renderer::D3D11Renderer d3d11renderer;
#endif
	Renderer::VulkanRenderer vulkanrenderer;
	// Renderer::OpenGLRenderer openglrenderer;
	// Renderer::NullRenderer nullrenderer;

	Renderer::Runtime renderer;
#if defined(__D3D11)
	if (options.type == Renderer::RENDERER_D3D11)
		renderer.SetRenderer(&d3d11renderer);
#endif
	if (options.type == Renderer::RENDERER_VULKAN)
		renderer.SetRenderer(&vulkanrenderer);
	//else if (options.type == Renderer::RENDERER_OPENGL)
	//	renderer.SetRenderer(&openglrenderer);

	Renderer::Input input;

	Graph::Game game;
	Graph::g_gamePtr = &game;

	game.registerService(&filesystem);
	game.registerService(&renderer);
	game.registerService(&input);
	game.setupServices();

	static const Renderer::Vertex vertices[] = {
		Renderer::Vertex(glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		Renderer::Vertex(glm::vec3( 1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		Renderer::Vertex(glm::vec3( 1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
		Renderer::Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f))
	};

	static const uint16_t indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	Renderer::MaterialJSON parser;

	Renderer::Material *newmat = parser.Load(renderer, "/materials/tri.json");
	if (!newmat) {
		game.Abort();
		return 0;
	}

	Renderer::Mesh newmesh;
	if (!newmesh.Setup(renderer.GetRenderer(), vertices, 4, indices, 6, Renderer::MESH_2D)) {
		game.Abort();
		return 0;
	}

	renderer.RegisterMesh(&newmesh);
	renderer.RegisterMaterial(newmat);

	Renderer::Prop *newprop = renderer.AllocateProp(0, 0); (void)newprop;

	// Finally we can run the game
	// resources will be cleaned up on exit
	game.Run();

	return 0;
}
