#include "LogWriter.h"

LogWriter::LogWriter()
{
}


LogWriter::~LogWriter()
{
	for (auto& Entry : Entries)
		Entry->File.close();
}

void LogWriter::New(std::string _Name)
{
	if(_Name == "")
	{
		std::shared_ptr<Entry> NewEntry = std::make_shared<Entry>();

		NewEntry->File.open(NewEntry->Dir);

		if (NewEntry->File.fail())
		{
			_mkdir("TestLogs\\");
			NewEntry->File.open(NewEntry->Dir);
		}

		Entries.push_back(NewEntry);
	}
	else
	{
		std::shared_ptr<Entry> NewEntry = std::make_shared<Entry>(_Name);
		NewEntry->File.open(NewEntry->Dir);

		if (NewEntry->File.fail())
		{
			_mkdir("TestLogs\\");
			NewEntry->File.open(NewEntry->Dir);
		}

		Entries.push_back(NewEntry);
	}
}

void LogWriter::Write(unsigned int _Index, std::string _Line)
{
	Entries[_Index]->File << _Line << std::flush;
}

void LogWriter::Close(unsigned int _Index)
{
	Entries[_Index]->File.close();
}

void LogWriter::Clear()
{
	Entries.clear();
}
