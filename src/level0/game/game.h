#ifndef LEVEL0_GAME_GAME_H
#define LEVEL0_GAME_GAME_H

#include "game_service.h"

#define GAME_TITLE_NAME "Quanta Engine"

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

	inline GameService *GetService(uint64_t hash) { return m_services[hash].get(); }

	template< typename T >
	inline T *GetService() { return dynamic_cast< T * >(m_services[T::Hash].get()); }

protected:
	std::unordered_map< uint64_t, std::unique_ptr< GameService > > m_services;
	bool m_shouldClose = false, m_running = false;

};

extern Game *g_Game;

#endif
