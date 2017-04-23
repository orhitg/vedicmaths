

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
		size_t i = 0;
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



void RunRandomNumberTest(size_t MaxLen, size_t Cycles)
{
	ifstream TestFile("F:\\College\\Sem 8\\Project\\Library\\VedicMathLibrary\\Data\\TestFiles\\RandomNumbers_Set3.txt");
	ofstream ResultFile("F:\\College\\Sem 8\\Project\\Library\\VedicMathLibrary\\Data\\ResultFiles\\RandomNumbers_Set3.csv");

	HANDLE Th = GetCurrentThread();
	UINT64 AvgVedic = 0, AvgTraditional = 0;
	INT64  S = 0;


	if (TestFile.is_open() && ResultFile.is_open())
	{
		string Divisor, Dividend;

		//Take FirstNumber as divisor
		TestFile >> Divisor;

		ResultFile << "Size, Traditional, Vedic" << endl;

		for (size_t i = Divisor.length(); i < MaxLen; i++)
		{
			TestFile >> Dividend;

			if (!RunSingleTest(Dividend, Divisor, Th, AvgVedic, AvgTraditional, Cycles))
			{
				cout << "\nResult Mismatch!\n[A="<<Dividend<<"][B="<<Divisor;
				
			}

			cout << "  Tr: " << AvgTraditional << "  , VEd: " << AvgVedic << endl;

			ResultFile << Dividend.length() << "," << AvgTraditional << "," << AvgVedic;

			S += (AvgTraditional - AvgVedic);
		}

		cout << "\n Avg Difference: " << (S / (Dividend.length() - Divisor.length()));

		TestFile.close();
		ResultFile.close();
	}
}


void Temp()
{
	BCDInteger 
		a("1166739413323874215601794898464527536197167356991178614276667454445594128426628765227157535300671034332788171214754436231740780110056561143927762347133102232584693155897291580159"),
		b("26736038687930346029045807261129522095961671393420620767872553501499846701109304452513005"), q, r;

	a.VedicDivision(b, q, r);

	q.Print();

	cout << "\n";

	r.Print();

	cout << "\n";
	
	a.TraditionalDivision(b, q, r);

	q.Print();

	cout << "\n";

	r.Print();

	cout << "\n";//c = a.TraditionalDivision(b);

	//c->Print();
}


int main()
{
	srand((unsigned)time(NULL));

	RunRandomNumberTest(200, 10000);

	//Temp();

	system("pause>nul");

	return 0;
}