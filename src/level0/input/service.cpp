#include "level0/pch.h"
#include "level0/log.h"

#include "level0/game/game.h"
#include "level0/cvarcmd/service.h"
#include "level0/render/service.h"
#include "service.h"

#include "level0/dependencies/imgui/imgui_impl_sdl.h"

InputService::InputService() : GameService("InputService")
{
#ifndef NDEBUG
	std::thread([this]{ this->ConsoleThread(); }).detach();
#endif
}

void InputService::Update()
{
	SDL_Event event;
	bool recoverFromWait = false;

	auto renderService = g_Game->GetService< RenderService >();
	auto cvarCmdService = g_Game->GetService< CVarCmdService >();

	while (renderService != nullptr) {
		if (recoverFromWait) {
			recoverFromWait = false;
		} else {
			if (SDL_PollEvent(&event) <= 0)
				break;
		}

		ImGui_ImplSDL2_ProcessEvent(&event);

		switch (event.type) {
		case SDL_QUIT:
			g_Game->RequestClose();
			break;
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				renderService->TryResizeSwapchain();
				break;
			case SDL_WINDOWEVENT_MINIMIZED:
				recoverFromWait = true;
				SDL_WaitEvent(&event);
				break;
			}
			break;
		}
	}

#ifndef NDEBUG
	if (m_consoleLines.size() > 0) {
		std::lock_guard< std::mutex > lock(m_consoleLock);

		std::string cmd = m_consoleLines.front();
		m_consoleLines.pop();

		cmd.erase(cmd.size() - 1);
		cvarCmdService->Exec(cmd);
	}
#endif
}

#ifndef NDEBUG
void InputService::ConsoleThread()
{
	std::array< char, 1024 > buf {};

	while (1) {
		Sys_GetConsoleInput(buf.data(), buf.size());

		std::lock_guard< std::mutex > lock(m_consoleLock);
		m_consoleLines.emplace(buf.data());
	}
}
#endif
