/*
 *   File Name: BCDInteger.h
 *   About:     Contains 'BCDInteger' class which represents integer as an array of bytes where each byte corresponds to
 *              a decimal digit.
 */


#pragma once

#include<iostream>
#include<string>
#include<exception>

namespace VedicMathLibrary
{
	using std::cout;
	using std::string;
	using std::exception;


	class BCDInteger
	{
		#pragma region Data Members

		private:
		bool     Positive = true;
		size_t   Capacity = 0;
		size_t   Length = 0;
		char*    Digits = NULL;

		const size_t NPOS = (-1);

		#pragma endregion		


		#pragma region Ctor & Dtor

		public:
		BCDInteger() {}
		BCDInteger(size_t initialCapacity)
		{
			this->Resize(initialCapacity, false);
		}
		BCDInteger(const string& str)
		{
			//Parse given String
			BCDInteger& Temp = Parse(str);

			//Initilize this instance with parsed values
			this->Capacity = Temp.Capacity;
			this->Digits = Temp.Digits;
			this->Length = Temp.Length;
			this->Positive = Temp.Positive;
		}
		~BCDInteger()
		{
			if (Digits != NULL)
				delete[] Digits;
		}

		#pragma endregion


		#pragma region Public Utility Methods
		public:
		bool IsPositive() const { return Positive; }
		size_t GetCapacity() const { return Capacity; }
		size_t GetLength() const { return Length; }

		char& operator[](size_t i) { return Digits[i]; }
		char  at(size_t i)const
		{
			if (i < Length)
				return Digits[i];
			else
				return 0;
		}
		
		void Print()
		{
			if (this->Positive == false)
				cout << '-';

			for (size_t i = 1; i <= this->Length; i++)
				cout << char(this->Digits[this->Length - i] + '0');

		}
		void Resize(size_t newCapacity, bool keepOldData)
		{
			//If current capacity is already sufficient then return immediately
			if (newCapacity <= this->Capacity)
				return;

			//Allocate new array
			char *Temp = new char[newCapacity];
			char *OldDigits = this->Digits;
			size_t OldLength = this->Length;

			this->Digits = Temp;
			this->Capacity = newCapacity;
			this->Length = 0;


			//If required then restore old data
			if (keepOldData && OldDigits != NULL)
			{
				for (size_t i = 0; i < OldLength; i++)
					this->Digits[i] = OldDigits[i];

				this->Length = OldLength;
			}

			//Delete old digits
			if (OldDigits != NULL)
				delete[] OldDigits;
		}
		void Swap(BCDInteger& target)
		{
			std::swap(this->Length, target.Length);
			std::swap(this->Capacity, target.Capacity);
			std::swap(this->Positive, target.Positive);
			std::swap(this->Digits, target.Digits);
		}
		void Clear()
		{
			if (Digits != NULL)
				delete[] Digits;
			Length = 0;
			Capacity = 0;
			this->Positive = true;
		}
		void Normalize()
		{
			while (Length > 0 && Digits[Length - 1] == 0)
				--Length;

			if (Length == 1 && Digits[0] == 0)
				this->Positive = true;
		}
		#pragma endregion


		#pragma region Overloaded Operators
		public:
		//Comparision
		bool        operator==(const BCDInteger& other)const
		{
			//First check if Length can determine equality
			if (this->Length != other.Length)
				return false;

			//If length is equal then check digit by digit
			for (size_t i = 0; i < this->Length; i++)
				if (this->Digits[i] != other.Digits[i])
					return false;

			//If no mismatch found then return true
			return true;
		}
		bool        operator<(const BCDInteger& other)const
		{
			//First check if Length can determine lesser
			if (this->Length < other.Length)
				return true;

			//If length is equal then check digit by digit
			for (size_t i = this->Length - 1; i != NPOS; i--)
			{
				if (this->Digits[i] < other.Digits[i])
					return true;

				if (this->Digits[i] > other.Digits[i])
					return false;
			}
			//If numbers are equal then return false
			return false;
		}
		bool        operator<=(const BCDInteger& other)const
		{
			return (this->operator<(other)) || (this->operator==(other));
		}

