#include "pch/pch.h"

#include "log.h"

static const char *Prefixes[] = {
	"[DEBUG] ",
	"[ERROR] ",
	"[INFO] ",
	"[WARN] "
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
		temp = fmt::format("{}{}\n", prefix, line);

		if (level == LogLevel::Error)
			std::cerr << temp << std::flush;
		else
			std::cout << temp << std::flush;

		if (fstream.is_open())
			this->fstream << temp << std::flush;

		// if some weird stuff happens
		while (this->buffer.size() >= 1024)
			this->buffer.erase(this->buffer.begin());

		this->buffer.push_back(temp);
	}
}

Log global_log;
