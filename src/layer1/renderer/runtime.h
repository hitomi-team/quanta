#ifndef RENDERER_RUNTIME_H
#define RENDERER_RUNTIME_H

#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "../graph/game.h"
#include "../log.h"
#include "imgui/imgui_impl_sdl.h"
#include "subsystems/rhi.h"
#include "mesh.h"
#include "material.h"
#include "prop.h"

#if defined(__VULKAN)
#include "subsystems/vulkan/vulkanrenderer.h"
#elif defined(__D3D11)
#include "subsystems/d3d11/d3d11renderer.h"
#include "subsystems/vulkan/vulkanrenderer.h"
#else
#error A renderer must be defined
#endif

namespace Renderer {

	class Runtime : public Graph::Service {
	public:
		Runtime();

		// overridden service functions
		bool Setup();
		bool Update();
		void Release();

		void SetRenderer(RHI *rhi);
		inline RHI *GetRenderer() { return rhi; }

		inline void RegisterMesh(Mesh *mesh) { meshes.push_back(mesh); }
		inline void RegisterMaterial(Material *material) { materials.push_back(material); }
	
		Prop *AllocateProp(unsigned meshidx, unsigned materialidx);
		void ReleaseProp(Prop *prop);

		void __debug_menu();

	private:
		RHI *rhi;

		std::vector<Prop *> prop_queue; // Rendered every frame
		std::vector<Mesh *> meshes;
		std::vector<Material *> materials;
	};

}

#endif
