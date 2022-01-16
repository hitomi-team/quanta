#include "pch.h"
#include "log.h"

static const char *Prefixes[] = {
	"[DEBUG] ",
	"[ERROR] ",
	"[INFO] ",
	"[WARN] ",
	"[FATAL] ",
};

Log::Log(const std::string &filename)
{
	Load(filename);
}

Log::~Log()
{
	if (this->file != nullptr) {
		fclose(this->file);
		this->file = nullptr;
	}
}

void Log::Load(const std::string &filename)
{
	this->file = fopen(filename.c_str(), "wb");
}

void Log::Print(LogLevel level, const char *msg)
{
	std::lock_guard< std::mutex > lock(this->mtx);

	const char *prefix = Prefixes[static_cast< int >(level)];

	std::stringstream stream(msg);
	std::string line;

	while (std::getline(stream, line, '\n')) {
		// the string that will be written to stream
		std::string temp = fmt::format(FMT_COMPILE("{}{}\n"), prefix, line);
#ifdef _WIN32
		std::vector< WCHAR > utf16str;
		int len = MultiByteToWideChar(CP_UTF8, 0, temp.c_str(), -1, nullptr, 0);
		utf16str.resize(len + 1);
		MultiByteToWideChar(CP_UTF8, 0, temp.c_str(), -1, utf16str.data(), len);
#endif

		if (level == LogLevel::Error || level == LogLevel::Fatal) {
#ifdef _WIN32
			BOOL res = WriteConsoleW(GetStdHandle(STD_ERROR_HANDLE), utf16str.data(), static_cast< DWORD >(temp.size()), nullptr, nullptr);
			if (res == FALSE) {
				WriteFile(GetStdHandle(STD_ERROR_HANDLE), temp.c_str(), static_cast< DWORD >(temp.size()), nullptr, nullptr);
				FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
			}
#else
			fwrite(temp.c_str(), 1, temp.size(), stderr);
			fflush(stderr);
#endif
		} else {
#ifdef _WIN32
			BOOL res = WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), utf16str.data(), static_cast< DWORD >(temp.size()), nullptr, nullptr);
			if (res == FALSE) {
				WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), temp.c_str(), static_cast< DWORD >(temp.size()), nullptr, nullptr);
				FlushFileBuffers(GetStdHandle(STD_OUTPUT_HANDLE));
			}
#else
			fwrite(temp.c_str(), 1, temp.size(), stdout);
			fflush(stdout);
#endif
		}

		if (this->file != nullptr) {
			fwrite(temp.c_str(), 1, temp.size(), this->file);
			fflush(this->file);
		}

		if (this->buffer.size() > 4096)
			this->buffer.pop_front();

		this->buffer.push_back(std::move(temp));
	}
}

Log g_Log;
