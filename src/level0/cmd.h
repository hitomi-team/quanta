#ifndef __cmd_hxx___
#define __cmd_hxx___

#include "cvar.h"

enum ECmdSource {
	SOURCE_BUFFER = 0,
	SOURCE_CLIENT
};

extern ECmdSource cmd_source;

void Cmd_Init();
void Cmd_Cleanup();
void Cmd_CleanupArgv();

int Cmd_Argc();
std::string Cmd_Argv(int i);
std::string Cmd_Args();

bool Cmd_ParseStr(const char *str);
void Cmd_Exec(const char *str);

void Cmd_Add(const char *name, std::function< int() > func);

#endif
