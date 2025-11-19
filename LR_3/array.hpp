#ifndef ARRAY_HPP
#define ARRAY_HPP
#include <iostream>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>

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

    //  Копирующий оператор присваивания
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
        if (index < 0 || index > capacity) {
            cerr << "Error: Index " << index
             << " is out of bounds (size " << size << ").\n";
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
        if (index >= 0 && index < capacity) {
            if (index < size) {
                for (uint32_t j = size - 1; j >= index && j < size; j--) {
                    data[j + 1] = data[j];
                }
            }
            data[index] = value;
            size++;
        } else {
            cout << "Error: Index out of bounds" << endl;
        }
    }

    // Получение элемента по индексу
    auto MGET_BY_IND(uint32_t index) const -> T& {
        if (index >= 0 && index < size)
            return data[index];
        else
            throw "Error: Index out of bounds\n";
    }

    void MDEL_BY_IND(uint32_t index) {
        if (index >= 0 && index < size) {
            for (uint32_t i = index; i < size - 1; i++) {
                data[i] = data[i + 1];
            }
            size--;
        } else {
            cout << "Error: Index out of bounds" << endl;
        }
    }

    void MSWAP_BY_IND(uint32_t index, T value) {
        if (index >= 0 && index < size) {
            data[index] = value;
        } else {
            cout << "Error: Index out of bounds" << endl;
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
            cout << "Ошибка открытия файла для записи!" << endl;
            return;
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
        if (!file.is_open()) return;
        uint32_t NewSize;
        file >> NewSize;
        size = 0;
        T value;
        while (file >> value && size < NewSize) {
            MPUSH_BACK(value);
        }
        file.close();
        cout << "Массив загружен из файла: " << filename << endl;
    }

    [[nodiscard]] auto GetSize() const -> uint32_t {
        return size;
    }

    [[nodiscard]] auto GetCapacity() const -> uint32_t {
        return capacity;
    }

    void SetSize(uint32_t newSize) {
        size = newSize;
    }

    void SetCapacity(uint32_t newCapacity) {
        capacity = newCapacity;
    }
};

#endif   // ARRAY_HPP
