
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

	cout << "[TRD...]";
	for (size_t i = 0; i < Cycles; i++)
	{
		QueryThreadCycleTime(ThreadHandle, &CyclesBeforeTask);
		Result = a.NewTraditionalQuotient(b);
		QueryThreadCycleTime(ThreadHandle, &CyclesAfterTask);
		
		Sum += (CyclesAfterTask - CyclesBeforeTask);

		delete Result;
	}

	AvgTraditional = (Sum / Cycles);

	Sum = 0;
	cout << "[VED...]";
	for (size_t i = 0; i < Cycles; i++)
	{
		QueryThreadCycleTime(ThreadHandle, &CyclesBeforeTask);
		Result = a.VedicQuotient(b);
		QueryThreadCycleTime(ThreadHandle, &CyclesAfterTask);

		Sum += (CyclesAfterTask - CyclesBeforeTask);

		delete Result;
	}

	AvgVedic = (Sum / Cycles);
}
void RunTests(string ResultFileName, size_t MaxLength)
{
	string StrA("3646474"), StrB("5353455");
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
			//digit = rand() % 10;
			//StrB.append(to_string(digit));

			cout << "\n Running Test with Length: " << StrA.length();

			RunSingleTest(StrA, StrB, Th, AvgVedic, AvgTraditional);

			cout << "  Tr: " << AvgTraditional << "  , VEd: " << AvgVedic;

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
	RunTests("SubtractTestResult", 100);
	
	BCDInteger a("556677996"), b("9444"), d("24543533");

	auto c = d.VedicQuotient(b);

	c->Print();

	cout << "\n";

	c = d.NewTraditionalQuotient(b);
	c->Print();

	system("pause>nul");

	return 0;
}