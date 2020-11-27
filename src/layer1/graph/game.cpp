#include "pch/pch.h"

#include "../log.h"
#include "game.h"

namespace Graph {

	void Game::registerService(Service *service)
	{
		if (!service)
			return;

		Services.push_back(service);
	}

	void Game::setupServices()
	{
		for (auto &service : this->Services) {
			if (!service->isInitialized()) {
				if (!service->Setup()) {
					global_log.Error(FMT_STRING("Failed to initialize service: {}"), service->getName());
					Abort();
				}

				service->setInitialized();
				global_log.Info(FMT_STRING("Service initialized: {}"), service->getName());
			}
		}
	}

	void Game::runServices()
	{
		for (auto &service : this->Services) {
			if (!service->isInitialized()) {
				global_log.Warn(FMT_STRING("Service was not initialized before calling runServices(): {}"), service->getName());

				if (!service->Setup()) {
					global_log.Error(FMT_STRING("Failed to initialize service: {}"), service->getName());
					Abort();
				}

				service->setInitialized();
			}

			if (!service->Update()) {
				global_log.Error(FMT_STRING("Service failed to run properly: {}"), service->getName());
				return;
			}
		}
	}

	void Game::killService(Service *service)
	{
		if (!service)
			return;

		Services.erase(std::remove(Services.begin(), Services.end(), service), Services.end());
		service->Release();
	}

	Service *Game::getService(std::string name)
	{
		for (auto &i : this->Services) {
			if (i->getName() == name)
				return i;
		}

		return nullptr;
	}

	void Game::Run()
	{
		while (!shouldClose && !_game_shouldClose)
			runServices();

		for (auto &i : this->Services)
			i->Release();

		global_log.Info("Game Quit");
	}

	void Game::Destroy()
	{
		shouldClose = true;
	}

	void Game::Abort()
	{
		global_log.Fatal("Game Aborted");
		std::abort();
	}

	bool _game_shouldClose;

}
