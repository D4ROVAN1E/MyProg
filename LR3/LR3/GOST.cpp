#include <iostream>
#include <vector>
#include <random>
#include <cstdint>
#include <map>
#include <cmath>
#include <iomanip>

using namespace std;

// Глобальные переменные для хранения простых чисел
vector<uint32_t> primes;
map<uint32_t, bool> sieve_results;

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

    primes.clear();
    sieve_results.clear();

    for (uint32_t i = 2; i < limit; ++i) {
        sieve_results[i] = is_prime[i];
        if (is_prime[i]) {
            primes.push_back(i);
        }
    }
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

// Быстрое возведение в степень по модулю
uint64_t mod_pow(uint64_t base, uint64_t exponent, uint64_t modulus) {
    if (modulus == 1) return 0;

    uint64_t result = 1;
    base %= modulus;

    while (exponent > 0) {
        if (exponent & 1) {
            result = (result * base) % modulus;
        }
        base = (base * base) % modulus;
        exponent >>= 1;
    }

    return result;
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

// Тест Поклингтона для проверки простоты
// n - число для проверки, F - частичное разложение n-1, t - параметр надежности
bool pocklington_test(uint32_t n, uint32_t F, uint32_t t = 10) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    // Получаем факторизацию F
    vector<pair<uint32_t, uint32_t>> F_factors = factorize(F);

    // Проверяем, что F > sqrt(n) - 1
    /*uint32_t sqrt_n = (uint32_t)sqrt(n);
    if (F <= sqrt_n - 1) {
        return false; // Условие теоремы Поклингтона не выполнено
    }*/

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
        // Проверяем малую теорему Ферма
        if (mod_pow(a, n - 1, n) != 1) {
            return false; // n - составное
        }
    }

    // Проверяем, что для всех q результат не равен 1
    for (auto a : randNum) {
        bool all_not_one = true;
        for (const auto& factor : F_factors) {

            uint32_t exp = (n - 1) / factor.first;
            uint32_t result = mod_pow(a, exp, n);

            if (result == 1) {
                all_not_one = false;
                break;
            }

        }
        if (all_not_one) {
            return true;
        }
    }

    return false;
}

// Генерация простого числа методом Поклингтона
uint32_t pocklington_generate_prime(uint32_t bits, uint32_t t = 10) {
    if (bits < 4) return 0;

    random_device rd;
    mt19937 gen(rd());

    uint32_t max_R = (1 << bits / 2) - 1;
    uint32_t min_R = 1 << (bits / 2 - 1);
    uint32_t max_F = (1 << bits / 2 + 1) - 1;
    uint32_t min_F = 1 << (bits / 2);
    uint32_t F = 1, n = 1;
    while (!pocklington_test(n, F, t)) {
        //Строим F
        F = 1;
        while (!(min_F <= F && F < max_F) && (F > 0)) {
            int primeIt = gen() % primes.size();
            int primeExp = gen() % 3;
            F *= pow(primes[primeIt], primeExp);
            while (F >= max_F) {
                F /= pow(primes[primeIt], primeExp);
                if (primeIt - 1 < 0) break;
                F *= pow(primes[--primeIt], primeExp);
            }
        }

        uniform_int_distribution<uint32_t> R_dis(min_R / 2, max_R / 2);
        uint32_t R = R_dis(gen) * 2; // Делаем четным

        n = R * F + 1;

    }

    return n;
}

// Линейный конгруэнтный генератор для получения случайных чисел в (0,1)
double linear_congruential_generator() {
    random_device rd;
    mt19937 gen(rd());
    static uint64_t seed = gen();
    const uint64_t a = 1664525; //a = 1 mod 4
    const uint64_t c = 1013904223; // gcd(c,m) = 1
    const uint64_t m = 4294967296LL; // 2^32

    seed = (a * seed + c) % m;
    return (double)seed / m;
}

// Генерация простого числа по алгоритму ГОСТ Р 34.10-94
uint64_t gost_generate_prime(uint32_t t, uint64_t q) {

    while (true) {
        bool getNum = true;

        // Вычисляем N
        double xi = linear_congruential_generator();

        // N = floor(2^(t-1) / q) + floor(xi * 2^(t-1) / q)
        uint64_t power_t_minus_1 = 1ULL << (t - 1);
        uint64_t N = power_t_minus_1 / q + (uint64_t)(xi * power_t_minus_1 / q);

        // Если N нечетное, то N = N + 1
        if (N % 2 == 1) {
            N = N + 1;
        }

        uint64_t u = 0, p = 1;

        while (true) {
            // Вычисляем p = (N + u) * q + 1
            p = (N + u) * q + 1;

            // Проверяем, что p <= 2^t
            uint64_t power_t = 1ULL << t;
            if (p > power_t) {
                getNum = false;
                break;
            }
            if ((mod_pow(2, p - 1, p) == 1 && mod_pow(2, N + u, p) != 1)) {
                break;
            }

            u = u + 2;
        }

        if(getNum) return p;
    }
}

