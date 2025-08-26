#include <iostream>
#include <vector>
#include <string>

#include "ExtendedEuclidean.h"
#include "ModuloComparison.h"
#include "Shamir.h"
#include "Fraction.h"
#include "Attack.h"

using namespace std;

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

void display_menu() {
    cout << "\nВыберите действие:\n";
    cout << "1) Сравнение двух чисел с возведением в степень по модулю\n";
    cout << "2) Расширенный алгоритм Евклида для чисел c и d\n";
    cout << "3) Нахождение обратного числа по модулю\n";
    cout << "4) Шифрование и дешифрование текста алгоритмом Шамира\n";
    cout << "5) Решение уравнения a*275 + b*145 = 10\n";
    cout << "6) Демонстрация атаки 'Встреча посередине' на шифр Шамира\n";
    cout << "0) Выход\n";
    cout << "Ваш выбор: ";
}

int main() {
    setlocale(LC_ALL, "Russian");
    int choice;

    while (true) {
        display_menu();
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); // Очищаем флаг ошибки
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Игнорируем оставшуюся часть строки
            cerr << "Ошибка: Введите число.\n";
            continue;
        }

        try {
            switch (choice) {
            case 1: {
                uint64_t firstDegree, secondDegree;
                uint32_t firstBase, secondBase, primeNum;

                Input(firstBase, secondBase, primeNum, firstDegree, secondDegree);

                cout << "\n--- Вычисление по теореме Ферма ---\n";
                if (!WheelTest(primeNum)) {
                    cout << "Модуль не является простым\n";
                    break;
                }
                if (!TheoremConditions(firstBase, primeNum) || !TheoremConditions(secondBase, primeNum)) {
                    cout << "Введённые данные не соответствуют условию теоремы Ферма\n";
                    break;
                }
                uint32_t Result1 = Ferm(firstBase, firstDegree, primeNum);
                uint32_t Result2 = Ferm(secondBase, secondDegree, primeNum);
                cout << firstBase << "^" << firstDegree << " mod " << primeNum << " = " << Result1 << endl;
                cout << secondBase << "^" << secondDegree << " mod " << primeNum << " = " << Result2 << endl;
                cout << "Результат: Числа " << (Result1 == Result2 ? "равны" : "не равны") << " по модулю.\n";

                cout << "\n--- Вычисление по разложению степени ---\n";
                Result1 = DecompOfDegree(firstBase, firstDegree, primeNum);
                Result2 = DecompOfDegree(secondBase, secondDegree, primeNum);
                cout << firstBase << "^" << firstDegree << " mod " << primeNum << " = " << Result1 << endl;
                cout << secondBase << "^" << secondDegree << " mod " << primeNum << " = " << Result2 << endl;
                cout << "Результат: Числа " << (Result1 == Result2 ? "равны" : "не равны") << " по модулю.\n";
                break;
            }
            case 2: {
                int32_t c, d;
                cout << "Введите два числа c и d для расширенного алгоритма Евклида: ";
                cin >> c >> d;
                if (cin.fail()) throw "Некорректный ввод.";

                cout << "Находим x и y для уравнения " << c << "x + " << d << "y = НОД(" << c << ", " << d << ")\n";
                int32_t x = ExtendedEuclidean(c, d, true); // true для вывода шагов
                cout << "Один из коэффициентов (x) = " << x << endl;
                break;
            }
            case 3: {
                int32_t num, mod;
                cout << "Введите число и модуль для нахождения обратного элемента: ";
                cin >> num >> mod;
                if (cin.fail()) throw "Некорректный ввод.";

                int32_t d_inv = ExtendedEuclidean(num, mod, true); // true для вывода шагов
                cout << "\nОбратный элемент d = " << d_inv << endl;
                cout << num << "^(-1) mod " << mod << " = " << (d_inv % mod + mod) % mod << endl;
                break;
            }
            case 4: {
                const uint32_t p = 4091;
                const uint32_t cA = 101;
                const uint32_t cB = 103;

                cout << "\n--- Легитимный протокол Шамира ---\n";
                string msg_str;
                cout << "Введите сообщение для шифрования: ";
                cin.ignore(); // Очистить буфер перед getline
                getline(cin, msg_str);

                vector<uint8_t> msg(msg_str.begin(), msg_str.end());
                cout << "Оригинальное сообщение: " << msg << endl;
                cout << "Параметры: p=" << p << ", cA=" << cA << ", cB=" << cB << endl;

                vector<uint32_t> cyphertext = encrypt(cA, cB, p, msg);
                cout << "Зашифрованное сообщение (шифротекст): " << cyphertext;

                vector<uint8_t> decrypted_msg = decrypt(cA, cB, p, cyphertext);
                cout << "Расшифрованное сообщение: " << decrypted_msg << endl;
                break;
            }
            case 5: {
                int x = 275, y = 145, c = 10;
                cout << "Решение уравнения a*275 + b*145 = 10\n";

                pair<int, int> answer = solve_diophantine(x, y, c);
                cout << "Одно из частных решений: x = " << answer.first << ", y = " << answer.second << endl;
                cout << "Проверка: " << x << "*" << answer.first << " + " << y << "*" << answer.second << " = " << x * answer.first + y * answer.second << endl;
                break;
            }
            case 6: {
                // --- Легитимный обмен ---
                // Используем корректные параметры: p - простое, ключи взаимно просты с p-1
                const uint32_t p = 4091;  // Простое число > 256
                const uint32_t cA = 101;  // Секретный ключ Алисы
                const uint32_t cB = 103;  // Секретный ключ Боба
                /*const uint32_t p = 1051117307;  // Простое число > 256
                const uint32_t cA = 1035792343;  // Секретный ключ Алисы
                const uint32_t cB = 1032668809;  // Секретный ключ Боба*/

                cout << "--- Легитимный протокол Шамира ---" << endl;
                string msg_str = "This is a secret message.";
                vector<uint8_t> msg(msg_str.begin(), msg_str.end());
                cout << "Оригинальное сообщение: " << msg << endl;
                cout << "Параметры: p=" << p << ", cA=" << cA << ", cB=" << cB << endl << endl;


                // Шифруем сообщение
                vector<uint32_t> cyphertext = encrypt(cA, cB, p, msg);
                cout << "Зашифрованное сообщение (шифротекст): " << cyphertext << endl;

                // Расшифровываем сообщение
                vector<uint8_t> decrypted_msg = decrypt(cA, cB, p, cyphertext);
                cout << "Расшифрованное сообщение: " << decrypted_msg << endl << endl;

                // --- Атака на основе известного открытого текста ---
                // Злоумышленник знает msg, cyphertext и p.

                // Запускаем атаку, используя только первый символ сообщения/шифротекста.
                // Предел поиска должен быть больше, чем реальные ключи.
                meet_in_the_middle_attack(p, msg[0], cyphertext[0], 200);
                break;
            }
            case 0:
                cout << "Выход из программы.\n";
                return 0;
            default:
                cout << "Неверный выбор. Пожалуйста, попробуйте снова.\n";
                break;
            }
        }
        catch (const char* s) {
            cerr << "Произошла ошибка: " << s << endl;
        }
        catch (const exception& e) {
            cerr << "Произошла стандартная ошибка: " << e.what() << endl;
        }
    }

    return 0;
}