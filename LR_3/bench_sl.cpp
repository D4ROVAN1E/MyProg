#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <boost/timer/timer.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include "singly_list.hpp"

using namespace std;
using namespace boost::timer;
using namespace boost::random;

// Количество элементов.
const uint32_t NUM_ELEMENTS = 30000; 

// Генератор случайных чисел
int getRandomInt() {
    static boost::random::random_device rd;
    static boost::random::mt19937 gen(rd());
    static boost::random::uniform_int_distribution<> dist(1, 1000000);
    return dist(gen);
}

// Тест вставки: Начало vs Конец
void bench_push_comparison() {
    cout << "\nBenchmark: Push Head vs Push Back" << endl;
    cout << "Количество элементов: " << NUM_ELEMENTS << endl;
    
    // HEAD
    ForwardList<int> listHead;
    cout << "[FPUSH_HEAD] Вставка в начало (O(1))..." << endl;
    cpu_timer timerHead;
    for (uint32_t i = 0; i < NUM_ELEMENTS; ++i) {
        listHead.FPUSH_HEAD(i);
    }
    timerHead.stop();
    cout << "  Время: " << timerHead.format();

    // BACK
    ForwardList<int> listBack;
    cout << "[FPUSH_BACK] Вставка в конец (O(N) каждая -> итого O(N^2))..." << endl;
    cpu_timer timerBack;
    for (uint32_t i = 0; i < NUM_ELEMENTS; ++i) {
        listBack.FPUSH_BACK(i);
    }
    timerBack.stop();
    cout << "  Время: " << timerBack.format();
    
    // Результат listBack должен быть в тысячи раз медленнее listHead
}

// Тест удаления: Начало vs Конец
void bench_del_comparison() {
    cout << "\nBenchmark: Delete Head vs Delete Back" << endl;
    
    // Подготовим два одинаковых списка
    ForwardList<int> list1;
    ForwardList<int> list2;
    for (uint32_t i = 0; i < NUM_ELEMENTS; ++i) {
        list1.FPUSH_HEAD(i);
        list2.FPUSH_HEAD(i);
    }

    // DEL HEAD
    cout << "[FDEL_HEAD] Удаление с головы (O(1))..." << endl;
    cpu_timer timerHead;
    while (list1.GetHead() != nullptr) {
        list1.FDEL_HEAD();
    }
    timerHead.stop();
    cout << "  Время: " << timerHead.format();

    // DEL BACK
    cout << "[FDEL_BACK] Удаление с хвоста (O(N) каждое)..." << endl;
    cpu_timer timerBack;
    while (list2.GetHead() != nullptr) {
        list2.FDEL_BACK();
    }
    timerBack.stop();
    cout << "  Время: " << timerBack.format();
}

// Тест поиска и вставки перед элементом
void bench_find_insert() {
    cout << "\nBenchmark: Find & Push Before" << endl;
    
    ForwardList<int> list;
    // Заполняем список значениями 0, 1, 2...
    for (uint32_t i = 0; i < 10000; ++i) {
        list.FPUSH_HEAD(i); 
    }

    cout << "[FPUSH_BEFORE] Поиск и вставка в середину..." << endl;
    
    cpu_timer timer;
    for (int i = 0; i < 1000; ++i) {
        // Ищем элементы ближе к концу списка, чтобы спровоцировать долгий поиск
        int target = i * 5; 
        try {
            list.FPUSH_BEFORE(target, -1);
        } catch (...) {
        }
    }
    timer.stop();
    cout << "  Операций: 1000" << endl;
    cout << "  Время: " << timer.format();
}

// Тест ввода-вывода (IO)
void bench_io() {
    cout << "\nBenchmark: I/O Operations (Text vs Binary)" << endl;
    
    // Увеличим размер для IO
    const uint32_t IO_SIZE = 50000;
    ForwardList<int> list;
    for (uint32_t i = 0; i < IO_SIZE; ++i) {
        list.FPUSH_HEAD(i);
    }

    string txtFile = "slist_bench.txt";
    string binFile = "slist_bench.bin";

    // TEXT SAVE
    cout << "[Text Save]   ";
    cpu_timer tTS;
    list.FSAVE(txtFile);
    tTS.stop();
    cout << tTS.format();

    // TEXT LOAD
    cout << "[Text Load]   ";
    ForwardList<int> listTxt;
    cpu_timer tTL;
    listTxt.FLOAD(txtFile);
    tTL.stop();
    cout << tTL.format();

    // BINARY SAVE
    cout << "[Binary Save] ";
    cpu_timer tBS;
    list.FSERIALIZE(binFile);
    tBS.stop();
    cout << tBS.format();

    // BINARY LOAD
    cout << "[Binary Load] ";
    ForwardList<int> listBin;
    cpu_timer tBL;
    listBin.FDESERIALIZE(binFile);
    tBL.stop();
    cout << tBL.format();

    // Удаление файлов
    remove(txtFile.c_str());
    remove(binFile.c_str());
}

int main() {
    setlocale(LC_ALL, "");
    cout << "Запуск Benchmarks для ForwardList (Singly Linked List)" << endl;

    try {
        bench_push_comparison();
        bench_del_comparison();
        bench_find_insert();
        bench_io();
    } catch (const exception& e) {
        cerr << "CRITICAL ERROR: " << e.what() << endl;
    }

    return 0;
}