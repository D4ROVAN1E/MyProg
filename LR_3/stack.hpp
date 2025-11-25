#ifndef STACK_HPP
#define STACK_HPP

#include <iostream>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

using namespace std;

template <typename T>
class Stack {
 private:
    T* data;
    uint32_t capacity;
    uint32_t size;

    void doubleStack() {  // Удвоение стека при достижении лимита capacity
        uint32_t cap = capacity;
        T* newData = new T[cap * 2];
        capacity = cap * 2;
        for (uint32_t i = 0; i < size; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }

 public:
    Stack() {
        size = 0;
        capacity = 1;
        data = new T[capacity];
        for (uint32_t i = 0; i < capacity; i++) {
            data[i] = T();
        }
    }

    explicit Stack(const uint32_t cap) {  // Конструктор
        if (cap == 0) {
            throw invalid_argument("Initial capacity must be greater than 0");
        }
        capacity = cap;
        size = 0;
        data = new T[capacity];
        for (uint32_t i = 0; i < capacity; i++) {
            data[i] = T();
        }
    }

    ~Stack() {  // Деструктор
        delete[] data;
    }

    Stack(const Stack<T>& other) {  // Копирующий конструктор
        capacity = other.capacity;
        size = other.size;
        data = new T[capacity];
        for (uint32_t i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    // Копирующий оператор присваивания
    auto operator=(const Stack<T>& other) -> Stack<T>& {
        if (this == &other) {  // Защита от a = a
            return *this;
        }
        delete[] data;

        capacity = other.capacity;
        size = other.size;
        data = new T[capacity];
        for (uint32_t i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
        return *this;
    }

    void SPUSH(T value) {  // Добавление элемента в конец стека
        if (size >= capacity) {
            doubleStack();
        }
        data[size++] = value;
    }

    auto SPOP() -> T {
        if (size == 0) {
            throw out_of_range("Stack underflow: cannot pop from an empty stack");
        }
        T rt = data[size - 1];
        size--;
        return rt;
    }

    void PRINT() {
        for (uint32_t i = 0; i < size; i++) {
            cout << data[i] << " ";
        }
        cout << endl;
    }

    // Сохранение стека в файл
    void SSAVE(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Could not open file for writing (" + filename + ")");
        }
        file << size << endl;
        for (uint32_t i = 0; i < size; i++) {
            file << data[i] << " ";
        }
        file.close();
        cout << "Стек сохранён в файл: " << filename << endl;
    }

    // Загрузка стека из файла
    void SLOAD(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Could not open file for reading (" + filename + ")");
        }
        uint32_t nsize;
        file >> nsize;
        
        if (file.fail()) {
             throw runtime_error("Failed to read stack size from " + filename);
        }

        size = 0;
        T value;
        while (size < nsize && file >> value) {
            SPUSH(value);
        }
        file.close();
        cout << "Стек загружен из файла: " << filename << endl;
    }

    // Сохранение в бинарный файл
    void SSAVE_BINARY(const string& filename) {
        // Открываем файл с флагом ios::binary
        ofstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Could not open binary file for writing (" + filename + ")");
        }

        // Записываем размер стека (сколько элементов считывать)
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));

        // Записываем массив данных целиком
        // reinterpret_cast преобразует указатель T* в char*, чтобы write мог записать байты
        file.write(reinterpret_cast<const char*>(data), size * sizeof(T));
        
        if (file.fail()) {
            throw runtime_error("Failed to write data to binary file");
        }

        file.close();
        cout << "Стек сохранён (bin): " << filename << endl;
    }

    // Загрузка из бинарного файла
    void SLOAD_BINARY(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Could not open binary file for reading (" + filename + ")");
        }

        uint32_t newSize = 0;
        // Читаем размер записанного стека
        file.read(reinterpret_cast<char*>(&newSize), sizeof(newSize));
        
        if (file.fail()) {
             throw runtime_error("Failed to read size from binary file");
        }

        // Если текущей ёмкости (capacity) не хватает, перевыделяем память
        if (newSize > capacity) {
            delete[] data;
            capacity = newSize; 
            data = new T[capacity];
        }

        // Обновляем размер и читаем данные прямо в память
        size = newSize;
        file.read(reinterpret_cast<char*>(data), size * sizeof(T));

        if (file.fail()) {
             throw runtime_error("Failed to read data from binary file (unexpected EOF or format)");
        }

        file.close();
        cout << "Стек загружен (bin): " << filename << endl;
    }

    auto GetSize() -> uint32_t {
        return size;
    }
};

#endif  // STACK_HPP
