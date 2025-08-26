/*#include "ExtendedEuclidean.h"
#include "ModuloComparison.h"
#include "Shamir.h"
#include "Fraction.h"
#include "Attack.h"

// Перегрузка оператора для красивого вывода вектора uint8_t (как строку)
ostream& operator<<(ostream& os, const vector<uint8_t>& vec) {
	for (uint8_t element : vec) {
		os << static_cast<char>(element);
	}
	return os;
}

template <typename T>
ostream& operator<<(ostream& os, vector<T> vec) {
	for (T element : vec) {
		os << element << " ";
	}
	os << "\n";
	return os;
}

*/