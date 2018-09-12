#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string>
#include <direct.h>	

class LogWriter
{
public:
	LogWriter();
	~LogWriter();

	void OpenNew();
	void Write(std::string _Line);
	void Close();

private:
	std::string GetCurrentDateTime() const;

	std::ofstream File;
	std::string FileName;
};