		//Assignment
		BCDInteger& operator=(const BCDInteger& source)
		{
			//Resize if necessery
			if (source.Length > this->Capacity)
				this->Resize(source.Length, false);

			//Copy data from source
			for (size_t i = 0; i < source.Length; i++)
				this->Digits[i] = source.Digits[i];

			this->Length = source.Length;

			//Return a reference to this object to allow chaining of = operator
			return *this;
		}

		//Mathematical
		BCDInteger& operator+=(const BCDInteger& n)
		{
			//Resize if necessery
			if (this->Capacity < n.Length)
				this->Resize(n.Length + 10, true);

			size_t i = 0;
			char Carry = 0;
			char Ch;

			//Add 
			size_t MaxLength = this->Length >= n.Length ? this->Length : n.Length;

			for (; i < MaxLength; i++)
			{
				Ch = this->at(i) + n.at(i) + Carry;
				this->Digits[i] = Ch % 10;
				Carry = Ch / 10;
			}

			//Accomodate Carry if necessery
			if (Carry != 0)
			{
				//Resize if necessery
				if (i >= this->Capacity)
					this->Resize(this->Capacity + 10, true);

				this->Digits[i++] = Carry;
			}

			//Save Length
			this->Length = i;

			return (*this);
		}

		#pragma endregion


		#pragma region Vedic Methods
		private:
		long VedicQuotientCrossMulti(char* flagDigits, size_t flagDigitCount, char* quotientDigits, size_t quotientDigitCount)const
		{
			long Result = 0;
			size_t MinCount = flagDigitCount < quotientDigitCount ? flagDigitCount : quotientDigitCount;

			for (size_t i = 0, j = flagDigitCount - 1; i < MinCount; i++, j--)
				Result += (flagDigits[j] * quotientDigits[i]);

			return Result;
		}
		void VedicRemainder(const char* flagDigits, size_t flagDigitCount, const char* qDigits, size_t qDigitCount,
			                const char* remainderDigits, size_t remainderDigitCount, long r, BCDInteger* result)const
		{
			//Set result as negative so that we can simply return to signify a failure
			result->Positive = false;


			//Step 1: Prepare storage for Remainder
			result->Resize(remainderDigitCount + 3, false);
			for (size_t j = 0; j < remainderDigitCount; j++)
				result->Digits[j] = remainderDigits[j];

			result->Length = remainderDigitCount;

			long Tr = r;
			while (Tr > 0)
			{
				result->Digits[result->Length++] = Tr % 10;
				Tr = Tr / 10;
			}


			//Step 2:Prepare Storage for Cross-Product of flagDigits and qDigits
			//       Also reserve space for Zeros added by multiplication of 10
			size_t CrossProductCapacity = flagDigitCount + 2;
			size_t Tm = flagDigitCount;
			while (Tm > 0)
			{
				CrossProductCapacity++;
				Tm = Tm / 10;
			}
			char* CrossProduct = new char[CrossProductCapacity];

			//Step 3: Calculate remainder
			for (size_t x = flagDigitCount - 1; x != NPOS; --x)
			{
				for (size_t i = 0; i < CrossProductCapacity; i++)          //Zero-Out Storage
					CrossProduct[i] = 0;

				//SubStep 1: Calculate Cross Product
				size_t CrossProductLength = 0, k;
				char   T = 0, Carry = 0;
				for (size_t i = 0, j = x; i <= x && i < qDigitCount; i++, --j)
				{
					T = flagDigits[j] * qDigits[i];
					k = x;

					while (T > 0 || Carry > 0)
					{
						CrossProduct[k] += (T % 10 + Carry);
						Carry = CrossProduct[k] / 10;

						CrossProduct[k] %= 10;
						T /= 10;

						++k;
					}

					if (k > CrossProductLength)
						CrossProductLength = k;
				}


				//If CrossProductLength is greater than result's length then remainder is 
				//negative and we should return
				if (CrossProductLength > result->Length)
					return;

				//SubStep 2: Subtract CrossProduct from Remainder(result)
				Carry = 1;
				for (size_t i = 0; i < CrossProductLength; i++)
				{
					T = result->Digits[i] + (9 - CrossProduct[i]) + Carry;
					result->Digits[i] = T % 10;
					Carry = T / 10;
				}
				for (size_t i = CrossProductLength; i < result->Length; i++)
				{
					T = result->Digits[i] + 9 + Carry;
					result->Digits[i] = T % 10;
					Carry = T / 10;
				}

				//If Carry == 0 then result is negative
				if (Carry == 0)
					return;

				//TODO: Trim result
			}

			result->Positive = true;
			return;
		}

