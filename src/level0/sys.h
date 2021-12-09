#ifndef LEVEL0_SYS_H
#define LEVEL0_SYS_H

int Sys_Init();
void Sys_Quit();

int Sys_InitConsole();
bool Sys_IsConsoleActive();

// ALIGN UR BUFFER TO BE ON A 4 BYTE BOUNDARY
char *Sys_GetConsoleInput(char *buf, size_t bufsize);

char *Sys_DuplicateString(const char *buf);

#define Sys_Assert(condition, msg) \
do { \
	if (!(condition)) \
		_Sys_Assert(msg); \
while (0)

void _Sys_Assert(const char *msg);

#endif
