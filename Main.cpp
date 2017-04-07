
#include<Windows.h>
#include<cstdlib>
#include<ctime>
#include<fstream>
#include"BCDInteger.h"


using namespace VedicMathLibrary;
using namespace std;

void RunSingleTest(string StrA, string StrB,const HANDLE& ThreadHandle, UINT64& AvgVedic, UINT64& AvgTraditional, size_t Cycles = 10000)
{
	BCDInteger a(StrA), b(StrB), *Result;

	UINT64 CyclesAfterTask, CyclesBeforeTask, Sum;
	
	Sum = 0;

	for (size_t i = 0; i < Cycles; i++)
	{
		QueryThreadCycleTime(ThreadHandle, &CyclesBeforeTask);
		Result = a.TraditionalMultiplication(b);
		QueryThreadCycleTime(ThreadHandle, &CyclesAfterTask);
		
		Sum += (CyclesAfterTask - CyclesBeforeTask);

		delete Result;
	}

	AvgTraditional = (Sum / Cycles);

	Sum = 0;

	for (size_t i = 0; i < Cycles; i++)
	{
		QueryThreadCycleTime(ThreadHandle, &CyclesBeforeTask);
		Result = a.VedicMultiplication(b);
		QueryThreadCycleTime(ThreadHandle, &CyclesAfterTask);

		Sum += (CyclesAfterTask - CyclesBeforeTask);

		delete Result;
	}

	AvgVedic = (Sum / Cycles);
}
void RunTests(string ResultFileName, size_t MaxLength)
{
	string StrA("3646474"), StrB("2353455");
	srand((unsigned)time(NULL));

	HANDLE Th = GetCurrentThread();
	UINT64 AvgVedic = 0, AvgTraditional = 0;

	ofstream File(ResultFileName + ".csv");

	if (File)
	{
		File << "Number Size,Vedic Math,Traditional" << endl;

		while (StrA.length() < MaxLength)
		{
			//Append random digit
			int digit = rand() % 10;
			StrA.append(to_string(digit));
			digit = rand() % 10;
			StrB.append(to_string(digit));

			cout << "\n Running Test with Length: " << StrA.length();

			RunSingleTest(StrA, StrB, Th, AvgVedic, AvgTraditional);

			//Write data
			File << StrA.length() << "," << AvgVedic << "," << AvgTraditional << endl;
		}
		File.close();
	}
	else
	{
		cout << "Error: Unable to open file for writing test result";
	}
}

int main()
{
	RunTests("TestTestResult", 100);
	
	system("pause>nul");

	return 0;
}