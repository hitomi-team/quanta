#ifndef LEVEL0_GAME_GAME_H
#define LEVEL0_GAME_GAME_H

#include "game_service.h"

class Game {
public:
	Game();
	~Game();

	// move the service into the game's possession
	void RegisterService(std::unique_ptr< GameService > &&service);
	void InitServices();
	void UpdateServices();

	void Run();

	void RequestClose();

	// Emergency Abort
	void Abort(std::string cause);

	GameService *GetService(const char *name);

protected:
	std::vector< std::unique_ptr< GameService > > m_services;
	bool m_shouldClose = false, m_running = false;

};

extern Game *g_Game;

#endif