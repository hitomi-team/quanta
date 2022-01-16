#ifndef LEVEL0_INPUT_SERVICE_H
#define LEVEL0_INPUT_SERVICE_H

#include "level0/game/game_service.h"

class InputService : public GameService {
public:
	UTIL_DEFINE_SERVICE_HASH(InputService);

	InputService();

	void Update();

#ifndef NDEBUG
private:
	void ConsoleThread();
	std::queue< std::string > m_consoleLines;
	std::mutex m_consoleLock;
#endif
};

#endif
