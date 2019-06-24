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
	NewEntry->File.open(CurrentDir + "\\" + NewEntry->Name);

	if (NewEntry->File.fail())
	{
		_mkdir("TestLogs\\");
		NewDir();

		NewEntry->File.open(CurrentDir + "\\" + NewEntry->Name);
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

bool LogWriter::Overwrite(unsigned int _FileIndex, std::string _ToReplace, std::string _ToReplaceWith)
{
	std::ifstream Input(CurrentDir + "\\" + Entries[_FileIndex]->Name);
	std::ofstream Output(CurrentDir + "\\Overwriting - " + Entries[_FileIndex]->Name);

	if (!Input)
	{
		std::cout << "Cannot open " << Entries[_FileIndex]->Name << " from " << CurrentDir << "\n";
		return false;
	}

	if (!Output)
	{
		std::cout << "Cannot open/create" << CurrentDir + "\\Overwriting - " + Entries[_FileIndex]->Name << "\n";
		return false;
	}

	std::string CurrentLine;
	size_t ReplaceLen = _ToReplace.length();

	while (std::getline(Input, CurrentLine))
	{
		if (CurrentLine == _ToReplace)
			CurrentLine = _ToReplaceWith;

		Output << CurrentLine << "\n";
	}

	Input.close();
	Output.close();

	std::ifstream FileToOverwriteWith(CurrentDir + "\\Overwriting - " + Entries[_FileIndex]->Name);
	
	std::ofstream FileToOverwrite;
	FileToOverwrite.open(CurrentDir + "\\" + Entries[_FileIndex]->Name, std::ofstream::out | std::ofstream::trunc);
	
	std::string Content = "";

	while (FileToOverwriteWith.eof() != true)
		Content += FileToOverwriteWith.get();

	Content.erase(Content.end() - 1);

	FileToOverwriteWith.close();

	std::string Dir = CurrentDir + "\\Overwriting - " + Entries[_FileIndex]->Name;
	std::remove(Dir.c_str());

	FileToOverwrite << Content;
	FileToOverwrite.close();

	return true;
}

void LogWriter::GenerateGNUFiles(TrainingModel _Model)
{
	NewFile(LogType::NONE, "RunAll");

	WriteAt(Entries.size() - 1, "ind_start = strstrt(ARG0, \"RunAll.txt\") - 1\n");
	WriteAt(Entries.size() - 1, "dir_ori = ARG0[0:ind_start]\n\n");
	WriteAt(Entries.size() - 1, "print dir_ori\n\n");

	WriteAt(Entries.size() - 1, "dat_bestsofar = 'BestSoFar.txt'\n");
	WriteAt(Entries.size() - 1, "dat_aver = 'Average.txt'\n");
	WriteAt(Entries.size() - 1, "dat_worstsofar = 'WorstSoFar.txt'\n\n");

	WriteAt(Entries.size() - 1, "dat_variance = 'Variance.txt'\n");
	WriteAt(Entries.size() - 1, "dat_mutrate = 'MutationRate.txt'\n\n");

	WriteAt(Entries.size() - 1, "dat_hof = 'HallOfFame.txt'\n");
	WriteAt(Entries.size() - 1, "dat_elitaver = 'EliteAver.txt'\n");
	WriteAt(Entries.size() - 1, "dat_top3aver = 'Top3Aver.txt'\n\n");

	if (_Model.HasHoF)
	{
		WriteAt(Entries.size() - 1, "#Graph - HOF\n");
		WriteAt(Entries.size() - 1, "set terminal wxt 2\n");
		WriteAt(Entries.size() - 1, "set grid\n\n");
		WriteAt(Entries.size() - 1, "set title \"Hall of Fame\" font \", 20\"\n");
		WriteAt(Entries.size() - 1, "set xlabel \"Index (P.)\"\n");
		WriteAt(Entries.size() - 1, "set ylabel \"Fitness\"\n");
		WriteAt(Entries.size() - 1, "plot dir_ori.dat_hof with points title 'Player'\n\n");
	}

	if (_Model.HasElite && _Model.HasHoF)
	{
		WriteAt(Entries.size() - 1, "#Graph - Elitism vs HoF\n");
		WriteAt(Entries.size() - 1, "set terminal wxt 3\n");
		WriteAt(Entries.size() - 1, "set grid\n\n");
		WriteAt(Entries.size() - 1, "set title \"Elitism vs Top 3 HoF Players\" font \", 20\"\n");
		WriteAt(Entries.size() - 1, "set xlabel \"Generation\"\n");
		WriteAt(Entries.size() - 1, "set ylabel \"Fitness\"\n");
		WriteAt(Entries.size() - 1, "plot dir_ori.dat_elitaver with linespoints title 'Elite Average Fitness', dir_ori.dat_top3aver with linespoints title 'Top 3 Average Fitness'\n\n");
	}

	WriteAt(Entries.size() - 1, "#Graph - Mutation\n");
	WriteAt(Entries.size() - 1, "set terminal wxt 1\n");
	WriteAt(Entries.size() - 1, "set grid\n\n");
	WriteAt(Entries.size() - 1, "set title \"Mutation vs Variance\" font \", 20\"\n");
	WriteAt(Entries.size() - 1, "set xlabel \"Generation\"\n");
	WriteAt(Entries.size() - 1, "set ylabel \"Rate\"\n");
	WriteAt(Entries.size() - 1, "plot dir_ori.dat_variance with linespoints title 'Population Variance', dir_ori.dat_mutrate with linespoints title 'Mutation Rate'\n\n");

	WriteAt(Entries.size() - 1, "#Graph - Performance\n");
	WriteAt(Entries.size() - 1, "set terminal wxt 0\n");
	WriteAt(Entries.size() - 1, "set grid\n\n");
	WriteAt(Entries.size() - 1, "set title \"Fitness\" font \", 20\"\n");
	WriteAt(Entries.size() - 1, "set xlabel \"Generation\"\n");
	WriteAt(Entries.size() - 1, "set ylabel \"Fitness\"\n");
	WriteAt(Entries.size() - 1, "plot dir_ori.dat_bestsofar with linespoints title 'Best-so-far', dir_ori.dat_aver with linespoints title 'Average', dir_ori.dat_worstsofar with linespoints title 'Worst-so-far'\n\n");
	WriteAt(Entries.size() - 1, "pause 30\n");
	WriteAt(Entries.size() - 1, "reread");
	CloseAt(Entries.size() - 1);
	Entries.pop_back();

	NewFile(LogType::NONE, "RunPerformance");
	WriteAt(Entries.size() - 1, "ind_start = strstrt(ARG0, \"RunPerformance.txt\") - 1\n");
	WriteAt(Entries.size() - 1, "dir_ori = ARG0[0:ind_start]\n\n");
	WriteAt(Entries.size() - 1, "dat_bestsofar = 'BestSoFar.txt'\n");
	WriteAt(Entries.size() - 1, "dat_aver = 'Average.txt'\n");
	WriteAt(Entries.size() - 1, "dat_worstsofar = 'WorstSoFar.txt'\n\n");

	WriteAt(Entries.size() - 1, "set terminal wxt 0\n");
	WriteAt(Entries.size() - 1, "set grid\n\n");
	WriteAt(Entries.size() - 1, "set title \"Fitness\" font \", 20\"\n");
	WriteAt(Entries.size() - 1, "set xlabel \"Generation\"\n");
	WriteAt(Entries.size() - 1, "set ylabel \"Fitness\"\n");
	WriteAt(Entries.size() - 1, "plot dir_ori.dat_bestsofar with linespoints title 'Best-so-far', dir_ori.dat_aver with linespoints title 'Average', dir_ori.dat_worstsofar with linespoints title 'Worst-so-far'\n\n");
	WriteAt(Entries.size() - 1, "pause 5\n");
	WriteAt(Entries.size() - 1, "reread");
	CloseAt(Entries.size() - 1);
	Entries.pop_back();

	NewFile(LogType::NONE, "RunDiversity");
	WriteAt(Entries.size() - 1, "ind_start = strstrt(ARG0, \"RunDiversity.txt\") - 1\n");
	WriteAt(Entries.size() - 1, "dir_ori = ARG0[0:ind_start]\n\n");
	WriteAt(Entries.size() - 1, "dat_variance = 'Variance.txt'\n");
	WriteAt(Entries.size() - 1, "dat_mutrate = 'MutationRate.txt'\n\n");

	WriteAt(Entries.size() - 1, "set terminal wxt 2\n");
	WriteAt(Entries.size() - 1, "set grid\n\n");
	WriteAt(Entries.size() - 1, "set title \"Mutation vs Variance\" font \",20\"\n");
	WriteAt(Entries.size() - 1, "set xlabel \"Generation\"\n");
	WriteAt(Entries.size() - 1, "set ylabel \"Rate\"\n");
	WriteAt(Entries.size() - 1, "plot dir_ori.dat_variance with linespoints title 'Population Variance', dir_ori.dat_mutrate with linespoints title 'Mutation Rate'\n\n");
	WriteAt(Entries.size() - 1, "pause 5\n");
	WriteAt(Entries.size() - 1, "reread");
	CloseAt(Entries.size() - 1);
	Entries.pop_back();

	if (_Model.HasHoF)
	{
		NewFile(LogType::NONE, "RunHoF");
		WriteAt(Entries.size() - 1, "ind_start = strstrt(ARG0, \"RunHoF.txt\") - 1\n");
		WriteAt(Entries.size() - 1, "dir_ori = ARG0[0:ind_start]\n\n");
		WriteAt(Entries.size() - 1, "dat_hof = 'HallOfFame.txt'\n");

		WriteAt(Entries.size() - 1, "set terminal wxt 2\n");
		WriteAt(Entries.size() - 1, "set grid\n\n");
		WriteAt(Entries.size() - 1, "set title \"Hall of Fame\" font \",20\"\n");
		WriteAt(Entries.size() - 1, "set xlabel \"Index (P.)\"\n");
		WriteAt(Entries.size() - 1, "set ylabel \"Fitness\"\n");
		WriteAt(Entries.size() - 1, "plot dir_ori.dat_hof with points title 'Player'\n\n");
		WriteAt(Entries.size() - 1, "pause 5\n");
		WriteAt(Entries.size() - 1, "reread");
		CloseAt(Entries.size() - 1);
		Entries.pop_back();
	}

	if (_Model.HasElite && _Model.HasHoF)
	{
		NewFile(LogType::NONE, "RunElitismHoF");
		WriteAt(Entries.size() - 1, "ind_start = strstrt(ARG0, \"RunHoF.txt\") - 1\n");
		WriteAt(Entries.size() - 1, "dir_ori = ARG0[0:ind_start]\n\n");
		WriteAt(Entries.size() - 1, "dat_elitaver = 'EliteAver.txt'\n");
		WriteAt(Entries.size() - 1, "dat_top3aver = 'Top3Aver.txt'\n\n");

		WriteAt(Entries.size() - 1, "set terminal wxt 2\n");
		WriteAt(Entries.size() - 1, "set grid\n\n");
		WriteAt(Entries.size() - 1, "set title \"Elitism vs Top 3 in HoF\" font \",20\"\n");
		WriteAt(Entries.size() - 1, "set xlabel \"Generation\"\n");
		WriteAt(Entries.size() - 1, "set ylabel \"Fitness\"\n");
		WriteAt(Entries.size() - 1, "plot dir_ori.dat_elitaver with linespoints title 'Elite Average Fitness', dir_ori.dat_top3aver with linespoints title 'Top 3 Average Fitness'\n\n");
		WriteAt(Entries.size() - 1, "pause 5\n");
		WriteAt(Entries.size() - 1, "reread");
		CloseAt(Entries.size() - 1);
		Entries.pop_back();
	}
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