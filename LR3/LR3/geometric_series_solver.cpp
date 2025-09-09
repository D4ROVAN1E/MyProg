#include <iostream>
#include <cstdint>
#include <vector>

using namespace std;

//наибольший общий делитель двух чисел
uint64_t gcd(uint64_t a, uint64_t b) {
    while (b) {
        a %= b;
        swap(a, b);
    }
    return a;
}

//Вычисляет степень числа
uint64_t power(uint64_t base, int exp) {
    uint64_t res = 1;
    while (exp > 0) {
        if (exp % 2 == 1) res *= base;
        base *= base;
        exp /= 2;
    }
    return res;
}


int main() {
    int a;
    uint64_t b;
    cin >> a >> b;

    // Согласно условию, ряд расходится, если b=1 
    if (b == 1) {
        cout << "infinity" << endl;
        return 0;
    }

    // При целых a > 0 и b > 1 сумма всегда является рациональным числом.
    // Коэффициенты C[k][i] - это числа Эйлера A(k, i-1).
    // C[k][i] - это коэффициент при x^i в полиноме P_k(x).
    uint64_t C[11][11] = { 0 };

    // Базовый случай для рекурренсии: a=1.
    // Сумма S_1 = sum(n * x^n) = x / (1-x)^2. Полином P_1(x) = x.
    C[1][1] = 1;

    // Динамически вычисляем коэффициенты полиномов Эйлера до P_a(x)
    for (int k = 2; k <= a; ++k) {
        for (int i = 1; i <= k; ++i) {
            // Рекуррентное соотношение для коэффициентов:
            // C[k][i] = i * C[k-1][i] + (k - i + 1) * C[k-1][i-1]
            C[k][i] = i * C[k - 1][i] + (uint64_t)(k - i + 1) * C[k - 1][i - 1];
        }
    }

    // Вычисляем итоговую дробь по формуле S = (b * P_a(1/b)) / (b-1)^(a+1)
    // Числитель дроби P_a(1/b) * b^a, чтобы избавиться от степеней 1/b
    uint64_t numerator_poly_val = 0;
    for (int i = 1; i <= a; ++i) {
        numerator_poly_val += C[a][i] * power(b, a - i);
    }

    // Полный числитель итоговой дроби
    uint64_t final_numerator = b * numerator_poly_val;

    // Знаменатель итоговой дроби
    uint64_t final_denominator = power(b - 1, a + 1);

    // Сокращаем дробь, находя НОД
    uint64_t common_divisor = gcd(final_numerator, final_denominator);

    // Выводим несократимую дробь 
    cout << final_numerator / common_divisor << "/" << final_denominator / common_divisor << endl;

    return 0;
}