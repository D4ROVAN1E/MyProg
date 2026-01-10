#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <boost/timer/timer.hpp>
#include "array.hpp"

using namespace std;

// Константы для тестов
const uint32_t SMALL_DATA_SIZE = 100000;
const uint32_t LARGE_DATA_SIZE = 1000000;

// Генератор случайных чисел
int getRandomInt() {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dist(1, 100000);
    return dist(gen);
}

void bench_push_back() {
    cout << "\nBenchmark: MPUSH_BACK" << endl;
    Array<int> arr;
    
    // Таймер запускается
    boost::timer::cpu_timer timer; 

    for (uint32_t i = 0; i < LARGE_DATA_SIZE; ++i) {
        arr.MPUSH_BACK(i);
    }
    
    // Останавливаем таймер и выводим результат
    timer.stop();
    cout << "Обработано элементов: " << LARGE_DATA_SIZE << endl;
    cout << "Результат: " << timer.format() << endl;
}

void bench_insert_middle() {
    cout << "\nBenchmark: MPUSH_BY_IND" << endl;
    cout << "Используем меньший размер данных." << endl;
    
    Array<int> arr;
    // Сначала заполним массив
    for (uint32_t i = 0; i < SMALL_DATA_SIZE; ++i) {
        arr.MPUSH_BACK(i);
    }

    boost::timer::cpu_timer timer;

    // Вставляем 1000 элементов всегда в середину
    for (int i = 0; i < 10000; ++i) {
        arr.MPUSH_BY_IND(arr.GetSize() / 2, getRandomInt());
    }

    timer.stop();
    cout << "Вставок выполнено: 10000 (в массив размером ~" << SMALL_DATA_SIZE << ")" << endl;
    cout << "Результат: " << timer.format() << endl;
}

void bench_access() {
    cout << "\nBenchmark: Operator[] (Случайное чтение)" << endl;
    Array<int> arr(LARGE_DATA_SIZE);
    
    // Заполняем данными
    for (uint32_t i = 0; i < LARGE_DATA_SIZE; ++i) {
        arr.MPUSH_BACK(i);
    }

    boost::timer::cpu_timer timer;

    int sink = 0;
    for (uint32_t i = 0; i < LARGE_DATA_SIZE; ++i) {
        // Читаем случайные индексы
        uint32_t randInd = getRandomInt() % LARGE_DATA_SIZE;
        sink += arr[randInd];
    }

    timer.stop();
    cout << "Прочитано элементов: " << LARGE_DATA_SIZE << endl;
    cout << "Результат: " << timer.format() << endl;
}

void bench_binary_io() {
    cout << "\nBenchmark: Binary Save/Load" << endl;
    Array<int> arr;
    for (uint32_t i = 0; i < LARGE_DATA_SIZE; ++i) {
        arr.MPUSH_BACK(i);
    }
    string filename = "bench_test.bin";

    cout << "[Запись]" << endl;
    boost::timer::cpu_timer timerSave;
    arr.MSAVE_BINARY(filename);
    timerSave.stop();
    cout << "  " << timerSave.format();

    cout << "[Чтение]" << endl;
    Array<int> arrLoad;
    boost::timer::cpu_timer timerLoad;
    arrLoad.MLOAD_BINARY(filename);
    timerLoad.stop();
    cout << "  " << timerLoad.format();
    
    // Удаляем временный файл
    remove(filename.c_str()); 
}

int main() {
    cout << "Запуск Benchmarks для Array<T>" << endl;
    
    try {
        bench_push_back();
        bench_access();
        bench_insert_middle();
        bench_binary_io();
    } catch (const exception& e) {
        cerr << "Exception caught: " << e.what() << endl;
    }

    return 0;
}