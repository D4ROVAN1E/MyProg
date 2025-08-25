#include "ModuloComparison.h"

void CheckPositive(const int64_t& inputNum) {
    if (inputNum < 0) {
        throw "Введённое число должно быть положительным \n";
    }
    else if (cin.fail()) {
        throw "Не удалось распознать число \n";
    }
}

void Input(uint32_t& firstBase, uint32_t& secondBase, uint32_t& primeNum, uint64_t& firstDegree, uint64_t& secondDegree) {

    int64_t testInputFirst;
    int64_t testInputSecond;

    cout << "Введите первое число и его степень\n";
    cin >> testInputFirst >> testInputSecond;
    CheckPositive(testInputFirst);
    CheckPositive(testInputSecond);

    firstBase = static_cast<uint32_t>(testInputFirst);
    firstDegree = testInputSecond;

    cout << "Введите второе число и его степень\n";
    cin >> testInputFirst >> testInputSecond;
    CheckPositive(testInputFirst);
    CheckPositive(testInputSecond);

    secondBase = static_cast<uint32_t>(testInputFirst);
    secondDegree = testInputSecond;

    cout << "Введите модуль (число должно быть простым >0) ";
    cin >> testInputFirst;
    CheckPositive(testInputFirst);

    primeNum = static_cast<uint32_t>(testInputFirst);
}

bool WheelTest(const uint32_t& primeNum) {
    if (primeNum == 2 || primeNum == 3 || primeNum == 5)
        return true;
    if (primeNum % 2 == 0 || primeNum % 3 == 0 || primeNum % 5 == 0) {
        cout << "Число не простое, оно делится на 2, 3 или 5\n";
        return false;
    }

    uint32_t squareOfNum = sqrt(primeNum);
    uint32_t divider = 7, countOfInc = 0;
    vector<uint32_t> divIncr = { 4, 2, 4, 2, 4, 6, 2, 6 };

    while (divider <= squareOfNum) {
        if (primeNum % divider == 0) {
            cout << "Число делится на " << divider << endl;
            return false;
        }
        if (countOfInc > 7)
            countOfInc = 0;
        divider += divIncr[countOfInc++];
    }
    return true;
}

uint32_t Ferm(const uint32_t& base, uint64_t degree, const uint32_t& primeNum) {
    degree %= primeNum - 1;
    uint64_t result = 1;
    uint64_t base64 = base % primeNum;

    for (uint64_t i = 0; i < degree; ++i) {
        result = (result * base64) % primeNum;
    }

    return static_cast<uint32_t>(result);
}

uint32_t Gcd(uint32_t firstNum, uint32_t secondNum) {
    while (secondNum != 0) {
        uint32_t remains = firstNum % secondNum;
        firstNum = secondNum;
        secondNum = remains;
    }
    return firstNum;
}

bool TheoremConditions(uint32_t base, uint32_t primeNum) {
    return Gcd(base, primeNum) == 1 && Ferm(base, primeNum - 1, primeNum) == 1;
}

vector<uint64_t> SquaresOfBase(const uint32_t& base, const uint32_t& powersOfTwo, const uint32_t& primeNum) {
    vector<uint64_t> baseSquares;
    baseSquares.push_back(static_cast<uint64_t>(base) % primeNum);

    for (uint32_t i = 1; i <= powersOfTwo; ++i) {
        baseSquares.push_back((baseSquares[i - 1] * baseSquares[i - 1]) % primeNum);
    }

    return baseSquares;
}

vector<uint8_t> DegreeToBin(uint64_t degree) {
    vector<uint8_t> binaryDegree;

    while (degree > 0) {
        binaryDegree.push_back(degree % 2);
        degree /= 2;
    }

    return binaryDegree;
}

void CheckLog(uint32_t& powersOfTwo, const uint64_t& degree) {
    uint64_t fullDegree = pow(2, powersOfTwo);

    if (fullDegree > degree)
        powersOfTwo--;
}

uint32_t DecompOfDegree(uint32_t base, uint64_t degree, uint32_t primeNum) {
    uint32_t powersOfTwo = floor(log2(degree));
    CheckLog(powersOfTwo, degree);

    vector<uint64_t> baseSquares = SquaresOfBase(base, powersOfTwo, primeNum);
    vector<uint8_t> binaryDegree = DegreeToBin(degree);
    uint64_t result = 1;

    for (uint64_t i = 0; i <= powersOfTwo; ++i) {
        if (binaryDegree[i] == 1) {
            result = (result * baseSquares[i]) % primeNum;
        }
    }
    return static_cast<uint32_t>(result);
}