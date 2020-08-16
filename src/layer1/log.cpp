#include "pch/pch.h"

#include "log.h"

static const char *Prefixes[] = {
	"debug: ",
	"error: ",
	"info : ",
	"warn : "
};

Log::Log(const std::string &filename)
{
	Load(filename);
}

void Log::Load(const std::string &filename)
{
	fstream = std::ofstream(filename);
}

void Log::Print(LogLevel level, const std::string &msg)
{
	std::unique_lock< std::mutex > lock(this->mtx);

	buffer.push_back(std::string(Prefixes[static_cast<int>(level)]) + msg + '\n');

	if (fstream.is_open()) {
		fstream   << Prefixes[static_cast<int>(level)] << msg << std::endl;
	} else {
		std::cout << Prefixes[static_cast<int>(level)] << msg << std::endl;
	}
}

Log global_log;