		public:
		void VedicMultiplication(const BCDInteger& Multiplier, BCDInteger& Product)const
		{
			size_t MaxLength = (this->Length > Multiplier.Length) ? this->Length : Multiplier.Length;

			//Object holding result of this method
			Product.Resize(this->Length + Multiplier.Length, false);
			for (size_t i = 0; i < Product.Capacity; i++)
				Product.Digits[i] = 0;


			//Initialize X & Y such that X.Length >= Y.Length
			const BCDInteger *X, *Y;
			if (this->Length >= Multiplier.Length)
			{
				X = this;
				Y = &Multiplier;
			}
			else
			{
				X = &Multiplier;
				Y = this;
			}


			size_t j = 0;


			//Phase 1
			for (size_t i = 0; i < X->Length; i++)
			{
				//Cross-Multiplication
				char Ch;
				for (size_t start = 0, end = i; start <= i; ++start, --end)
				{
					Ch = X->at(start) * Y->at(end) + Product.Digits[j];
					Product.Digits[j] = Ch % 10;
					Product.Digits[j + 1] += Ch / 10;
				}

				++j;


			}

			//Phase 2
			for (size_t i = 1; i < Y->Length; i++)
			{
				//Cross-Multiplication
				char Ch;
				for (size_t start = i, end = MaxLength - 1; start < MaxLength; ++start, --end)
				{
					Ch = X->at(start) * Y->at(end) + Product.Digits[j];
					Product.Digits[j] = Ch % 10;
					Product.Digits[j + 1] += Ch / 10;
				}

				++j;
			}


			//Set Proper Sign
			Product.Positive = ((this->Positive && Multiplier.Positive) || (!this->Positive && !Multiplier.Positive));

			Product.Normalize();
		}
		void VedicDivision(const BCDInteger& Divisor, BCDInteger& Quotient, BCDInteger& Remainder)const
		{
			//Check if Divisor is Zero
			if (Divisor.Length == 1 && Divisor.Digits[0] == 0)
				throw exception("Invalid Operation! Divison by zero.");

			//Check if Diviser is of longer than this
			if (this->Length < Divisor.Length)
			{
				Quotient.Clear();
				Remainder = (*this);
				return;
			}

			//Split Diviser 
			char  DiviserDigit = Divisor.Digits[Divisor.Length - 1];
			char* FlagDigits = Divisor.Digits;
			size_t FlagDigitCount = Divisor.Length - 1;

			//If DiviserDigit is less then 5 then take another digit with it
			//if ((DiviserDigit < 5) && (Divisor.Length>1))
			//{
			//	DiviserDigit = DiviserDigit * 10 + Divisor.Digits[Divisor.Length - 2];
			//	--FlagDigitCount;
			//}
			
			//Split Dividend
			char* RemainderDigits = this->Digits;
			const size_t RemainderDigitcount = FlagDigitCount;

			char* QuotientDigits = this->Digits + RemainderDigitcount;
			const size_t QuotientDigitCount = this->Length - RemainderDigitcount;

			//Quotient
			Quotient.Resize(QuotientDigitCount, false);
			Quotient.Length = Quotient.Capacity;


			//Indices
			size_t i = QuotientDigitCount - 1,
				j = QuotientDigitCount;


			//Effective Dividend                   
			long ED = QuotientDigits[i], q = 0, r = 0, T1 = 0, T2 = 0;

			bool Run = true;
			while (Run)
			{
				if (ED >= 0)
				{
					if (i == NPOS)
					{
						VedicRemainder(FlagDigits, FlagDigitCount, Quotient.Digits, Quotient.Length, RemainderDigits, RemainderDigitcount, 
							           r, &Remainder);
						if (Remainder.Positive)
							Run = false;
						else
						{
							ED = -1;
							continue;
						}
					}
					else
					{
						q = ED / DiviserDigit;
						r = ED % DiviserDigit;

						--i;
						Quotient.Digits[--j] = q;
					}
				}
				else
				{
					if (Quotient.Digits[j] == 0) //Backtracking
					{
						++j;
						++i;
						long T = VedicQuotientCrossMulti(FlagDigits, FlagDigitCount, Quotient.Digits + j, QuotientDigitCount - j);
						r += T;
						r /= 10;

						//Change ED to a negative value so that in next iteration we again end up here
						ED = -1;
						continue;

					}
					else
					{
						--(Quotient.Digits[j]);
						r += DiviserDigit;

						if (i == NPOS)
							ED = 1;
					}
				}

				//Cross Multiply Quotient & FlagDigits
				if (i != NPOS)
				{
					T1 = VedicQuotientCrossMulti(FlagDigits, FlagDigitCount, Quotient.Digits + j, QuotientDigitCount - j);

					T2 = r * 10 + QuotientDigits[i];
					ED = T2 - T1;
				}

			}

			//Set Proper Sign
			Quotient.Positive  = ((this->Positive && Divisor.Positive) || (!this->Positive && !Divisor.Positive));
			Remainder.Positive = ((this->Positive && Divisor.Positive) || (this->Positive && !Divisor.Positive));

			//Normalize
			Quotient.Normalize();
			Remainder.Normalize();
		}

