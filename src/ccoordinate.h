#ifndef _CCOORDINATE_
#define _CCOORDINATE_

#include <iostream>

typedef int Value;

using namespace std;

class CCoordinate
{
		Value X;
		Value Y;
		Value Z;

	public :
		CCoordinate();

		void SetX(Value x);
		void SetY(Value y);
		void SetZ(Value z);

		void Set(Value x, Value y);
		void Set(Value x, Value y, Value z);

		Value GetX();
		Value GetY();
		Value GetZ();

		float DistanceWith(Value x, Value y, Value z);
		float DistanceWith(CCoordinate coo);

		void Display(ostream& os) const;
		friend ostream& operator<<(ostream& os, const CCoordinate& coo);

};

#endif
