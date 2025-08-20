#include <iostream>
#include <vector>
#include <random>
#include <cstdint>
#include <algorithm>
#include <map>
#include <cmath>

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
uint32_t mod_pow(uint32_t base, uint32_t exponent, uint32_t modulus) {
    if (modulus == 1)
        return 0; // всё по модулю 1 равно 0

    base %= modulus;
    uint32_t result = 1;

    while (exponent > 0) {
        if (exponent & 1) {
            result = (uint64_t)result * base % modulus;
        }
        base = (uint64_t)base * base % modulus;
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

// Тест Поклингтона для проверки простоты
// n - число для проверки, F - частичное разложение n-1, t - параметр надежности
bool pocklington_test(uint32_t n, uint32_t F, uint32_t t = 10) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    // Получаем факторизацию F
    vector<pair<uint32_t, uint32_t>> F_factors = factorize(F);

    // Проверяем, что F > sqrt(n) - 1
    uint32_t sqrt_n = (uint32_t)sqrt(n);
    if (F <= sqrt_n - 1) {
        return false; // Условие теоремы Поклингтона не выполнено
    }

    // Генератор случайных чисел
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint32_t> dis(2, n - 1);

    // Выполняем t проверок
    // Выполняем t проверок
    vector<uint32_t> randNum;
    for (uint32_t test = 0; test < t; ++test) {
        uint32_t a = dis(gen);
        randNum.push_back(a);
    }
    for (auto a : randNum) {
        // Шаг 2: Проверяем a^(n-1) mod n = 1 (малая теорема Ферма)
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
uint32_t generate_prime_pocklington(uint32_t bits, uint32_t t = 10) {
    if (bits < 4) return 0;

    random_device rd;
    mt19937 gen(rd());

    uint32_t max_R = (1 << bits / 2) - 1;
    uint32_t min_R = 1 << (bits / 2 - 1);
    uint32_t max_F = (1 << bits / 2 + 1) - 1;
    uint32_t min_F = 1 << (bits / 2);
    uint32_t F = 1, n = 1;
    while (!pocklington_test(n, F, t)) {
        // Шаг 2: Строим F
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

double count_true(int num) {
    double tr = 0.0;
    for (int i = 0; i < 50; ++i) {
        tr += pocklington_test(num, 15, 10);
    }
    return tr;
}

int main() {
    setlocale(LC_ALL, "ru");
    sieve_of_eratosthenes();
    
    /*cout << "=== ПРОГРАММА ГЕНЕРАЦИИ И ПРОВЕРКИ ПРОСТЫХ ЧИСЕЛ МЕТОДОМ ПОКЛИНГТОНА ===" << endl;

    // Инициализируем решето
    cout << "\n1. Построение решета Эратосфена..." << endl;
    sieve_of_eratosthenes();
    cout << "Найдено простых чисел: " << primes.size() << endl;

    // Тестируем известные числа
    cout << "\n2. Тестирование известных чисел..." << endl;

    // Пример из документации: n = 4021
    uint64_t test_n = 4021;
    uint64_t test_F = 67;
    vector<pair<uint32_t, uint32_t>> test_factors = { {67, 1} };

    cout << "Тестируем число " << test_n << " с F = " << test_F << endl;
    bool result = pocklington_test(test_n, test_F, test_factors, 5);
    cout << "Результат: " << (result ? "ПРОСТОЕ" : "СОСТАВНОЕ") << endl;

    // Генерируем простые числа разной разрядности
    cout << "\n3. Генерация простых чисел..." << endl;

    for (uint32_t bits = 10; bits <= 20; bits += 2) {
        cout << "\nГенерируем " << bits << "-битное простое число..." << endl;
        uint64_t generated = generate_prime_pocklington(bits, 10);

        if (generated > 0) {
            cout << "Сгенерировано число: " << generated << endl;

            // Дополнительная проверка
            uint64_t F_check = generate_F(bits / 2 + 1);
            if (F_check > 0 && (generated - 1) % F_check == 0) {
                vector<pair<uint32_t, uint32_t>> factors_check = factorize(F_check);
                bool final_check = pocklington_test(generated, F_check, factors_check, 20);
                cout << "Финальная проверка: " << (final_check ? "ПОДТВЕРЖДЕНО" : "НЕ ПОДТВЕРЖДЕНО") << endl;
            }
        }
        else {
            cout << "Не удалось сгенерировать число за отведенное время" << endl;
        }
    }

    // Интерактивный режим
    cout << "\n4. Интерактивный режим проверки" << endl;
    cout << "Введите число для проверки (0 для выхода): ";

    uint64_t user_input;
    while (cin >> user_input && user_input != 0) {
        if (user_input < 2) {
            cout << "Число должно быть больше 1" << endl;
        }
        else {
            // Пытаемся найти подходящее F
            uint64_t best_F = 0;
            vector<pair<uint32_t, uint32_t>> best_factors;

            // Пробуем разные значения F
            for (uint64_t F = 2; F * F < user_input && F < 1000; ++F) {
                if ((user_input - 1) % F == 0) {
                    vector<pair<uint32_t, uint32_t>> factors = factorize(F);
                    if (!factors.empty()) {
                        uint64_t sqrt_n = (uint64_t)sqrt(user_input);
                        if (F > sqrt_n - 1) {
                            best_F = F;
                            best_factors = factors;
                            break;
                        }
                    }
                }
            }

            if (best_F > 0) {
                bool is_prime = pocklington_test(user_input, best_F, best_factors, 10);
                cout << "Число " << user_input << " с F = " << best_F << ": "
                    << (is_prime ? "ПРОСТОЕ" : "СОСТАВНОЕ") << endl;
            }
            else {
                cout << "Не удалось найти подходящее разложение для теста Поклингтона" << endl;
            }
        }

        cout << "Введите число для проверки (0 для выхода): ";
    }

    cout << "\n=== ПРОГРАММА ЗАВЕРШЕНА ===" << endl;*/
    return 0;
}