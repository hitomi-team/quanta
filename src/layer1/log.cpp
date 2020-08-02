#include "log.h"

static const char *Prefixes[] = {
	"debug: ",
	"error: ",
	"info : ",
	"warn : "
};

Log::Log(std::string filename)
{
	Load(filename);
}

void Log::Load(std::string filename)
{
	fstream = std::ofstream(filename);
}

void Log::Print(LogLevel level, std::string msg)
{
	buffer.push_back(std::string(Prefixes[static_cast<int>(level)]) + msg + '\n');

	if (fstream.is_open()) {
		fstream   << Prefixes[static_cast<int>(level)] << msg << std::endl;
	} else {
		std::cout << Prefixes[static_cast<int>(level)] << msg << std::endl;
	}
}

Log global_log;
