
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
		Result = a.TraditionalQuotient(b);
		QueryThreadCycleTime(ThreadHandle, &CyclesAfterTask);
		
		Sum += (CyclesAfterTask - CyclesBeforeTask);

		cout << "\n";
		Result->Print();

		delete Result;
	}

	AvgTraditional = (Sum / Cycles);

	Sum = 0;
	for (size_t i = 0; i < Cycles; i++)
	{
		QueryThreadCycleTime(ThreadHandle, &CyclesBeforeTask);
		Result = a.VedicQuotient(b);
		QueryThreadCycleTime(ThreadHandle, &CyclesAfterTask);

		Sum += (CyclesAfterTask - CyclesBeforeTask);

		cout << "\n";
		Result->Print();

		delete Result;
	}

	AvgVedic = (Sum / Cycles);
}
void RunTests(string ResultFileName, size_t MaxLength)
{
	string StrA("3646474"), StrB("459");
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
			
			//cout << "[A = " << StrA << "][B = " << StrB << "]";

			RunSingleTest(StrA, StrB, Th, AvgVedic, AvgTraditional, 1);

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

void Temp()
{
	BCDInteger a("446663543486987464354469897076"), b("668567474766587768705"), d("123"), e("50607");

	BCDInteger* c = a.TraditionalQuotient(b);

	c->Print();

	c = e.VedicQuotient(d);

	cout << "\n";

	c->Print();

}


int main()
{
	RunTests("QuotientTestResult", 50);
	
	//Temp();

	system("pause>nul");

	return 0;
}