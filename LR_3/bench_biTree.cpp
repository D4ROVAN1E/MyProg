#include <iostream>
#include <vector>
#include <string>
#include <cstdio> 
#include <random>
#include <boost/timer/timer.hpp>
#include "binary_tree.hpp"

using namespace std;

const uint32_t DATA_SIZE = 500000;

// Генератор случайных чисел
int getRandomInt() {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dist(1, 10000000); 
    return dist(gen);
}

// Тест вставки (построение дерева)
void bench_insert() {
    cout << "\nBenchmark: TINSERT (Построение дерева)" << endl;
    FullBinaryTree<int> tree;

    // Генерируем данные заранее, чтобы не мерить время генерации
    vector<int> data(DATA_SIZE);
    for(int& val : data) val = getRandomInt();

    boost::timer::cpu_timer timer;

    for (const int& val : data) {
        tree.TINSERT(val);
    }

    timer.stop();
    cout << "Вставлено узлов: " << DATA_SIZE << endl;
    cout << "Результат: " << timer.format() << endl;
}

// Тест обхода проверка на Full Binary
void bench_full_check() {
    cout << "\nBenchmark: TFULL (Обход всех узлов)" << endl;
    
    // Подготовка дерева
    FullBinaryTree<int> tree;
    for (uint32_t i = 0; i < DATA_SIZE; ++i) {
        tree.TINSERT(getRandomInt());
    }

    cout << "Дерево построено. Запуск замера TFULL()..." << endl;

    boost::timer::cpu_timer timer;
    
    bool result = tree.TFULL(); 

    timer.stop();
    cout << "Результат: " << timer.format() << endl;
    (void)result; // Подавляем предупреждение unused variable
}

// Тест ввода/вывода
void bench_io() {
    cout << "\nBenchmark: I/O Operations" << endl;
    
    // Используем меньший размер для IO тестов, чтобы не забивать диск
    const uint32_t IO_SIZE = 100000;
    FullBinaryTree<int> tree;
    for (uint32_t i = 0; i < IO_SIZE; ++i) {
        tree.TINSERT(getRandomInt());
    }

    string txtFile = "tree_test.txt";
    string binFile = "tree_test.bin";

    // TEXT MODE
    cout << "[Text: TSAVE]" << endl;
    boost::timer::cpu_timer timerTxtSave;
    tree.TSAVE(txtFile);
    timerTxtSave.stop();
    cout << " Time: " << timerTxtSave.format();

    cout << "[Text: TLOAD]" << endl;
    FullBinaryTree<int> treeLoadTxt;
    boost::timer::cpu_timer timerTxtLoad;
    treeLoadTxt.TLOAD(txtFile);
    timerTxtLoad.stop();
    cout << " Time: " << timerTxtLoad.format();

    // BINARY MODE
    cout << "[Binary: TSAVE_BINARY]" << endl;
    boost::timer::cpu_timer timerBinSave;
    tree.TSAVE_BINARY(binFile);
    timerBinSave.stop();
    cout << " Time: " << timerBinSave.format();

    cout << "[Binary: TLOAD_BINARY]" << endl;
    FullBinaryTree<int> treeLoadBin;
    boost::timer::cpu_timer timerBinLoad;
    treeLoadBin.TLOAD_BINARY(binFile);
    timerBinLoad.stop();
    cout << " Time: " << timerBinLoad.format();

    // Очистка
    remove(txtFile.c_str());
    remove(binFile.c_str());
}

int main() {
    cout << "Запуск Benchmarks для FullBinaryTree<T>" << endl;

    try {
        bench_insert();
        bench_full_check();
        bench_io();
    } catch (const exception& e) {
        cerr << "Exception caught: " << e.what() << endl;
    }

    return 0;
}