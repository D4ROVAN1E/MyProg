#include "Fraction.h";

//Раскладывает отношение a/b в цепную дробь.
vector<int> get_continued_fraction(int a, int b) {
    vector<int> coeffs;
    while (b > 0) {
        int quotient = a / b;
        int remainder = a % b;
        coeffs.push_back(quotient);
        a = b;
        b = remainder;
    }
    return coeffs;
}

//Вычисляет подходящие дроби P_k/Q_k
void calculate_convergents(const vector<int>& coeffs, vector<int>& P, vector<int>& Q) {
    // Начальные условия, как в примере
    int p_prev2 = 0, p_prev1 = 1;
    int q_prev2 = 1, q_prev1 = 0;

    for (size_t k = 0; k < coeffs.size(); ++k) {
        int ak = coeffs[k];
        int pk = ak * p_prev1 + p_prev2;
        int qk = ak * q_prev1 + q_prev2;

        P.push_back(pk);
        Q.push_back(qk);

        p_prev2 = p_prev1;
        p_prev1 = pk;
        q_prev2 = q_prev1;
        q_prev1 = qk;
    }
}

//Находит решение уравнения ax + by = c.
pair<int, int> solve_diophantine(int orig_a, int orig_b, int c) {

    // Проверка на разрешимость
    int gcd = Gcd(orig_a, orig_b);
    if (c % gcd != 0) {
        throw "Решений в целых числах не существует, так как c не делится на НОД .\n";
    }

    // Упрощение уравнения делением на НОД
    int a = orig_a / gcd;
    int b = orig_b / gcd;
    int c_prime = c / gcd;

    //Получаем коэффициенты цепной дроби
    vector<int> coeffs = get_continued_fraction(a, b);

    //Вычисляем подходящие дроби
    vector<int> P, Q;
    calculate_convergents(coeffs, P, Q);

    // Нам нужны предпоследние значения P_{k-1} и Q_{k-1}
    int p_penultimate = P[P.size() - 2];
    int q_penultimate = Q[P.size() - 2];

    // Используем фундаментальное свойство: a*Q_{k-1} - b*P_{k-1} = (-1)^{k-1}
    int k = coeffs.size() - 1;
    int sign = ((k - 1) % 2 == 0) ? 1 : -1; // (-1)^{k-1}    

    // Мы имеем решение для a*x + b*y = sign
    // x0 = q_penultimate, y0 = -p_penultimate

    // Умножаем x0 и y0 на c_prime и на знак, чтобы правая часть стала c_prime
    int multiplier = c_prime * (a * q_penultimate - b * p_penultimate);

    int a0 = q_penultimate * multiplier;
    int b0 = -p_penultimate * multiplier;
    return { a0, b0 };
}

/*
// Заданные коэффициенты уравнения
int a = 275;
int b = 145;
int c = 10;
*/