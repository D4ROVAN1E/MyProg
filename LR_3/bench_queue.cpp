#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <boost/timer/timer.hpp>
#include "queue.hpp"

using namespace std;
using namespace boost::timer;

// Количество операций для нагрузочных тестов
const uint32_t NUM_OPS = 10000000; 

void bench_growth_drain() {
    cout << "\nBenchmark: Growth (Resize) & Drain" << endl;
    cout << "Количество элементов: " << NUM_OPS << endl;
    
    Queue<int> q;

    // Тест на заполнение
    cout << "[QPUSH] Заполнение очереди (с ресайзами)..." << endl;
    cpu_timer timerPush;
    for (uint32_t i = 0; i < NUM_OPS; ++i) {
        q.QPUSH(i);
    }
    timerPush.stop();
    cout << "  Время: " << timerPush.format();

    // Тест на опустошение
    cout << "[QPOP] Опустошение очереди..." << endl;
    cpu_timer timerPop;
    while (!q.empty()) {
        q.QPOP();
    }
    timerPop.stop();
    cout << "  Время: " << timerPop.format();
}

void bench_circular_buffer() {
    cout << "\nBenchmark: Circular Buffer Logic" << endl;
    
    // Предзаполняем очередь, чтобы она не была пустой
    Queue<int> q;
    const int STABLE_SIZE = 5000;
    for (int i = 0; i < STABLE_SIZE; ++i) q.QPUSH(i);
 
    cpu_timer timerCircle;
    
    for (uint32_t i = 0; i < NUM_OPS; ++i) {
        q.QPUSH(i); // Хвост сдвинулся
        q.QPOP();   // Голова сдвинулась
    }
    
    timerCircle.stop();
    cout << "  Итераций: " << NUM_OPS << endl;
    cout << "  Время: " << timerCircle.format();
}

void bench_io() {
    cout << "\nBenchmark: I/O Operations" << endl;
    
    const uint32_t IO_SIZE = 500000; 
    Queue<int> q;
    for (uint32_t i = 0; i < IO_SIZE; ++i) q.QPUSH(i);

    string txtFile = "queue_bench.txt";
    string binFile = "queue_bench.bin";

    // TEXT SAVE
    cout << "[Text Save]   ";
    cpu_timer tTS;
    q.QSAVE(txtFile);
    tTS.stop();
    cout << tTS.format();

    // TEXT LOAD
    cout << "[Text Load]   ";
    Queue<int> qTxt;
    cpu_timer tTL;
    qTxt.QLOAD(txtFile);
    tTL.stop();
    cout << tTL.format();

    // BINARY SAVE
    cout << "[Binary Save] ";
    cpu_timer tBS;
    q.QSAVE_BINARY(binFile);
    tBS.stop();
    cout << tBS.format();

    // BINARY LOAD
    cout << "[Binary Load] ";
    Queue<int> qBin;
    cpu_timer tBL;
    qBin.QLOAD_BINARY(binFile);
    tBL.stop();
    cout << tBL.format();

    // Удаление файлов
    remove(txtFile.c_str());
    remove(binFile.c_str());
}

int main() {
    setlocale(LC_ALL, "");
    cout << "Запуск Benchmarks для Queue" << endl;

    try {
        bench_growth_drain();
        bench_circular_buffer();
        bench_io();
    } catch (const exception& e) {
        cerr << "CRITICAL ERROR: " << e.what() << endl;
    }

    return 0;
}