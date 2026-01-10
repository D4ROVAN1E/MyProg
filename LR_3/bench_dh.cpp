#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <random>
#include <boost/timer/timer.hpp>
#include "dh.hpp"

using namespace std;
using namespace boost::timer;

const uint32_t NUM_ELEMENTS = 100000;  // Количество элементов для теста
const uint32_t STRING_LEN = 16;        // Длина ключа

// Генерация случайных ключей заранее, чтобы исключить генерацию из замеров
auto generateKeys(uint32_t count) -> vector<string> {
    vector<string> keys;
    keys.reserve(count);

    // Символы для ключей
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    for (uint32_t i = 0; i < count; ++i) {
        string s;
        s.reserve(STRING_LEN);
        for (uint32_t j = 0; j < STRING_LEN; ++j) {
            s += charset[dist(gen)];
        }
        keys.push_back(s);
    }
    return keys;
}

void bench_core_operations() {
    cout << "\nBenchmark: DoubleHash Core Operations" << endl;
    cout << "Количество элементов: " << NUM_ELEMENTS << endl;

    // Подготовка
    vector<string> keys = generateKeys(NUM_ELEMENTS);
    DoubleHash<int> hashTable; 

    // Вставка
    cout << "[INSERT] Заполнение таблицы..." << endl;
    
    cpu_timer timerInsert;
    for (uint32_t i = 0; i < NUM_ELEMENTS; ++i) {
        hashTable.insert(keys[i], i);
    }
    timerInsert.stop();
    cout << " Время: " << timerInsert.format();

    // Поиск существующих
    cout << "[FIND HIT] Поиск всех ключей..." << endl;
    
    cpu_timer timerFind;
    volatile int sink = 0; // Защита от оптимизации
    for (const auto& key : keys) {
        int* val = hashTable.find(key);
        if (val) sink += *val;
    }
    timerFind.stop();
    cout << " Время: " << timerFind.format();

    // Поиск несуществующих
    // Генерируем ключи, которых точно нет.
    cout << "[FIND MISS] Поиск несуществующих ключей..." << endl;
    
    vector<string> missingKeys;
    missingKeys.reserve(NUM_ELEMENTS);
    for (const auto& k : keys) missingKeys.push_back("MISS_" + k);

    cpu_timer timerMiss;
    for (const auto& key : missingKeys) {
        int* val = hashTable.find(key);
        if (val) sink += *val;
    }
    timerMiss.stop();
    cout << "  Время: " << timerMiss.format();

    // Удаление (Remove)
    cout << "[REMOVE] Удаление 50% элементов..." << endl;
    
    cpu_timer timerRemove;
    for (uint32_t i = 0; i < NUM_ELEMENTS / 2; ++i) {
        hashTable.remove(keys[i]);
    }
    timerRemove.stop();
    cout << "  Время: " << timerRemove.format();
    
    cout << "Осталось элементов: " << hashTable.size() << endl;
}

void bench_io_operations() {
    cout << "\nBenchmark: DoubleHash I/O" << endl;
    
    // Используем меньший набор данных для IO
    uint32_t IO_SIZE = 50000;
    DoubleHash<int> ht;
    vector<string> keys = generateKeys(IO_SIZE);
    for (uint32_t i = 0; i < IO_SIZE; ++i) ht.insert(keys[i], i);

    string txtFile = "dh_bench.txt";
    string binFile = "dh_bench.bin";

    //TEXT SAVE
    cout << "[Text Save]   ";
    cpu_timer tTS;
    ht.serialize_text(txtFile);
    tTS.stop();
    cout << tTS.format();

    //TEXT LOAD
    // Чтение текстового файла медленное из-за парсинга строк >> int
    cout << "[Text Load]   ";
    DoubleHash<int> htTxt;
    cpu_timer tTL;
    htTxt.deserialize_text(txtFile);
    tTL.stop();
    cout << tTL.format();

    //BINARY SAVE
    cout << "[Binary Save] ";
    cpu_timer tBS;
    ht.serialize_bin(binFile);
    tBS.stop();
    cout << tBS.format();

    //BINARY LOA
    cout << "[Binary Load] ";
    DoubleHash<int> htBin;
    cpu_timer tBL;
    htBin.deserialize_bin(binFile);
    tBL.stop();
    cout << tBL.format();

    // Удаление временных файлов
    remove(txtFile.c_str());
    remove(binFile.c_str());
}

int main() {

    cout << "Запуск Benchmarks для DoubleHash" << endl;
    
    try {
        bench_core_operations();
        bench_io_operations();
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}