		#pragma endregion


		#pragma region Traditional Methods
		public:
		void TraditionalMultiplication(const BCDInteger& Multiplier, BCDInteger& Product)const
		{
			Product.Resize(this->Length + Multiplier.Length, false);

			for (size_t i = 0; i < Product.Capacity; i++)
				Product.Digits[i] = 0;

			size_t Shift = 0, Carry = 0, i, j;

			for (i = 0; i < Multiplier.Length; i++)
			{
				Carry = 0;
				for (j = 0; j < this->Length; j++)
				{
					char M = this->Digits[j] * Multiplier.Digits[i] + Product.Digits[Shift + j] + Carry;
					Carry = M / 10;
					Product.Digits[Shift + j] = M % 10;

				}
				Product.Digits[Shift + j] = Carry;

				++Shift;
			}

			//Set proper sign
			Product.Positive = ((this->Positive && Multiplier.Positive) || (!this->Positive && !Multiplier.Positive));				

			Product.Normalize();
		}
		void TraditionalDivision(const BCDInteger& Divisor, BCDInteger& Quotient, BCDInteger& Remainder )const
		{
			//Check if Divisor is Zero
			if (Divisor.Length == 1 && Divisor.Digits[0] == 0)
				throw exception("Invalid Operation! Divison by zero.");

			//Check if Diviser is of longer than this
			if (this->Length < Divisor.Length)
			{
				Quotient.Clear();
				Remainder = (*this);
				return;
			}

			//Calculate digits in Quotient
			size_t QuotientDigitCount = (this->Length - Divisor.Length) + 1;

			//Create space to hold Quotient
			Quotient.Resize(QuotientDigitCount, false);
			Quotient.Length = QuotientDigitCount;
			size_t j = QuotientDigitCount - 1;

			//Create Effective dividend
			Remainder = (*this);
			char *EDOriginalArray = Remainder.Digits;
			Remainder.Digits = (Remainder.Digits + QuotientDigitCount) - 1;
			Remainder.Length = Divisor.Length;

			//Create Space to hold result of multiplication between Guess and divise
			BCDInteger MultiResult(Divisor.Length + 1);

			//Other variables
			char Guess = 0, Carry = 0, T;
			bool CreateNewGuess = true;
			size_t i = QuotientDigitCount - 1, k = 0;


			//Master Loop
			while (i != NPOS)
			{
				//Guess
				if (CreateNewGuess)
				{
					T = Remainder[Divisor.Length - 1];
					if (Remainder.Length > Divisor.Length)
						T += 10 * Remainder[Divisor.Length];

					Guess = T / Divisor.Digits[Divisor.Length - 1];
					if (Guess > 9)
						Guess = 9;
				}
				else
					Guess--;

				//Multiplication
				Carry = 0;
				for (k = 0; k < Divisor.Length; k++)
				{
					T = Divisor.Digits[k] * Guess + Carry;
					Carry = T / 10;
					MultiResult.Digits[k] = T % 10;
				}
				MultiResult.Digits[k] = Carry;

				if (Carry > 0)
					MultiResult.Length = Divisor.Length + 1;
				else
					MultiResult.Length = Divisor.Length;

				//Ensure Mutiplication result is less then ED
				if (Remainder < MultiResult)
				{
					CreateNewGuess = false;
					continue;
				}

				//Subtraction
				Carry = 1;
				for (k = 0; k < Remainder.Length; k++)
				{
					T = Remainder.Digits[k] + (9 - MultiResult[k]) + Carry;
					Carry = T / 10;
					Remainder.Digits[k] = T % 10;
				}

				if (Remainder.Digits[Remainder.Length - 1] != 0)
					++Remainder.Length;

				Quotient.Digits[j--] = Guess;
				CreateNewGuess = true;
				Remainder.Digits--;
				i--;

			}

			//Restore original digits array for  ED
			Remainder.Digits = EDOriginalArray;

			//Set Proper Sign
			Quotient.Positive  = ((this->Positive && Divisor.Positive) || (!this->Positive && !Divisor.Positive));
			Remainder.Positive = ((this->Positive && Divisor.Positive) || (this->Positive && !Divisor.Positive));

			//Normalize
			Quotient.Normalize();
			Remainder.Normalize();				
		}

