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

	void NewDir();
	void NewFile(LogType _Type, std::string _Name);
	void GenerateGNUFiles();

	void WriteAt(unsigned int _FileIndex, std::string _Line);
	void WriteAt(unsigned int _FileIndex, float _Axis1, float _Axis2);
	void WriteAt(unsigned int _FileIndex, unsigned int _Index, std::string _Label, float _Value);

	void CloseAt(unsigned int _Index);
	void Clear();

	std::string GetCurrentDateTime() const;

private:
	struct Entry
	{
		std::ofstream File;
		std::string Name;
		LogType Type;

		Entry(LogType _Type, std::string _Name) : Type(_Type), Name( _Name + ".txt") {};
	};

	std::string CurrentDir;
	std::string LastSavedDateTime;
	std::vector<std::shared_ptr<Entry>> Entries;
};

