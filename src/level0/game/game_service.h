#ifndef LEVEL0_GAME_SERVICE_H
#define LEVEL0_GAME_SERVICE_H

class GameService {
public:
	const char *name;

	GameService() = delete;
	GameService(const char *_name);
	inline virtual ~GameService() {};

	virtual void Update() = 0;

};

#endif
