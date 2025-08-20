#include <iostream>
#include <vector>
#include <random>
#include <cstdint>
#include <algorithm>
#include <map>

using namespace std;

// Глобальные переменные для хранения простых чисел и их статуса
vector<uint32_t> primes;
map<uint32_t, bool> sieve_results; // true - простое, false - составное

// Решето Эратосфена для чисел меньше 500
void sieve_of_eratosthenes(uint32_t limit = 500) {
    vector<bool> is_prime(limit, true);
    is_prime[0] = is_prime[1] = false;

    for (uint32_t i = 2; i * i < limit; ++i) {
        if (is_prime[i]) {
            for (uint32_t j = i * i; j < limit; j += i) {
                is_prime[j] = false;
            }
        }
    }

    // Сохраняем результаты
    primes.clear();
    sieve_results.clear();

    for (uint32_t i = 2; i < limit; ++i) {
        sieve_results[i] = is_prime[i];
        if (is_prime[i]) {
            primes.push_back(i);
        }
    }
}

// Быстрое возведение в степень
int32_t mod_pow(int32_t base, int32_t exponent, int32_t modulus) {
    if (modulus == 1)
        return 0; // всё по модулю 1 равно 0

    base %= modulus;
    int32_t result = 1;

    while (exponent > 0) {
        if (exponent & 1) {
            result = (int64_t)result * base % modulus;
        }
        base = (int64_t)base * base % modulus;
        exponent >>= 1;
    }

    return result;
}

// Факторизация числа на простые множители
vector<pair<uint32_t, uint32_t>> factorize(uint32_t n) {
    vector<pair<uint32_t, uint32_t>> factors;

    for (uint32_t p : primes) {
        if (static_cast<uint32_t>(p) * p > n) break;

        uint32_t count = 0;
        while (n % p == 0) {
            n /= p;
            count++;
        }

        if (count > 0) {
            factors.push_back({ p, count });
        }
    }

    if (n > 1) {
        factors.push_back({ static_cast<uint32_t>(n), 1 });
    }

    return factors;
}

// Тест Миллера для проверки простоты
bool miller_test(uint32_t n, uint32_t t) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    // Факторизуем n-1
    auto factors = factorize(n - 1);

    // Генератор случайных чисел
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint32_t> dis(2, n - 1);

    // Выполняем t проверок
    vector<uint32_t> randNum;
    for (uint32_t test = 0; test < t; ++test) {
        uint32_t a = dis(gen);
        randNum.push_back(a);
    }
    for (auto a : randNum) {
        // Проверка малой теоремы Ферма
        uint32_t fermat_result = mod_pow(a, n - 1, n);
        if (fermat_result != 1) {
            return false;
        }
    }

    // Дополнительные проверки для каждого простого множителя

    for (const auto& factor : factors) {
        bool all_equal_one = true;
        for (auto a : randNum) {
            uint32_t exp = (n - 1) / factor.first;
            uint32_t result = mod_pow(a, exp, n);

            if (result != 1) {
                all_equal_one = false;
                break;
            }
        }
        if (all_equal_one) {
            return false;
        }
    }

    return true;
}

uint32_t miller_generate_prime(uint32_t k, const uint32_t& t) {
    uint32_t n_min = 1 << (k - 2), n_max = (1 << (k - 1)) - 1;
    random_device rd;
    mt19937 gen(rd());
    uint32_t n = 0, m = 1;
    while (!miller_test(n, t)) {
        m = 1;
        while (!(n_min < m && m < n_max) && (m > 0)) {
            int32_t primeIt = gen() % primes.size();
            uint32_t primeExp = gen() % 3;
            m *= pow(primes[primeIt], primeExp);
            while (m >= n_max) {
                m /= pow(primes[primeIt], primeExp);
                if (primeIt - 1 < 0) {
                    break;
                }
                m *= pow(primes[--primeIt], primeExp);
            }
        }
        n = 2 * m + 1;
    }
    return n;
}

int main() {
    setlocale(LC_ALL, "ru");
    sieve_of_eratosthenes();
    cout << miller_primary(5, 10);
    return 0;
}