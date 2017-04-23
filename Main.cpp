

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

bool RunSingleTest(string StrA, string StrB, const HANDLE& ThreadHandle, UINT64& AvgVedic, UINT64& AvgTraditional, size_t Cycles = 10000)
{
	BCDInteger a(StrA), b(StrB), q1, r1, q2, r2;
	//cpp_int b_a(StrA), b_b(StrB),b_c;
	UINT64 CyclesAfterTask, CyclesBeforeTask, Sum;

	Sum = 0;

	for (size_t i = 0; i < Cycles; i++)
	{
		QueryThreadCycleTime(ThreadHandle, &CyclesBeforeTask);
		a.TraditionalDivision(b, q1, r1);
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
		a.VedicDivision(b, q2, r2);
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

	//Ensure results are same
	if (q1 != q2 || r1 != r2)
		return false;
	else
		return true;

}
void RunTests(string ResultFileName, size_t MaxLength)
{
	string StrA("39485849754357945803457"), StrB("1465548498"), Ra, Rb;

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

			cout << "\nRunning Test with Length: " << StrA.length();

			//cout << "\n[A = " << Ra << "][B = " << Rb << "]";

			//cout << "\n[A = " << StrA << "][B = " << StrB << "]";

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
	BCDInteger a("3646474104"), b("846387"), q, r;

	a.VedicDivision(b, q, r);

	q.Print();

	cout << "\n";
	//c = a.TraditionalDivision(b);

	//c->Print();
}


void RunRandomNumberTest(int MaxLen, size_t Cycles)
{
	ifstream TestFile("F:\\College\\Sem 8\\Project\\Library\\VedicMathLibrary\\Data\\TestFiles\\RandomNumbers_Set2.txt");
	ofstream ResultFile("F:\\College\\Sem 8\\Project\\Library\\VedicMathLibrary\\Data\\ResultFiles\\RandomNumbers_Set1.csv");

	HANDLE Th = GetCurrentThread();
	UINT64 AvgVedic = 0, AvgTraditional = 0;
	INT64  S = 0;


	if (TestFile.is_open() && ResultFile.is_open())
	{
		string Divisor, Dividend;

		//Take FirstNumber as divisor
		TestFile >> Divisor;

		for (size_t i = Divisor.length(); i < MaxLen; i++)
		{
			TestFile >> Dividend;

			if (!RunSingleTest(Dividend, Divisor, Th, AvgVedic, AvgTraditional, Cycles))
			{
				cout << "\nResult Mismatch!";
			}

			cout << "  Tr: " << AvgTraditional << "  , VEd: " << AvgVedic << endl;
			S += (AvgTraditional - AvgVedic);
		}

		cout << "\n Avg Difference: " << (S / (Dividend.length() - Divisor.length()));

		TestFile.close();
		ResultFile.close();
	}
}



int main()
{
	srand((unsigned)time(NULL));

	RunRandomNumberTest(200, 1);

	system("pause>nul");

	return 0;
}