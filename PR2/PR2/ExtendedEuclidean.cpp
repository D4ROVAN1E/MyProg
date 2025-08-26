#include "ExtendedEuclidean.h"

int32_t ExtendedEuclidean(int32_t firstNum, int32_t secondNum, bool output) {

	if (Gcd(firstNum, secondNum) != 1)
		throw "Вводимые числа не взаимно простые \n";

	vector<int32_t> firstStr = { firstNum, 1, 0 };
	vector<int32_t> secondStr = { secondNum, 0, 1 };
	int32_t quotient = 0;

	if (output) {
		cout << "r-------x-------y--------------------\n";
		cout << firstStr[0] << "\t" << firstStr[1] << "\t" << firstStr[2] << endl;
		cout << secondStr[0] << "\t" << secondStr[1] << "\t" << secondStr[2] << endl;
	}

	while (secondStr[0] != 0) {
		quotient = firstStr[0] / secondStr[0];
		vector<int32_t> thirdStr = { firstStr[0] % secondStr[0], firstStr[1] - quotient * secondStr[1], firstStr[2] - quotient * secondStr[2] };

		if (output)
			cout << thirdStr[0] << "\t" << thirdStr[1] << "\t" << thirdStr[2] << "\t" << "quotient = " << quotient << endl;

		firstStr = secondStr;
		secondStr = thirdStr;
	}
	return firstStr[1];
}