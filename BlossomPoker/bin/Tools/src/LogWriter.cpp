#include "../inc/LogWriter.h"

LogWriter::LogWriter()
{
}


LogWriter::~LogWriter()
{
	for (auto& Entry : Entries)
		Entry->File.close();
}

void LogWriter::NewDir()
{
	LastSavedDateTime = GetCurrentDateTime();
	CurrentDir = "TestLogs\\" + LastSavedDateTime;
	mkdir(CurrentDir.c_str());
}

void LogWriter::NewFile(LogType _Type, std::string _Name)
{
	std::shared_ptr<Entry> NewEntry = std::make_shared<Entry>(_Type, _Name);
	NewEntry->File.open(CurrentDir + "\\" + LastSavedDateTime + " - " + NewEntry->Name);

	if (NewEntry->File.fail())
	{
		_mkdir("TestLogs\\");
		NewDir();

		NewEntry->File.open(CurrentDir + "\\" + LastSavedDateTime + " - " + NewEntry->Name);
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

void LogWriter::GenerateGNUFile()
{
	NewFile(LogType::NONE, "GNU Commands");

	WriteAt(Entries.size() - 1, "set title \"Performance\" font \", 20\"\n");
	WriteAt(Entries.size() - 1, "set xlabel \"Generation\"\n");
	WriteAt(Entries.size() - 1, "set ylabel \"Fitness\"\n");
	WriteAt(Entries.size() - 1, "plot 'D:\\Git Repositories\\BlossomPoker\\BlossomPoker\\TestLogs\\" + LastSavedDateTime + "\\" + LastSavedDateTime + " - GenBestFitness.txt' with linespoints title 'Best Fitness', 'D:\\Git Repositories\\BlossomPoker\\BlossomPoker\\TestLogs\\" + LastSavedDateTime + "\\" + LastSavedDateTime + " - GenerationPerformance.txt' with linespoints title 'Average Fitness' , 'D:\\Git Repositories\\BlossomPoker\\BlossomPoker\\TestLogs\\" + LastSavedDateTime + "\\" + LastSavedDateTime + " - GenWorstFitness.txt' with linespoints title 'Worst Fitness'\n\n");

	WriteAt(Entries.size() - 1, "set title \"Mutation vs Variance\" font \",20\"\n");
	WriteAt(Entries.size() - 1, "set xlabel \"Generation\"\n");
	WriteAt(Entries.size() - 1, "set ylabel \"Rate\"\n");
	WriteAt(Entries.size() - 1, "plot 'D:\\Git Repositories\\BlossomPoker\\BlossomPoker\\TestLogs\\" + LastSavedDateTime + "\\" + LastSavedDateTime + " - PopulationVariance.txt' with linespoints title 'Population Variance', 'D:\\Git Repositories\\BlossomPoker\\BlossomPoker\\TestLogs\\" + LastSavedDateTime + "\\" + LastSavedDateTime + " - MutationRate.txt' with linespoints title 'Mutation Rate'\n\n");

	WriteAt(Entries.size() - 1, "set title \"Hall of Fame\" font \",20\"\n");
	WriteAt(Entries.size() - 1, "set xlabel \"Index (P.)\"\n");
	WriteAt(Entries.size() - 1, "set ylabel \"Fitness\"\n");
	WriteAt(Entries.size() - 1, "plot 'D:\\Git Repositories\\BlossomPoker\\BlossomPoker\\TestLogs\\" + LastSavedDateTime + "\\" + LastSavedDateTime + " - HallOfFame.txt' with points title 'Player'\n\n");

	WriteAt(Entries.size() - 1, "set title \"Elitism vs Top 3 in HoF\" font \",20\"\n");
	WriteAt(Entries.size() - 1, "set xlabel \"Generation\"\n");
	WriteAt(Entries.size() - 1, "set ylabel \"Fitness\"\n");
	WriteAt(Entries.size() - 1, "plot 'D:\\Git Repositories\\BlossomPoker\\BlossomPoker\\TestLogs\\" + LastSavedDateTime + "\\" + LastSavedDateTime + " - GenEliteAverFitness.txt' with linespoints title 'Elite Average Fitness', 'D:\\Git Repositories\\BlossomPoker\\BlossomPoker\\TestLogs\\" + LastSavedDateTime + "\\" + LastSavedDateTime + " - GenHoFTop3AverFitness.txt' with linespoints title 'Top 3 Average Fitness'\n\n");

	CloseAt(Entries.size() - 1);
	Entries.pop_back();
}

void LogWriter::CloseAt(unsigned int _Index)
{
	Entries[_Index]->File.close();
}

void LogWriter::Clear()
{
	Entries.clear();
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