#include "level0/pch.h"
#include "level0/log.h"

#include "level0/game/game.h"
#include "level0/cvarcmd/service.h"
#include "level0/filesystem/service.h"
#include "level0/render/service.h"
#include "level0/input/service.h"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	if (Sys_Init() != 0) {
		g_Log.Error("Cannot Initialize System");
		return 1;
	}

	try {
		Game game;
		g_Game = &game;

		game.RegisterService(std::make_unique< CVarCmdService >());
		game.RegisterService(std::make_unique< FilesystemService >(argv[0]));
		game.RegisterService(std::make_unique< RenderService >());
		game.RegisterService(std::make_unique< InputService >());
		game.Run();
	} catch (const std::exception &e) {
		g_Log.Error(FMT_COMPILE("Game Exception Caught: {}"), e.what());
		Sys_Quit();
		return 63;
	}

	Sys_Quit();

	return 0;
}
