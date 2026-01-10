#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <boost/timer/timer.hpp>
#include "stack.hpp"

using namespace std;
using namespace boost::timer;

// Количество операций (10 миллионов)
const uint32_t NUM_ELEMENTS = 10000000; 

void bench_push_pop() {
    cout << "\nBenchmark: Push & Pop" << endl;
    cout << "Количество элементов: " << NUM_ELEMENTS << endl;
    
    Stack<int> s;

    // Тест SPUSH
    cout << "[SPUSH] Заполнение стека..." << endl;
    cpu_timer timerPush;
    for (uint32_t i = 0; i < NUM_ELEMENTS; ++i) {
        s.SPUSH(i);
    }
    timerPush.stop();
    cout << "  Время: " << timerPush.format();

    // Тест SPOP
    cout << "[SPOP] Опустошение стека..." << endl;
    cpu_timer timerPop;
    for (uint32_t i = 0; i < NUM_ELEMENTS; ++i) {
        s.SPOP();
    }
    timerPop.stop();
    cout << "  Время: " << timerPop.format();
}

void bench_io() {
    cout << "\nBenchmark: I/O Operations" << endl;
    
    // Для IO берем меньше элементов
    const uint32_t IO_SIZE = 500000; 
    Stack<int> s;
    for (uint32_t i = 0; i < IO_SIZE; ++i) s.SPUSH(i);

    string txtFile = "stack_bench.txt";
    string binFile = "stack_bench.bin";

    // TEXT SAVE
    cout << "[Text Save]   ";
    cpu_timer tTS;
    s.SSAVE(txtFile);
    tTS.stop();
    cout << tTS.format();

    // TEXT LOAD
    cout << "[Text Load]   ";
    Stack<int> sTxt;
    cpu_timer tTL;
    sTxt.SLOAD(txtFile);
    tTL.stop();
    cout << tTL.format();

    // BINARY SAVE
    cout << "[Binary Save] ";
    cpu_timer tBS;
    s.SSAVE_BINARY(binFile);
    tBS.stop();
    cout << tBS.format();

    // BINARY LOAD
    cout << "[Binary Load] ";
    Stack<int> sBin;
    cpu_timer tBL;
    sBin.SLOAD_BINARY(binFile);
    tBL.stop();
    cout << tBL.format();

    // Удаление временных файлов
    remove(txtFile.c_str());
    remove(binFile.c_str());
}

int main() {
    setlocale(LC_ALL, "");
    cout << "Запуск Benchmarks для Stack" << endl;

    try {
        bench_push_pop();
        bench_io();
    } catch (const exception& e) {
        cerr << "CRITICAL ERROR: " << e.what() << endl;
    }

    return 0;
}