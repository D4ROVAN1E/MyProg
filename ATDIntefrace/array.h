#pragma once
#include <iostream>
#include <cstdint>
#include <fstream>
#include <sstream>

using namespace std;

template <typename T>
struct Array {
    T* data;
    uint32_t capacity;
    uint32_t size;

    Array() { //Конструктор для пустого массива
        size = capacity = 1;
        data = new T[capacity];
        for (uint32_t i = 0; i < capacity; i++) {
            data[i] = string("0");
        }
    }

    Array(const uint32_t cap) { //Конструктор
        size = capacity = cap - 1;
        data = new T[cap];
        for (uint32_t i = 0; i < cap; i++) {
            data[i] = string("0");
        }
    }

    ~Array() { //Деструктор (очищает помять когда выходим из области видимости с массивом)
        delete[] data;
    }

    Array(const Array<T>& other) { //Копирующий конструктор (Если создаём новый массив на основе предыдущего)
        capacity = other.capacity;
        size = other.size;
        data = new T[capacity];
        for (uint32_t i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    Array<T>& operator=(const Array<T>& other) { //Копирующий оператор присваивания (Если имеем два массива с данными и присваиваем один другому)
        if (this == &other) { //Защита от a = a
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
};

template <typename T>
void doubleArray(Array<T>& ar) { //Удвоение массива при достижении лимита capacity
    uint32_t cap = ar.capacity;
    T* newData = new T[cap * 2];
    ar.capacity = cap * 2;
    for (uint32_t i = 0; i < ar.size; i++) {
        newData[i] = ar.data[i];
    }
    delete[] ar.data;
    ar.data = newData;
}

template <typename T>
void MPUSH_BACK(Array<T>& ar, T value) { //Добавление элемента в конец массива
    if (ar.size + 1 > ar.capacity) {
        doubleArray(ar);
    }
    ar.data[ar.size] = value;
    ar.size++;
}

template <typename T>
void MPUSH_BY_IND(Array<T>& ar, uint32_t index, T value) { //Добавление элемента по индексу
    if (ar.size + 1 > ar.capacity) {
        doubleArray(ar);
    }
    if (index >= 0 && index < ar.capacity) {
        if (index < ar.size) {
            for (uint32_t j = ar.size - 1; j >= index && j < ar.size; j--) {
                ar.data[j + 1] = ar.data[j];
            }
        }
        ar.data[index] = value;
        ar.size++;
    }
    else {
        cout << "Error: Index out of bounds" << endl;
    }
}

template <typename T>
T MGET_BY_IND(Array<T>& ar, uint32_t index) { //Получение элемента по индексу
    if (index >= 0 && index < ar.size)
        return ar.data[index];
    else {
        cout << "Error: Index out of bounds" << endl;
        return string("0");
    }
}

template <typename T>
void MDEL_BY_IND(Array<T>& ar, uint32_t index) {
    if (index >= 0 && index < ar.size) {
        for (uint32_t i = index; i < ar.size; i++) {
            ar.data[i] = ar.data[i + 1];
        }
        ar.size--;
    }
    else {
        cout << "Error: Index out of bounds" << endl;
    }
}

template <typename T>
void MSWAP_BY_IND(Array<T>& ar, uint32_t index, T value) {
    if (index >= 0 && index < ar.size) {
        ar.data[index] = value;
    }
    else {
        cout << "Error: Index out of bounds" << endl;
    }
}

template <typename T>
void PRINT(Array<T>& ar) {
    for (uint32_t i = 0; i < ar.size; i++) {
        cout << ar.data[i] << " ";
    }
    cout << endl;
}

// Сохранение массива в файл
template <typename T>
void MSAVE(const Array<T>& ar, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для записи!" << endl;
        return;
    }
    file << ar.size << endl;
    for (uint32_t i = 0; i < ar.size; i++) {
        file << ar.data[i] << " ";
    }
    file.close();
    cout << "Массив сохранён в файл: " << filename << endl;
}

// Загрузка массива из файла
template <typename T>
void MLOAD(Array<T>& ar, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return;
    uint32_t size;
    file >> size;
    ar.size = 0;
    T value;
    while (file >> value && ar.size < size) {
        MPUSH_BACK(ar, value);
    }
    file.close();
    cout << "Массив загружен из файла: " << filename << endl;
}