		#pragma endregion


		#pragma region Static Methods
		public:
		static BCDInteger& Parse(const string& valueString)
		{

			//Validate valueString
			if (!valueString.empty())
			{
				char ch = valueString[0];
				if (ch != '+' && ch != '-' && (ch < '1' || ch > '9'))
					throw exception("Error parsing valueString! Invalid character encountered.");

				if ((ch == '+' || ch == '-') && (valueString.length() == 1))
					throw exception("Error parsing valueString! Invalid format.");

				for (size_t i = 1; i < valueString.length(); i++)
				{
					ch = valueString[i];
					if (ch < '0' || ch > '9')
						throw exception("Error parsing valueString! Invalid character encountered.");
				}

			}

			//Create a BCDInteger 
			BCDInteger* Value = new BCDInteger();

			if (valueString.empty() == false)
			{
				//Calculate sign and digit Count

				size_t DigitCount = valueString.length();

				char ch = valueString[0];
				if (ch == '-')
				{
					Value->Positive = false;
					DigitCount--;
				}
				else
					if (ch == '+')
					{
						Value->Positive = true;
						DigitCount--;
					}
					else
					{
						Value->Positive = true;
					}


				//Allocate memory
				Value->Digits = new char[DigitCount];
				Value->Capacity = DigitCount;

				//Fill digits
				for (size_t i = 0, j = valueString.length() - 1; i < DigitCount; ++i, --j)
					Value->Digits[i] = valueString[j] - '0';

				Value->Length = DigitCount;
			}

			return *Value;
		}
		#pragma endregion
	};


}
