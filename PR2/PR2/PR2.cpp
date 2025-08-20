#include <windows.h>
#include "ExtendedEuclidean.h";
#include "ModuloComparison.h";
#include "Shamir.h";

template <typename T>
ostream& operator<<(ostream& os, vector<T> vec) {
	for (T element : vec) {
		os << element << " ";
	}
	os << "\n";
	return os;
}


int main() {
	//SetConsoleOutputCP(1251);
	//SetConsoleCP(1251);
	setlocale(LC_ALL, "Russian");
	//uint64_t firstDegree, secondDegree;
	//uint32_t firstBase, secondBase, primeNum;
	try {
		string msg_str = "Test string!!###&*$#@";
		vector<uint8_t> msg(msg_str.begin(), msg_str.end());
		vector<uint32_t> cyphertext = encrypt(1049, 1109, 311, msg);
		cout << msg << cyphertext;
		msg = decrypt(1049, 1109, 311, cyphertext);
		cout << msg;
		/*Input(firstBase, secondBase, primeNum, firstDegree, secondDegree);
		cout << "--------------------- Вычисление по теореме Ферма ---------------------\n";
		if (!WheelTest(primeNum)) {
			cout << "Модуль не является простым\n";
			return 0;
		}
		else if (!TheoremConditions(firstBase, primeNum) || !TheoremConditions(secondBase, primeNum)) {
			cout << "Введённые условия не соответствуют условию теоремы Ферма\n";
			return 0;
		}
		uint32_t Result1 = Ferm(firstBase, firstDegree, primeNum);
		uint32_t Result2 = Ferm(secondBase, secondDegree, primeNum);
		cout << firstBase << " в степени " << firstDegree << " по модулю " << primeNum << " = " << Result1 << endl;
		cout << secondBase << " в степени " << secondDegree << " по модулю " << primeNum << " = " << Result2 << endl;
		if (Result1 == Result2)
			cout << "Числа равны\n";
		else
			cout << "Числа не равны\n";
		cout << "--------------------- Вычисление по разложению степени ---------------------\n";
		Result1 = DecompOfDegree(firstBase, firstDegree, primeNum);
		Result2 = DecompOfDegree(secondBase, secondDegree, primeNum);
		cout << firstBase << " в степени " << firstDegree << " по модулю " << primeNum << " = " << Result1 << endl;
		cout << secondBase << " в степени " << secondDegree << " по модулю " << primeNum << " = " << Result2 << endl;
		if (Result1 == Result2)
			cout << "Числа равны\n";
		else
			cout << "Числа не равны\n";
		int32_t firstNum, mod;
		cin >> firstNum >> mod;
		int32_t d = ExtendedEuclidean(firstNum, mod, mod);
		cout << "Обратный элемент d = " << d << endl;
		cout << firstNum << "^(-1) mod " << mod << " = " << mod + d << endl;
		cout << firstNum << " * " << mod + d << " mod " << mod << " = 1";*/
		return 0;
	}

	catch (const char* s) {
		cerr << s;
	}
}