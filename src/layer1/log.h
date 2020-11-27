#ifndef LOG_H
#define LOG_H

enum class LogLevel : int {
	Debug = 0,
	Error = 1,
	Info =  2,
	Warn =  3,
	Fatal = 4,
};

class Log {
public:
	Log() {  }
	Log(const std::string &filename);
	void Load(const std::string &filename);

	inline void Debug(const std::string &msg) { Print(LogLevel::Debug, msg); }
	inline void Error(const std::string &msg) { Print(LogLevel::Error, msg); }
	inline void Info(const std::string &msg)  { Print(LogLevel::Info,  msg); }
	inline void Warn(const std::string &msg)  { Print(LogLevel::Warn,  msg); }

	inline std::vector<std::string> &getBuffer() { return buffer; }

	template < typename _StringType, typename ... Args >
	inline void Debug(const _StringType &fmt, Args &&...args) { Print(LogLevel::Debug, fmt::format(fmt, std::forward< Args >(args)...)); }

	template < typename _StringType, typename ... Args >
	inline void Error(const _StringType &fmt, Args &&...args) { Print(LogLevel::Error, fmt::format(fmt, std::forward< Args >(args)...)); }

	template < typename _StringType, typename ... Args >
	inline void Info(const _StringType &fmt, Args &&...args) { Print(LogLevel::Info, fmt::format(fmt, std::forward< Args >(args)...)); }

	template < typename _StringType, typename ... Args >
	inline void Warn(const _StringType &fmt, Args &&...args) { Print(LogLevel::Warn, fmt::format(fmt, std::forward< Args >(args)...)); }

protected:
	std::mutex mtx;
	std::vector<std::string> buffer;
	std::ofstream fstream;
	void Print(LogLevel level, const std::string &msg);
};

extern Log global_log;

#endif
