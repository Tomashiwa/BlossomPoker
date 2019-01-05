#include "LogWriter.h"

LogWriter::LogWriter()
{
}


LogWriter::~LogWriter()
{
	for (auto& Entry : Entries)
		Entry->File.close();
}

void LogWriter::NewFile(LogType _Type, std::string _Name)
{
	std::shared_ptr<Entry> NewEntry = std::make_shared<Entry>(_Type, _Name);
	NewEntry->File.open(NewEntry->Dir);

	if (NewEntry->File.fail())
	{
		_mkdir("TestLogs\\");
		NewEntry->File.open(NewEntry->Dir);
	}

	Entries.push_back(NewEntry);

	WriteAt(Entries.size() - 1, "# " + _Name + "\n");

	switch (_Type)
	{
		case LogType::Graph_Line:
			WriteAt(Entries.size() - 1, "# Type: Line Graph\n");
			break;
		case LogType::Graph_Bar:
			WriteAt(Entries.size() - 1, "# Type: Bar Graph\n");
			break;
		default:
			WriteAt(Entries.size() - 1, "# Type: NOT FOUND\n");
			break;
	}

	WriteAt(Entries.size() - 1, "# \n");
}

void LogWriter::WriteAt(unsigned int _FileIndex, std::string _Line)
{
	Entries[_FileIndex]->File << _Line << std::flush;
}

void LogWriter::WriteAt(unsigned int _FileIndex, float _Axis1, float _Axis2)
{
	Entries[_FileIndex]->File << _Axis1 << " " << _Axis2 << "\n" << std::flush;
}

void LogWriter::WriteAt(unsigned int _FileIndex, unsigned int _Index, std::string _Label, float _Value)
{
	Entries[_FileIndex]->File << _Index << " " << _Label << " " << _Value << "\n" << std::flush;
}

void LogWriter::CloseAt(unsigned int _Index)
{
	Entries[_Index]->File.close();
}

void LogWriter::Clear()
{
	Entries.clear();
}
