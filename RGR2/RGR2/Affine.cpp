#include "Affine.h"

//Находит наибольший общий делитель
int Gcd(int firstNum, int secondNum) {
    while (secondNum != 0) {
        int remains = firstNum % secondNum;
        firstNum = secondNum;
        secondNum = remains;
    }
    return firstNum;
}

//Расширенный алгоритм Евклида
int ExtendedEuclidean(int firstNum, int secondNum) {

    vector<int> firstStr = { firstNum, 1, 0 };
    vector<int> secondStr = { secondNum, 0, 1 };
    int quotient = 0;

    while (secondStr[0] != 0) {
        quotient = firstStr[0] / secondStr[0];
        vector<int> thirdStr = { firstStr[0] % secondStr[0], firstStr[1] - quotient * secondStr[1], firstStr[2] - quotient * secondStr[2] };

        firstStr = secondStr;
        secondStr = thirdStr;
    }
    return firstStr[1];
}


//Находит обратный элемент
int modInverse(int a, int m) {
    int x = ExtendedEuclidean(a, m); 

    // ExtendedEuclidean может вернуть отрицательное число, а нам нужен остаток в диапазоне [0, m-1]. Эта формула это гарантирует.
    return (x % m + m) % m;
}

//Шифрует текст с использованием аффинного шифра.
vector<uint8_t> encrypt(string text, int a, int b) {
    vector<uint8_t> cyphertext;
    const int m = 256;
    vector<uint8_t> plaintext(text.begin(), text.end());

    for (int c : plaintext) {
        int encrypted_char = (a * c + b) % m;
        cyphertext.push_back(encrypted_char);
    }

    return cyphertext;
}

//Дешифрует текст, зашифрованный аффинным шифром.
string decrypt(vector<uint8_t> cipher, int a, int b) {
    string result = "";
    const int m = 256;

    int a_inv = modInverse(a, m);

    for (int c : cipher) {
        char decrypted_char = a_inv * (c - b);
        result += decrypted_char;
    }

    return result;
}
