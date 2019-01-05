#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string>
#include <direct.h>	
#include <vector>
#include <memory>

#include "LogType.h"

class LogWriter
{
public:
	LogWriter();
	~LogWriter();

	void NewFile(LogType _Type, std::string _Name);

	void WriteAt(unsigned int _FileIndex, std::string _Line);
	void WriteAt(unsigned int _FileIndex, float _Axis1, float _Axis2);
	void WriteAt(unsigned int _FileIndex, unsigned int _Index, std::string _Label, float _Value);

	void CloseAt(unsigned int _Index);
	void Clear();

private:
	struct Entry
	{
		std::ofstream File;
		std::string Dir;
		LogType Type;

		Entry(LogType _Type, std::string _Name) : Type(_Type), Dir("TestLogs\\" + GetCurrentDateTime() + " - " + _Name + ".txt") {};

		std::string GetCurrentDateTime() const
		{
			auto t = std::time(nullptr);
			auto tm = *std::localtime(&t);

			std::ostringstream oss;
			oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
			std::string str = oss.str();

			return str;
		}
	};

	std::vector<std::shared_ptr<Entry>> Entries;
};

