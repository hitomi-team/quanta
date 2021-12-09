#include "level0/pch.h"
#include "level0/log.h"

#include "level0/game/game.h"
#include <physfs.h>

#include "service.h"

FilesystemService::FilesystemService(const char *argv0) : GameService("FilesystemService")
{
	if (PHYSFS_init(argv0) == 0)
		g_Game->Abort("Cannot init FilesystemService!");

	PHYSFS_permitSymbolicLinks(0);

	if (PHYSFS_mount("data.zip", nullptr, 1) == 0) {
		PHYSFS_deinit();
		g_Game->Abort("FilesystemService: No data pack found!");
	}
}

FilesystemService::~FilesystemService()
{
	PHYSFS_deinit();
}