// Генерация простого числа заданной битовой длины
uint64_t generate_prime_of_size(uint32_t target_bits) {
    if (target_bits <= 2) {
        return 3; // Минимальное простое число
    }
    uint32_t minPrimalTarget = target_bits;
    while (minPrimalTarget > 9) {
        minPrimalTarget = ceil((double)minPrimalTarget / 2);
    }
    // Начинаем с малого простого числа
    uint64_t current_prime = 3;
    uint32_t current_bits = 0, minPrimeInd = 0;
    while (current_bits != minPrimalTarget) {
        current_prime = primes[minPrimeInd++];
        current_bits = ceil(log2(current_prime));
    }

    // Пошагово увеличиваем размер простого числа
    while (current_bits < target_bits) {
        uint32_t next_bits = current_bits * 2;
        uint64_t next_prime = gost_generate_prime(next_bits, current_prime);

        current_prime = next_prime;
        current_bits = next_bits;
    }

    return current_prime;
}

int main() {
    setlocale(LC_ALL, "ru");
    sieve_of_eratosthenes();
    int choice;
    bool run = true;
    while (run) {
        cout << "=== Выберите процедуру генерации простых чисел ===\n1) Миллер\n2) Поклингтон\n3) ГОСТ\n4) Выход\n";
        cin >> choice;
        switch (choice)
        {
        case 1: {
            uint32_t tBits, k = 0;
            cout << "Введите размер простого числа в битах (5 - 32)\n";
            cin >> tBits;
            if (tBits > 32 || tBits < 5) {
                cout << "Вы ввели некорректное кол-во бит\n";
                break;
            }
            //Проверка отвергнутых решетом Эратосфена
            for (auto num : sieve_results) {
                if (num.second == false) {
                    k += miller_test(num.first, 20);
                }
            }
            cout << "N" << setw(10) << "P" << setw(40) << "Проверка вероятностным тестом" << setw(6) << "k\n";
            for (int i = 1; i <= 10; i++) {
                uint32_t num = miller_generate_prime(tBits, 20);
                char testRes;
                if (miller_test(num, 20)) {
                    testRes = '+';
                }
                else {
                    testRes = '-';
                }
                cout << i << setw(15) << num << setw(20) << testRes << setw(20) << k << endl;
            }
            break;
        }
        case 2: {
            uint32_t tBits, k = 0;
            cout << "Введите размер простого числа в битах (5 - 32)\n";
            cin >> tBits;
            if (tBits > 32 || tBits < 5) {
                cout << "Вы ввели некорректное кол-во бит\n";
                break;
            }
            //Проверка отвергнутых решетом Эратосфена
            for (auto num : sieve_results) {
                if (num.second == false) {
                    auto factor = factorize(num.first);
                    k += pocklington_test(num.first, (factor[factor.size() - 1].first - 1), 20);
                }
            }
            cout << "N" << setw(10) << "P" << setw(40) << "Проверка вероятностным тестом" << setw(6) << "k\n";
            for (int i = 1; i <= 10; i++) {
                uint32_t num = pocklington_generate_prime(tBits, 20);
                auto factor = factorize(num);
                char testRes;
                if (pocklington_test(num, (factor[factor.size() - 1].first - 1), 20)) {
                    testRes = '+';
                }
                else {
                    testRes = '-';
                }
                cout << i << setw(15) << num << setw(20) << testRes << setw(20) << k << endl;
            }
            break;
        }
        case 3: {
            uint32_t tBits, k = 0;
            cout << "Введите размер простого числа в битах (5 - 32)\n";
            cin >> tBits;
            if (tBits > 32 || tBits < 5) {
                cout << "Вы ввели некорректное кол-во бит\n";
                break;
            }
            //Проверка отвергнутых решетом Эратосфена
            for (auto num : sieve_results) {
                if (num.second == false) {
                    k += miller_test(num.first, 20);
                }
            }
            cout << "N" << setw(10) << "P" << setw(40) << "Проверка вероятностным тестом" << setw(6) << "k\n";
            for (int i = 1; i <= 10; i++) {
                uint32_t num = generate_prime_of_size(tBits);
                char testRes;
                if (miller_test(num, 20)) {
                    testRes = '+';
                }
                else {
                    testRes = '-';
                }
                cout << i << setw(15) << num << setw(20) << testRes << setw(20) << k << endl;
            }
            break;
        }
        case 4: {
            run = false;
            break;
        }
        default:
            cout << "Неверный ввод\n";
            break;
        }
    }
    
    return 0;
}