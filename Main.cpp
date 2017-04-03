
#include<Windows.h>
#include"BCDInteger.h"

using namespace VedicMathLibrary;

int main()
{
	BCDInteger a("9999999999999999999999999999999999999999999999999999999999"), b("11111");

	UINT64 Cb, Ca, Sum;
	HANDLE th = GetCurrentThread();

	size_t Cycles = 10000;
	

	Sum = 0;

	for (size_t i = 0; i < Cycles; i++)
	{
		QueryThreadCycleTime(th, &Ca);
		a.TraditionalMultiplication(b);
		QueryThreadCycleTime(th, &Cb);

		Sum += (Cb - Ca);
	}

	cout << "\n Average (Traditional) = " << (Sum / Cycles);


	Sum = 0;

	for (size_t i = 0; i < Cycles; i++)
	{
		QueryThreadCycleTime(th, &Ca);
		a*b;
		QueryThreadCycleTime(th, &Cb);

		Sum += (Cb - Ca);
	}

	cout << "\n Average (Vedic) = " << (Sum / Cycles);


	
	system("pause>nul");

	return 0;
}