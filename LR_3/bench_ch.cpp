#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <boost/timer/timer.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include "ch.hpp"

// Константы
const uint32_t NUM_ELEMENTS = 10000;    // Количество элементов
const uint32_t STRING_LEN = 6;          // Длина ключа-строки

// Генератор случайных строк (ключей)
// Генерируем заранее, чтобы не включать это время в замер хэш-таблицы
vector<string> generateKeys(uint32_t count) {
    vector<string> keys;
    keys.reserve(count);

    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    boost::random::random_device rd;
    boost::random::mt19937 gen(rd());
    boost::random::uniform_int_distribution<> dist(0, sizeof(alphanum) - 2);

    for (uint32_t i = 0; i < count; ++i) {
        string s;
        s.reserve(STRING_LEN);
        for (uint32_t j = 0; j < STRING_LEN; ++j) {
            s += alphanum[dist(gen)];
        }
        keys.push_back(s);
    }
    return keys;
}

void bench_core_operations() {
    cout << "\nBenchmark: Core Operations (Insert/Find/Remove)" << endl;
    
    // Подготовка данных
    cout << "Генерация " << NUM_ELEMENTS << " случайных ключей..." << endl;
    vector<string> keys = generateKeys(NUM_ELEMENTS);
    
    // Создаем таблицу. Начнем с малого размера, чтобы форсировать resize.
    CuckooHash<int> hashTable(100); 

    // Вставка
    cout << "[INSERT] Вставка " << NUM_ELEMENTS << " элементов..." << endl;
    boost::timer::cpu_timer timerInsert;
    
    for (uint32_t i = 0; i < NUM_ELEMENTS; ++i) {
        // Ключ - случайная строка, Значение - просто число i
        hashTable.insert(keys[i], i);
    }
    
    timerInsert.stop();
    cout << "  Time: " << timerInsert.format();

    // Поиск (Успешный)
    cout << "[FIND HIT] Поиск всех существующих ключей..." << endl;
    boost::timer::cpu_timer timerFind;
    
    volatile int dummySum = 0; // Чтобы компилятор не выкинул цикл
    for (const auto& key : keys) {
        int* val = hashTable.find(key);
        if (val) dummySum += *val;
    }
    
    timerFind.stop();
    cout << "  Time: " << timerFind.format();

    // Поиск (Неудачный - MISS)
    // Генерируем новые ключи, которых точно нет (добавляем префикс)
    cout << "[FIND MISS] Поиск несуществующих ключей..." << endl;
    vector<string> missingKeys;
    missingKeys.reserve(NUM_ELEMENTS);
    for(const auto& k : keys) missingKeys.push_back("MISS_" + k);

    boost::timer::cpu_timer timerMiss;
    
    for (const auto& key : missingKeys) {
        int* val = hashTable.find(key);
        if (val) dummySum += *val; // Сюда мы зайти не должны
    }
    
    timerMiss.stop();
    cout << "  Time: " << timerMiss.format();

    // Удаление
    cout << "[REMOVE] Удаление половины элементов..." << endl;
    boost::timer::cpu_timer timerRem;
    
    for (uint32_t i = 0; i < NUM_ELEMENTS / 2; ++i) {
        hashTable.remove(keys[i]);
    }

    timerRem.stop();
    cout << "  Time: " << timerRem.format();
    
    cout << "Текущий размер таблицы после удаления: " << hashTable.size() << endl;
}

void bench_io() {
    cout << "\nBenchmark: I/O Operations (Text vs Binary)" << endl;
    
    // Подготовим таблицу поменьше для IO тестов
    uint32_t IO_SIZE = 50000;
    CuckooHash<int> ht(IO_SIZE * 2); 
    vector<string> keys = generateKeys(IO_SIZE);
    for(uint32_t i=0; i<IO_SIZE; ++i) ht.insert(keys[i], i);

    string txtFile = "hash_bench.txt";
    string binFile = "hash_bench.bin";

    // Text Save
    cout << "[Text Save]   ";
    boost::timer::cpu_timer tTS;
    ht.serialize_text(txtFile);
    tTS.stop();
    cout << tTS.format();

    // Text Load
    cout << "[Text Load]   ";
    CuckooHash<int> htTxt;
    boost::timer::cpu_timer tTL;
    htTxt.deserialize_text(txtFile);
    tTL.stop();
    cout << tTL.format();

    // Binary Save
    cout << "[Binary Save] ";
    boost::timer::cpu_timer tBS;
    ht.serialize_bin(binFile);
    tBS.stop();
    cout << tBS.format();

    // Binary Load
    cout << "[Binary Load] ";
    CuckooHash<int> htBin;
    boost::timer::cpu_timer tBL;
    htBin.deserialize_bin(binFile);
    tBL.stop();
    cout << tBL.format();

    // Cleanup
    remove(txtFile.c_str());
    remove(binFile.c_str());
}

int main() {
    cout << "Запуск Benchmarks для CuckooHash" << endl;
    
    try {
        bench_core_operations();
        bench_io();
    } catch (const exception& e) {
        cerr << "CRITICAL ERROR: " << e.what() << endl;
    }

    return 0;
}