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

class LogWriter
{
public:
	LogWriter();
	~LogWriter();

	void New(std::string _Name);
	void Write(unsigned int _Index, std::string _Line);
	void Close(unsigned int _Index);
	void Clear();

private:
	struct Entry
	{
		std::ofstream File;
		std::string Dir;

		Entry() 
		{
			Dir = "TestLogs\\" + GetCurrentDateTime() + ".txt";
		}

		Entry(std::string _Name) : Dir("TestLogs\\" + GetCurrentDateTime() + " - " + _Name +".txt"){}

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

