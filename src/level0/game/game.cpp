#include "level0/pch.h"
#include "level0/log.h"

#include "level0/sys.h"

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
	m_services[UtilStringHash(service->name)] = std::move(service);
}

void Game::UpdateServices()
{
	for (auto &service : m_services)
		service.second->Update();
}

void Game::Run()
{
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

Game *g_Game = nullptr;
