#include "level0/pch.h"
#include "cmd.h"
#include "log.h"

#define MAX_CMD_ARGV 128

struct CmdCommand {
	const char *name;
	std::function< int() > func;

	CmdCommand() = delete;

	inline CmdCommand(const char *name, std::function< int() > func)
	{
		this->name = name;
		this->func = func;
	}
};

static int cmd_argc;
static std::string cmd_args;
static std::array< std::string, MAX_CMD_ARGV > cmd_argv {};
static std::array< std::string, MAX_CMD_ARGV > empty_cmd_argv {};

static std::vector< CmdCommand > CmdList;

ECmdSource cmd_source = ECmdSource::SOURCE_BUFFER;

void Cmd_Init()
{
	cmd_argc = 0;
	cmd_args = nullptr;
}

void Cmd_Cleanup()
{
	Cmd_CleanupArgv();
}

void Cmd_CleanupArgv()
{
	IO_EraseCPPVector(cmd_args);

	std::copy(empty_cmd_argv.begin(), empty_cmd_argv.end(), cmd_argv.begin());

	cmd_argc = 0;
}

int Cmd_Argc()
{
	return cmd_argc;
}

std::string Cmd_Argv(int i)
{
	if (i < 0 || i > cmd_argc)
		return "";
	return cmd_argv[i];
}

std::string Cmd_Args()
{
	return cmd_args;
}

bool Cmd_ParseStr(const char *str)
{
	std::array< char, 1024 > buf {};
	int e, c_len;
	uint32_t c;
	const char *str_end = str + strlen(str) + 1;

	if (str == nullptr)
		return false;

	Cmd_CleanupArgv();

	cmd_args = str;

	for (;;) {
		for (;;) {
			c_len = utf8_decode_v2(str, str_end, &c, &e);
			if (e != 0)
				return false;
			if (c == '\0' || c > ' ' || c == '\n')
				break;
			str += c_len;
		}

		if (c == '\n') {
			str += c_len;
			break;
		}

		if (c == '\0')
			return true;

		str = Com_ParseStr(buf.data(), buf.size(), str);
		if (str == nullptr)
			return true;

		if (cmd_argc < MAX_CMD_ARGV)
			cmd_argv[cmd_argc++] = buf.data();
	}

	return true;
}

void Cmd_Exec(const char *str)
{
	if (!Cmd_ParseStr(str))
		return;

	for (auto &cmd : CmdList) {
		if (SDL_strcmp(cmd_argv[0].c_str(), cmd.name) == 0) {
			if (cmd.func() != 0)
				g_Log.Error(FMT_COMPILE("failed to execute command \"{}\""), cmd_argv[0]);
			return;
		}
	}

	if (!CVar_Cmd())
		g_Log.Error(FMT_COMPILE("unknown command: \"{}\""), cmd_argv[0]);
}

void Cmd_Add(const char *name, std::function< int() > func)
{
	CmdList.emplace_back(name, func);
}
