#ifndef LEVEL0_LOG_H
#define LEVEL0_LOG_H

// This module does not need 4-byte aligned buffers

enum class LogLevel : int {
	Debug = 0,
	Error = 1,
	Info =  2,
	Warn =  3,
	Fatal = 4,
};

class Log {
public:
	std::deque< std::string > buffer;

	inline Log() { this->file = nullptr; }
	Log(const std::string &filename);
	~Log();

	void Load(const std::string &filename);

	inline void Debug(const char *msg) { Print(LogLevel::Debug, msg); }
	inline void Error(const char *msg) { Print(LogLevel::Error, msg); }
	inline void Info(const char *msg)  { Print(LogLevel::Info,  msg); }
	inline void Warn(const char *msg)  { Print(LogLevel::Warn,  msg); }

	inline void Debug(const std::string &msg) { Print(LogLevel::Debug, msg.c_str()); }
	inline void Error(const std::string &msg) { Print(LogLevel::Error, msg.c_str()); }
	inline void Info(const std::string &msg)  { Print(LogLevel::Info,  msg.c_str()); }
	inline void Warn(const std::string &msg)  { Print(LogLevel::Warn,  msg.c_str()); }

	template < typename _StringType, typename ... Args >
	inline void Debug(const _StringType &fmt, Args &&...args) { Print(LogLevel::Debug, fmt::format(fmt, std::forward< Args >(args)...).c_str()); }

	template < typename _StringType, typename ... Args >
	inline void Error(const _StringType &fmt, Args &&...args) { Print(LogLevel::Error, fmt::format(fmt, std::forward< Args >(args)...).c_str()); }

	template < typename _StringType, typename ... Args >
	inline void Info(const _StringType &fmt, Args &&...args) { Print(LogLevel::Info, fmt::format(fmt, std::forward< Args >(args)...).c_str()); }

	template < typename _StringType, typename ... Args >
	inline void Warn(const _StringType &fmt, Args &&...args) { Print(LogLevel::Warn, fmt::format(fmt, std::forward< Args >(args)...).c_str()); }

	template < typename _StringType, typename ... Args >
	inline void Fatal(const _StringType &fmt, Args &&...args) { Print(LogLevel::Fatal, fmt::format(fmt, std::forward< Args >(args)...).c_str()); }

	inline void ClearBuffer() { this->buffer.clear(); }

protected:
	std::mutex mtx;
	FILE *file;

	void Print(LogLevel level, const char *msg);

};

extern Log g_Log;

#endif
