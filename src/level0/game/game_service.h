#ifndef LEVEL0_GAME_SERVICE_H
#define LEVEL0_GAME_SERVICE_H

// This is intended for services!
#define UTIL_DEFINE_SERVICE_HASH(x) \
static constexpr uint64_t Hash = UtilStringHash(#x)

class GameService {
public:
	const char *name;

	GameService() = delete;
	GameService(const char *_name);
	virtual ~GameService() = default;

	virtual void Update() = 0;

};

#endif
