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
	this->fstream = std::ofstream(filename);
}

void Log::Print(LogLevel level, const std::string &msg)
{
	std::unique_lock< std::mutex > lock(this->mtx);

	std::stringstream stream(msg);
	std::string line;

	// the string that will be written to stream
	std::string temp;

	const char *prefix = Prefixes[static_cast< int >(level)];

	while (std::getline(stream, line, '\n')) {
		temp.clear();
		temp.append(prefix);
		temp.append(line);
		temp.append(1, '\n');

		if (level == LogLevel::Error)
			std::cerr << temp << std::flush;
		else
			std::cout << temp << std::flush;

		if (fstream.is_open())
			this->fstream << temp << std::flush;

		// TODO: limit the amount of lines
		this->buffer.push_back(temp);
	}
}

Log global_log;
