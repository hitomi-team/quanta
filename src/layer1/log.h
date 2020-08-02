#ifndef LOG_H
#define LOG_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

enum class LogLevel : int {
	Debug = 0,
	Error = 1,
	Info =  2,
	Warn =  3
};

class Log {
public:
	Log() {  }
	Log(std::string filename);
	void Load(std::string filename);

	void Debug(std::string msg) { Print(LogLevel::Debug, msg); }
	void Error(std::string msg) { Print(LogLevel::Error, msg); }
	void Info(std::string msg)  { Print(LogLevel::Info,  msg); }
	void Warn(std::string msg)  { Print(LogLevel::Warn,  msg); }

	inline std::vector<std::string> getBuffer() { return buffer; }

protected:
	std::vector<std::string> buffer;
	std::ofstream fstream;
	void Print(LogLevel level, std::string msg);
};

extern Log global_log;


#endif
