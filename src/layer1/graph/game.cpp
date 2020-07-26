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
					// Handle error!
					return;
				}
			}
		}		
	}

	void Game::runServices()
	{
		for (auto &service : this->Services) {
			if (!service->isInitialized()) { // This is really unsafe! Issue severe warning!
				service->Setup();
			}
			
			if (!service->Update()) {
				// Handle error! Maybe a popup window or something?
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
		setupServices();

		while (!shouldClose)
			runServices();
		
		for (auto &i : this->Services)
			i->Release();
	}

	void Game::Destroy()
	{
		shouldClose = true;
	}

}
