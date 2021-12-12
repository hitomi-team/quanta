#include "level0/pch.h"
#include "level0/log.h"

#ifndef _WIN32
// This is needed, on my system the program will not respond to ^C/SIGTERM requests.
static void SignalHandler(int s)
{
	(void)s;
	g_Log.Warn("SIGINT/SIGTERM received! Quitting early...");
	std::exit(64);
}
#endif

int Sys_Init()
{
#ifndef _WIN32
	struct sigaction handler = {};
	handler.sa_handler = SignalHandler;
	sigaction(SIGINT, &handler, nullptr);
	sigaction(SIGTERM, &handler, nullptr);
#endif

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		g_Log.Error("Failed to initialize SDL2");
		return 1;
	}

	return 0;
}

void Sys_Quit()
{
	SDL_Quit();
}

int Sys_InitConsole()
{
#ifdef _WIN32
	HANDLE houtput = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE herror = GetStdHandle(STD_ERROR_HANDLE);

	bool houtput_pipe = GetFileType(houtput) == FILE_TYPE_DISK || GetFileType(houtput) == FILE_TYPE_PIPE;
	bool herror_pipe = GetFileType(herror) == FILE_TYPE_DISK || GetFileType(herror) == FILE_TYPE_PIPE;
	FILE *fp;

	if (!Sys_IsConsoleActive()) {
		HANDLE hinput;

		if (AllocConsole() == FALSE)
			return -1;

		hinput = CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		houtput = CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (!houtput_pipe) {
			_wfreopen_s(&fp, L"CONOUT$", L"w", stdout);
			SetStdHandle(STD_OUTPUT_HANDLE, houtput);
		}

		if (!herror_pipe) {
			_wfreopen_s(&fp, L"CONOUT$", L"w", stderr);
			SetStdHandle(STD_ERROR_HANDLE, houtput);
		}

		_wfreopen_s(&fp, L"CONIN$", L"r", stdin);		
		SetStdHandle(STD_INPUT_HANDLE, hinput);
	}

	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

#endif
	return 0;
}

bool Sys_IsConsoleActive()
{
#ifdef _WIN32
	DWORD dummy;
	return GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &dummy);
#else
	return isatty(fileno(stdin));
#endif
}

char *Sys_GetConsoleInput(char *buf, size_t bufsize)
{
#ifdef _WIN32
	DWORD dummy;
	std::vector< WCHAR > rc(bufsize);

	if (!ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE), rc.data(), (DWORD)bufsize, &dummy, nullptr))
		return nullptr;

	WideCharToMultiByte(CP_UTF8, 0, rc.data(), -1, buf, (int)bufsize, nullptr, nullptr);

	return buf;
#else
	return fgets(buf, bufsize, stdin);
#endif
}

char *Sys_DuplicateString(const char *buf)
{
	size_t n = strlen(buf);
	char *res = new char[n + 1]();
	SDL_strlcpy(res, buf, n + 1);
	return res;
}

void _Sys_Assert(const char *msg)
{
	fwrite(msg, 1, strlen(msg), stderr);
	debug_break();
}
