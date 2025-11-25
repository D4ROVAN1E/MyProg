#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <iostream>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept> 

using namespace std;

template <typename T>
class Array {
 private:
    T* data;
    uint32_t capacity;
    uint32_t size;

    void doubleArray() {  // Удвоение массива при достижении лимита capacity
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
    Array() : size(0)  // Конструктор для пустого массива
            , capacity(1)
            , data(new T[1]) {}

    explicit Array(const uint32_t cap) : size(cap - 1)  // Конструктор
                                , capacity(cap)
                                , data(new T[cap]) {
        for (uint32_t i = 0; i < cap; i++) {
            data[i] = T();
        }
    }

    ~Array() {  // Деструктор
        delete[] data;
    }

    Array(const Array<T>& other) : size(other.size)  // Копирующий конструктор
                                    , capacity(other.capacity)
                                    , data(new T[other.capacity]) {
        for (uint32_t i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    // Копирующий оператор присваивания
    auto operator=(const Array<T>& other) -> Array<T>& {
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

    // Неконстантная перегрузка оператора скобок
    auto operator[](uint32_t index) -> T& {
        if (index >= size) {
            throw out_of_range("Error: Index " + to_string(index) 
            + " is out of bounds (size " + to_string(size) + ").");
        }
        return data[index];
    }

    // Константная перегрузка оператора скобок (для чтения)
    auto operator[](uint32_t index) const -> const T& {
    if (index >= size) {
        throw out_of_range("Error: Index " + to_string(index) 
        + " is out of bounds (size " + to_string(size) + ").");
    }
    return data[index];
}

    void MPUSH_BACK(T value) {  // Добавление элемента в конец массива
        if (size + 1 > capacity) {
            doubleArray();
        }
        data[size] = value;
        size++;
    }

    // Добавление элемента по индексу
    void MPUSH_BY_IND(uint32_t index, T value) {
        if (size + 1 > capacity) {
            doubleArray();
        }
        if (index <= size) {
            for (uint32_t j = size; j > index; j--) {
                data[j] = data[j - 1];
            }
            data[index] = value;
            size++;
        } else {
            throw out_of_range("Error: Index " + to_string(index) + " is out of bounds for insertion.");
        }
    }

    // Получение элемента по индексу
    auto MGET_BY_IND(uint32_t index) const -> T& {
        if (index < size) {
            return data[index];
        } else {
            throw out_of_range("Error: Index " + to_string(index) + " is out of bounds.");
        }
    }

    void MDEL_BY_IND(uint32_t index) {
        if (index < size) {
            for (uint32_t i = index; i < size - 1; i++) {
                data[i] = data[i + 1];
            }
            size--;
        } else {
            throw out_of_range("Error: Index " + to_string(index) + " is out of bounds for deletion.");
        }
    }

    void MSWAP_BY_IND(uint32_t index, T value) {
        if (index < size) {
            data[index] = value;
        } else {
            throw out_of_range("Error: Index " + to_string(index) + " is out of bounds for swap.");
        }
    }

    void PRINT() const {
        for (uint32_t i = 0; i < size; i++) {
            cout << data[i] << " ";
        }
        cout << endl;
    }

    // Сохранение массива в файл
    void MSAVE(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Error: Unable to open file for writing: " + filename);
        }
        file << size << endl;
        for (uint32_t i = 0; i < size; i++) {
            file << data[i] << " ";
        }
        file.close();
        cout << "Массив сохранён в файл: " << filename << endl;
    }

    // Загрузка массива из файла
    void MLOAD(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Error: Unable to open file for reading: " + filename);
        }
        uint32_t NewSize;
        if (!(file >> NewSize)) {
             throw runtime_error("Error: Failed to read size from file: " + filename);
        }
        
        size = 0;
        T value;
        while (size < NewSize && file >> value) {
            MPUSH_BACK(value);
        }

        if (size != NewSize) {
            throw runtime_error("Error: File corrupted or incomplete data.");
        }

        file.close();
        cout << "Массив загружен из файла: " << filename << endl;
    }

    // Сохранение массива в бинарный файл
    void MSAVE_BINARY(const string& filename) const {
        ofstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Error: Unable to open file for binary writing: " + filename);
        }

        // Записываем размер массива
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));

        if (size > 0) {
            file.write(reinterpret_cast<const char*>(data), size * sizeof(T));
        }
        
        if (!file) {
             throw runtime_error("Error: Write operation failed for file: " + filename);
        }

        file.close();
        cout << "Массив (бинарный) сохранён в файл: " << filename << endl;
    }

    // Загрузка массива из бинарного файла
    void MLOAD_BINARY(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
             throw runtime_error("Error: Unable to open file for binary reading: " + filename);
        }

        uint32_t newSize = 0;
        // Читаем размер массива
        file.read(reinterpret_cast<char*>(&newSize), sizeof(newSize));

        if (!file) { 
            throw runtime_error("Error: Failed to read size from binary file.");
        }

        // Подготовка памяти
        if (newSize > capacity) {
            delete[] data;
            capacity = newSize; 
            data = new T[capacity];
        }
        size = newSize;

        // Читаем данные прямо в массив
        if (size > 0) {
            file.read(reinterpret_cast<char*>(data), size * sizeof(T));
            if (!file) {
                 throw runtime_error("Error: Failed to read data from binary file (incomplete file).");
            }
        }

        file.close();
        cout << "Массив (бинарный) загружен из файла: " << filename << endl;
    }

    [[nodiscard]] auto GetSize() const -> uint32_t {
        return size;
    }

    [[nodiscard]] auto GetCapacity() const -> uint32_t {
        return capacity;
    }

    void SetSize(uint32_t newSize) {
        if (newSize > capacity) {
             throw length_error("Error: New size exceeds current capacity.");
        }
        size = newSize;
    }

    void SetCapacity(uint32_t newCapacity) {
        if (newCapacity < size) {
            throw length_error("Error: New capacity cannot be smaller than current size.");
        }
        capacity = newCapacity;
    }
};

#endif   // ARRAY_HPP