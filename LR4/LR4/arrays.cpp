#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cctype>
#include <string>

using namespace std;

template<typename T>
void print_container(const string& prefix, const T& container) {
    cout << prefix;
    for (const auto& elem : container) {
        cout << elem << " ";
    }
    cout << endl;
}

bool is_prime(int n) {
    if (n <= 1) return false; 
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

bool has_repeated_digits(int n) {
    bool seen[10] = { false }; // Массив для отслеживания встреченных цифр
    if (n == 0) return false;
    int temp = n;
    while (temp > 0) {
        int digit = temp % 10;
        if (seen[digit]) {
            return true; // Цифра уже встречалась
        }
        seen[digit] = true;
        temp /= 10;
    }
    return false;
}

int main() {
    setlocale(LC_ALL, "Russian");

    random_device rd;
    mt19937_64 gen(rd());
    cout << "--- Задания 1, 2, 3 ---" << endl;

    const int n = 20;
    vector<int> arr1(n), arr2(n);
    uniform_int_distribution<> dist1(10, 100);

    for (int& val : arr1) val = dist1(gen);
    for (int& val : arr2) val = dist1(gen);

    print_container("Исходный массив arr1:", arr1);
    print_container("Исходный массив arr2:", arr2);
    cout << endl;

    cout << "Простые числа из диапазона [0, 10] в arr1: ";
    for (int val : arr1) {
        if (val >= 0 && val <= 10 && is_prime(val)) {
            cout << val << " ";
        }
    }
    cout << endl;

    cout << "Простые числа из диапазона [0, 10] в arr2: ";
    for (int val : arr2) {
        if (val >= 0 && val <= 10 && is_prime(val)) {
            cout << val << " ";
        }
    }
    cout << endl;
    cout << "(Числа не найдены, так как диапазон инициализации [10, 100] не содержит простых чисел из диапазона [0, 10])" << endl << endl;

    // Сортировка первой половины arr1 по возрастанию
    sort(arr1.begin(), arr1.begin() + n / 2);

    // Сортировка второй половины arr2 по убыванию
    sort(arr2.begin() + n / 2, arr2.end(), greater<int>());

    print_container("arr1 (первая половина отсортирована):", arr1);
    print_container("arr2 (вторая половина отсортирована):", arr2);
    cout << endl;

    // Обмен отсортированной части arr1 с несортированной частью arr2
    swap_ranges(arr1.begin(), arr1.begin() + n / 2, arr2.begin());

    cout << "Модификация согласно заданию:" << endl;
    print_container("Финальный arr1 (несортированные части):", arr1);
    print_container("Финальный arr2 (отсортированные части):", arr2);
    cout << "------------------------------------------" << endl << endl;


    cout << "--- Задание 4 ---" << endl;
    const int char_array_size = 25;
    string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    uniform_int_distribution<> dist_char(0, alphabet.length() - 1);

    vector<char> char_arr(char_array_size);
    for (char& c : char_arr) {
        c = alphabet[dist_char(gen)];
    }

    print_container("Исходный массив символов:", char_arr);

    // Замена регистра
    for (char& c : char_arr) {
        if (islower(c)) {
            c = toupper(c);
        }
        else if (isupper(c)) {
            c = tolower(c);
        }
    }

    print_container("Массив с измененным регистром:", char_arr);
    cout << "------------------------------------------" << endl << endl;


    cout << "--- Задание 5 ---" << endl;
    const int int_array_size = 15;
    uniform_int_distribution<> dist2(1000, 2000);

    vector<int> source_arr(int_array_size);
    for (int& val : source_arr) {
        val = dist2(gen);
    }
    print_container("Исходный массив [1000, 2000]:", source_arr);

    // Сортировка по убыванию
    sort(source_arr.begin(), source_arr.end(), greater<int>());
    print_container("Отсортированный массив:", source_arr);

    // Создание нового массива с числами, где есть повторяющиеся цифры
    vector<int> filtered_arr;
    for (int val : source_arr) {
        if (has_repeated_digits(val)) {
            filtered_arr.push_back(val);
        }
    }

    print_container("Массив с числами, содержащими повторяющиеся цифры:", filtered_arr);
    cout << "------------------------------------------" << endl;

    return 0;
}