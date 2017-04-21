

#include<Windows.h>
#include<cstdlib>
#include<ctime>
#include<fstream>
#include"BCDInteger.h"

using namespace VedicMathLibrary;
using namespace std;



//srand() should have been called already
string GenerateRandomNumberString(int MaxLen)
{
	int Len = rand() % MaxLen + 1;
	string Number;

	int Digit = rand() % 9 + 1;
	Number.append(to_string(Digit));


	for (int i = 0; i < Len; i++)
	{
		Digit = rand() % 10;
		Number.append(to_string(Digit));
	}

	return Number;
}

void RunSingleTest(string StrA, string StrB,const HANDLE& ThreadHandle, UINT64& AvgVedic, UINT64& AvgTraditional, size_t Cycles = 10000)
{
	BCDInteger a(StrA), b(StrB), q, r;
	//cpp_int b_a(StrA), b_b(StrB),b_c;
	UINT64 CyclesAfterTask, CyclesBeforeTask, Sum;
	
	Sum = 0;

	for (size_t i = 0; i < Cycles; i++)
	{
		QueryThreadCycleTime(ThreadHandle, &CyclesBeforeTask);
		a.TraditionalDivision(b, q, r);
		//b_c = b_a / b_b;
		QueryThreadCycleTime(ThreadHandle, &CyclesAfterTask);
		
		Sum += (CyclesAfterTask - CyclesBeforeTask);

		/*
		cout << "\n[Q = ";
		q.Print();
		cout << "][R = ";
		r.Print();
		cout << "]";
		//*/
	}

	AvgTraditional = (Sum / Cycles);

	Sum = 0;
	for (size_t i = 0; i < Cycles; i++)
	{
		QueryThreadCycleTime(ThreadHandle, &CyclesBeforeTask);
		a.VedicDivision(b,q,r);
		QueryThreadCycleTime(ThreadHandle, &CyclesAfterTask);

		Sum += (CyclesAfterTask - CyclesBeforeTask);


		/*
		cout << "\n[Q = ";
		q.Print();
		cout << "][R = ";
		r.Print();
		cout << "]";

		//*/
	}

	AvgVedic = (Sum / Cycles);
}
void RunTests(string ResultFileName, size_t MaxLength)
{
	string StrA("39485849754357945803457"), StrB("72634759759645435"), Ra, Rb;

	HANDLE Th = GetCurrentThread();
	UINT64 AvgVedic = 0, AvgTraditional = 0;

	ofstream File(ResultFileName + ".csv");

	if (File)
	{
		File << "Number Size,Vedic Math,Traditional" << endl;
		int i = 0;
		while (i++ < MaxLength)
		{
			//Append random digit
			int digit = rand() % 10;
			StrA.append(to_string(digit));
			//digit = rand() % 10;
			//StrB.append(to_string(digit));

			Ra = GenerateRandomNumberString(MaxLength);
			Rb = GenerateRandomNumberString(MaxLength);
			
			//cout << "\nRunning Test with Length: " << StrA.length();
			
			//cout << "\n[A = " << Ra << "][B = " << Rb << "]";

			cout << "\n[A = " << StrA << "][B = " << StrB << "]";

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

void Temp()
{
	BCDInteger a("3646474104"), b("846387"), q,r;

	a.VedicDivision(b,q,r);

	q.Print();
	
	cout << "\n";
	//c = a.TraditionalDivision(b);

	//c->Print();
}


int main()
{
	srand((unsigned)time(NULL));

	RunTests("DivisionTestResult", 100);
	
	//Temp();

	system("pause>nul");

	return 0;
}