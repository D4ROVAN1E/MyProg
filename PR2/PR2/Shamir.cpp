#include "Shamir.h"

uint64_t modular_multiply(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t result = 0;
    a %= mod;

    while (b > 0) {
        // Если текущий бит b равен 1, добавляем a к результату
        if (b % 2 == 1) {
            result = (result + a) % mod;
        }

        // Удваиваем a (с взятием по модулю)
        a = (a * 2) % mod;

        // Переходим к следующему биту b
        b /= 2;
    }

    return result;
}

uint64_t power(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base %= mod;

    while (exp > 0) {
        // Если текущий бит степени равен 1
        if (exp % 2 == 1) {
            // Используем безопасное модульное умножение
            result = modular_multiply(result, base, mod);
        }

        // Возводим основание в квадрат
        // Используем безопасное модульное умножение
        base = modular_multiply(base, base, mod);

        // Переходим к следующему биту
        exp /= 2;
    }

    return result;
}

// Принимает вектор чисел, ключ и модуль, и шифрует каждое число в векторе.
vector<uint32_t> textEncrypt(vector<uint32_t> cyphertext, uint32_t key, uint32_t p) {
    vector<uint32_t> newText;
    for (uint32_t symb : cyphertext) {
        uint32_t x1 = power(symb, key, p);
        newText.push_back(x1);
    }
    return newText;
}

// Функция для шифрования сообщения по схеме Шамира.
vector<uint32_t> encrypt(const uint32_t cA, const uint32_t cB, const uint32_t p, const vector<uint8_t>& message) {

    if (Gcd(cA, p) != 1 || Gcd(cB, p) != 1)
        throw "Все ключи должны быть взаимнопростыми с p\n";
    else if (p <= 265 || p >= 4294967296LL)
        throw "Простое число должно быть больше 256 и меньше чем 2^32";

    vector<uint32_t> cyphertext;
    for (uint8_t s : message) {
        cyphertext.push_back(s);
    }

    cyphertext = textEncrypt(cyphertext, cA, p);
    cyphertext = textEncrypt(cyphertext, cB, p);

    return cyphertext;
}

// Функция для расшифровки сообщения по схеме Шамира.
vector<uint8_t> decrypt(const uint32_t cA, const uint32_t cB, const uint32_t p, vector<uint32_t> cyphertext) {

    if (Gcd(cA, p) != 1 || Gcd(cB, p) != 1)
        throw "Все ключи должны быть взаимнопростыми с p\n";
    else if (p <= 265 || p >= 4294967296LL)
        throw "Простое число должно быть больше 256 и меньше чем 2^32";


    // Абонент A вычисляет секретное число DA, такое что (CA * DA) mod (P-1) = 1
    uint32_t dA = ExtendedEuclidean(cA, p - 1, 0) + (p - 1);

    // Абонент B вычисляет секретное число DB, такое что (CB * DB) mod (P-1) = 1
    uint32_t dB = ExtendedEuclidean(cB, p - 1, 0) + (p - 1);

    cyphertext = textEncrypt(cyphertext, dA, p);
    cyphertext = textEncrypt(cyphertext, dB, p);

    vector<uint8_t> message;
    for (uint32_t s : cyphertext) {
        message.push_back(s);
    }

    return message;
}

