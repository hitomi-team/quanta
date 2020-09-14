#ifndef LOG_H
#define LOG_H

enum class LogLevel : int {
	Debug = 0,
	Error = 1,
	Info =  2,
	Warn =  3
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

protected:
	std::mutex mtx;
	std::vector<std::string> buffer;
	std::ofstream fstream;
	void Print(LogLevel level, const std::string &msg);
};

extern Log global_log;

#endif
