#ifndef LEVEL0_FILESYSTEM_SERVICE_H
#define LEVEL0_FILESYSTEM_SERVICE_H

#include "level0/game/game_service.h"

class FilesystemService : public GameService {
public:
	UTIL_DEFINE_SERVICE_HASH(FilesystemService);

	FilesystemService() = delete;
	FilesystemService(const char *argv0);
	~FilesystemService();

	inline void Update() {}

private:

};

#endif
