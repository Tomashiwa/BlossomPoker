#include "LogWriter.h"

LogWriter::LogWriter()
{
}


LogWriter::~LogWriter()
{
	File.close();
}

void LogWriter::OpenNew()
{
	FileName = "TestLogs\\" + GetCurrentDateTime() + ".txt";
	File.open(FileName);

	if (File.fail())
	{
		_mkdir("TestLogs\\");
		File.open(FileName);
	}
}

void LogWriter::Write(std::string _Line)
{
	File << _Line << std::flush;
}

void LogWriter::Close()
{
	File.close();
}

std::string LogWriter::GetCurrentDateTime() const
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::ostringstream oss; 
	oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
	std::string str = oss.str();

	return str;
}

