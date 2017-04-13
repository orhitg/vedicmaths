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
		BCDInteger(){}
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

		
		#pragma region Private Methods
		private:
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
		}
	    void CrossMultiply(const BCDInteger& a, const BCDInteger& b, size_t startIndex, size_t endIndex, BCDInteger& result)const
		{
			if (startIndex <= endIndex)
			{
				//Clear result
				//result.Length = 0;

				char Ch;
				static BCDInteger Temp(2);

				//Calculate result
				for (size_t i = startIndex, j = endIndex; i <= endIndex; i++, j--)
				{
					Ch = (a.at(i) * b.at(j));

					if (Ch == 0)
						Temp.Length = 0;
					else
					if (Ch < 10)
					{
						Temp.Length = 1;
						Temp.Digits[0] = Ch;
					}
					else
					{
						Temp.Length = 2;
						Temp.Digits[0] = Ch % 10;
						Temp.Digits[1] = Ch / 10;
					}					

					result += Temp;
				}
			}
			else
				throw exception("startIndex is greater then endIndex");
		}

		#pragma endregion

		
		#pragma region Accessors
		public:
		bool IsPositive() const { return Positive; }
		size_t GetCapacity() const { return Capacity; }
		size_t GetLength() const { return Length; }

		char& operator[](size_t i){ return Digits[i]; }
		char  at(size_t i)const
		{
			if (i < Length)
				return Digits[i];
			else
				return 0;
		}
		#pragma endregion


		#pragma region Overloaded Operators
		public:
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
			for (size_t i = this->Length - 1; i != NPOS; i++)
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

			for (;i < MaxLength ; i++)
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


		#pragma region Public Methods
		public:
		void Print()
		{
			if (this->Positive == false)
				cout << '-';

			for (size_t i = 1; i <= this->Length; i++)
				cout << char( this->Digits[this->Length - i] + '0') ;
		}

		BCDInteger* VedicMultiplication(const BCDInteger& n)const
		{
			size_t MaxLength = (this->Length > n.Length) ? this->Length : n.Length;

			//Object holding result of this method
			BCDInteger* Product = new BCDInteger(this->Length + n.Length);
			for (size_t i = 0; i < Product->Capacity; i++)
				Product->Digits[i] = 0;


			//Initialize X & Y such that X.Length >= Y.Length
			const BCDInteger *X, *Y;
			if (this->Length >= n.Length)
			{
				X = this;
				Y = &n;
			}
			else
			{
				X = &n;
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
					Ch = X->at(start) * Y->at(end) + Product->Digits[j];
					Product->Digits[j] = Ch % 10;
					Product->Digits[j + 1] += Ch / 10;
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
					Ch = X->at(start) * Y->at(end) + Product->Digits[j];
					Product->Digits[j] = Ch % 10;
					Product->Digits[j + 1] += Ch / 10;
				}

				++j;
			}


			//Save Product Length
			if (Product->Digits[j] == 0)
				Product->Length = j;
			else
				Product->Length = j + 1;

			//Set Proper Sign
			if ((this->Positive && n.Positive) || (!this->Positive && !n.Positive))
				Product->Positive = true;
			else
				Product->Positive = false;


			//Return result
			return Product;
		}
		BCDInteger* TraditionalMultiplication(const BCDInteger& n)const
		{
			BCDInteger *Product = new BCDInteger(this->Length + n.Length);

			for (size_t i = 0; i < Product->Capacity; i++)
				Product->Digits[i] = 0;

			size_t Shift = 0, Carry = 0, i, j;

			for (i = 0; i < n.Length; i++)
			{
				Carry = 0;
				for (j = 0; j < this->Length; j++)
				{
					char M = this->Digits[j] * n.Digits[i] + Product->Digits[Shift + j] + Carry;
					Carry = M / 10;
					Product->Digits[Shift + j] = M % 10;

				}
				Product->Digits[Shift + j] = Carry;

				++Shift;
			}

			if (Carry == 0)
				Product->Length = Product->Capacity - 1;
			else
				Product->Length = Product->Capacity;


			if ((this->Positive && n.Positive) || (!this->Positive && !n.Positive))
				Product->Positive = true;
			else
				Product->Positive = false;

			return Product;
		}


		BCDInteger* VedicQuotient(const BCDInteger& diviser)const 
		{
			//Split Diviser         
		
			char  DiviserDigit = diviser.Digits[diviser.Length - 1];
			char* FlagDigits = diviser.Digits;
			const size_t FlagDigitCount = diviser.Length - 1;

			//Split Dividend
			char* RemainderDigits = this->Digits;
			const size_t RemainderDigitcount = FlagDigitCount;

			char* QuotientDigits = this->Digits + RemainderDigitcount;
			const size_t QuotientDigitCount = this->Length - RemainderDigitcount;

			//Quotient
			BCDInteger *Q = new BCDInteger(QuotientDigitCount);
			Q->Length = Q->Capacity;

			//Indices
			size_t i = QuotientDigitCount - 1,
				   j = QuotientDigitCount - 1;


			//Effective Dividend                   
			long ED = QuotientDigits[i], q = 0, r = 0, T1 = 0, T2 = 0;

			if (ED < DiviserDigit)
			{
				ED = (ED * 10) + QuotientDigits[--i];
				--j;
				--(Q->Length);				
			}

			while ( i != NPOS)
			{
				if (ED >= 0)
				{
					q = ED/ DiviserDigit;
					r = ED%DiviserDigit;

					--i;

					Q->Digits[j--] = q;
				}
				else
				{
					--(Q->Digits[j]);
					r += DiviserDigit;
				}

				//Cross Multiply Q & FlagDigits
				T1 = 0;
				size_t MinCount = FlagDigitCount;
				if (MinCount > (Q->Length - j - 1))
					MinCount = (Q->Length - j - 1);

				for (size_t k = j+1, l = MinCount - 1; l != NPOS; k++ ,l--)
				{
					T1 += (Q->Digits[k] * FlagDigits[l]);
				}

				T2 = r * 10 + QuotientDigits[i];
				ED = T2 - T1;

			}
			
			//Set Proper Sign
			if ((this->Positive && diviser.Positive) || (!this->Positive && !diviser.Positive))
				Q->Positive = true;
			else
				Q->Positive = false;


			return Q;
		}
		
		BCDInteger* NewTraditionalQuotient(const BCDInteger& diviser)const
		{
			BCDInteger *Q;
			size_t i = this->Length - diviser.Length, k = 0;
		
			
			char* TempStore, *ED, *MultiResult;
			size_t EDLen = 0, MultiResultLen = 0;

			TempStore = new char[this->Length];
			for (k = 0; k < this->Length; k++)
				TempStore[k] = this->Digits[k];

			ED = TempStore + i;
			EDLen = diviser.Length;
			MultiResult = new char[diviser.Length + 1];

			Q = new BCDInteger(i + 1);
			Q->Length = Q->Capacity;
			
			size_t j = Q->Capacity - 1;

			char Guess = 0;
			bool CreateNewGuess = true;

			while (i != NPOS)
			{
				//Guess
				if (CreateNewGuess)
				{
					char T = ED[diviser.Length - 1];
					if (EDLen > diviser.Length)
						T += 10 * ED[diviser.Length];

					Guess = T / diviser.Digits[diviser.Length - 1];
					Guess = Guess % 10;
				}
				else
					Guess--;

				//cout << "GuessComplete";

				char Carry, T;

				//Multiplication
				Carry = 0;
				for (k = 0; k < diviser.Length; k++)
				{
					T = diviser.Digits[k] * Guess + Carry;
					Carry = T / 10;
					MultiResult[k] = T % 10;
				}
				MultiResult[k] = Carry;
				if (Carry > 0)
					MultiResultLen = diviser.Length + 1;
				else
					MultiResultLen = diviser.Length;
				
				//cout << "MultiComplete";
				//Ensure Mutiplication result is less then ED
				
				if (EDLen < MultiResultLen)
				{
					CreateNewGuess = false;
					continue;
				}
				bool Lesser = false;
				for (size_t k = 0 ,l = EDLen-1 ; k < EDLen; k++, --l)
				{
					if (ED[l] < MultiResult[l])
					{
						Lesser = true;
						break;
					}
					else
						if (ED[l] > MultiResult[l])
							break;
				}
				if (Lesser)
				{
					CreateNewGuess = false;
					continue;
				}


				//Subtraction
				Carry = 1;
				for (k = 0; k < EDLen; k++)
				{
					T = ED[k] + (9 - MultiResult[k]) + Carry;
					Carry = T / 10;
					ED[k] = T % 10;
				}
				
				if (ED[EDLen - 1] != 0)
					++EDLen;

				Q->Digits[j--] = Guess;
				CreateNewGuess = true;
				ED--;		
				i--;

			}



			//Set Proper Sign
			if ((this->Positive && diviser.Positive) || (!this->Positive && !diviser.Positive))
				Q->Positive = true;
			else
				Q->Positive = false;

			delete[] TempStore;

			return Q;
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

				if ((ch == '+' || ch == '-')  && (valueString.length() == 1))
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
