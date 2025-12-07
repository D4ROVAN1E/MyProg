#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <boost/timer/timer.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include "doubly_list.hpp"

using namespace std;
using namespace boost::timer;
using namespace boost::random;

// Количество элементов для основных тестов
const uint32_t NUM_ELEMENTS = 100000; 

// Генератор случайных чисел
int getRandomInt() {
    static boost::random::random_device rd;
    static boost::random::mt19937 gen(rd());
    static boost::random::uniform_int_distribution<> dist(1, 1000000);
    return dist(gen);
}

// Тест вставки в начало и конец (O(1))
void bench_push_ends() {
    cout << "\nBenchmark: Push Head vs Push Back (O(1))" << endl;
    
    DoublyList<int> listHead;
    cout << "[LPUSH_HEAD] Вставка " << NUM_ELEMENTS << " элементов в начало..." << endl;
    cpu_timer timerHead;
    for (uint32_t i = 0; i < NUM_ELEMENTS; ++i) {
        listHead.LPUSH_HEAD(i);
    }
    timerHead.stop();
    cout << "  Время: " << timerHead.format();

    DoublyList<int> listBack;
    cout << "[LPUSH_BACK] Вставка " << NUM_ELEMENTS << " элементов в конец..." << endl;
    cpu_timer timerBack;
    for (uint32_t i = 0; i < NUM_ELEMENTS; ++i) {
        listBack.LPUSH_BACK(i);
    }
    timerBack.stop();
    cout << "  Время: " << timerBack.format();
}

// Тест поиска и вставки в середину
void bench_middle_operations() {
    cout << "\nBenchmark: Middle Operations (Search + Insert)" << endl;
    cout << "Примечание: LPUSH_AFTER выполняет линейный поиск O(N)." << endl;

    // Создаем список поменьше
    uint32_t SMALL_SIZE = 10000;
    DoublyList<int> list;
    for (uint32_t i = 0; i < SMALL_SIZE; ++i) {
        list.LPUSH_BACK(i); // Заполняем числами 
    }

    cout << "[LPUSH_AFTER] Вставка 1000 элементов в середину списка..." << endl;
    
    cpu_timer timerMid;
    // Пытаемся вставить число -1 после каждого четного элемента в первой половине
    for (uint32_t i = 0; i < 1000; ++i) {
        // Ищем элемент 'i * 2' и вставляем после него
        list.LPUSH_AFTER(i * 2, -1);
    }
    timerMid.stop();
    cout << "  Время: " << timerMid.format();
}

// Тест поиска (LGET_BY_VALUE)
void bench_search() {
    cout << "\nBenchmark: LGET_BY_VALUE (Linear Search)" << endl;
    
    DoublyList<int> list;
    // Заполняем
    for (uint32_t i = 0; i < NUM_ELEMENTS; ++i) {
        list.LPUSH_BACK(i);
    }

    cout << "Поиск случайных элементов в списке размера " << NUM_ELEMENTS << "..." << endl;
    
    cpu_timer timerSearch;
    volatile int foundCount = 0;

    // Ищем 1000 случайных чисел
    for (int i = 0; i < 1000; ++i) {
        int target = getRandomInt() % NUM_ELEMENTS; 
        if (list.LGET_BY_VALUE(target) != nullptr) {
            foundCount++;
        }
    }
    timerSearch.stop();
    cout << "  Время: " << timerSearch.format();
}

// Тест ввода-вывода (IO)
void bench_io() {
    cout << "\nBenchmark: I/O Operations (Text vs Binary)" << endl;
    
    DoublyList<int> list;
    for (uint32_t i = 0; i < NUM_ELEMENTS; ++i) {
        list.LPUSH_BACK(i);
    }

    string txtFile = "list_bench.txt";
    string binFile = "list_bench.bin";

    // TEXT
    cout << "[Text Save]   ";
    cpu_timer tTS;
    list.LSAVE(txtFile);
    tTS.stop();
    cout << tTS.format();

    cout << "[Text Load]   ";
    DoublyList<int> listTxt;
    cpu_timer tTL;
    listTxt.LLOAD(txtFile);
    tTL.stop();
    cout << tTL.format();

    // BINARY
    cout << "[Binary Save] ";
    cpu_timer tBS;
    list.LSAVE_BIN(binFile);
    tBS.stop();
    cout << tBS.format();

    cout << "[Binary Load] ";
    DoublyList<int> listBin;
    cpu_timer tBL;
    listBin.LLOAD_BIN(binFile);
    tBL.stop();
    cout << tBL.format();

    // Cleanup
    remove(txtFile.c_str());
    remove(binFile.c_str());
}

int main() {

    cout << "Запуск Benchmarks для DoublyList" << endl;
    
    try {
        bench_push_ends();
        bench_middle_operations();
        bench_search();
        bench_io();
    } catch (const exception& e) {
        cerr << "CRITICAL ERROR: " << e.what() << endl;
    }

    return 0;
}