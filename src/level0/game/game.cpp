#include "level0/pch.h"
#include "level0/log.h"

#include "level0/sys.h"
#include "level0/cmd.h"
#include "level0/cvar.h"

#include "game.h"

Game::Game()
{
	g_Log.Info("Game Init");
}

Game::~Game()
{
	g_Log.Info("Game Quit");
}

void Game::RegisterService(std::unique_ptr< GameService > &&service)
{
	g_Log.Info("Registered Service: {}\n", service->name);
	m_services.push_back(std::move(service));
}

void Game::UpdateServices()
{
	for (auto &service : m_services) {
		service->Update();
	}
}

void Game::Run()
{
	Cmd_Add("quit", [this]() -> int {
		this->RequestClose();
		return 0;
	});

	Cmd_Add("exit", [this]() -> int {
		this->RequestClose();
		return 0;
	});

	while (!m_shouldClose)
		this->UpdateServices();
}

void Game::RequestClose()
{
	m_shouldClose = true;
}

void Game::Abort(std::string cause)
{
	throw std::runtime_error(cause);
}

GameService *Game::GetService(const char *name)
{
	for (auto &service : m_services) {
		if (SDL_strcmp(name, service->name) == 0)
			return service.get();
	}

	return nullptr;
}

Game *g_Game = nullptr;
