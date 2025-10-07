#pragma once
#include <iostream>
#include <cstdint>
#include <fstream>
#include <sstream>

using namespace std;

template <typename T>
struct Stack {
    T* data;
    uint32_t capacity;
    uint32_t size;

    Stack() {
        size = 0;
        capacity = 1;
        data = new T[capacity];
        for (uint32_t i = 0; i < capacity; i++) {
            data[i] = T();
        }
    }

    Stack(const uint32_t cap) { //Конструктор (инициализация и создание)
        capacity = cap;
        size = 0;
        data = new T[capacity];
        for (uint32_t i = 0; i < capacity; i++) {
            data[i] = T();
        }
    }

    ~Stack() { //Деструктор (очищает помять когда выходим из области видимости с стеком)
        delete[] data;
    }

    Stack(const Stack<T>& other) { //Копирующий конструктор (Если создаём новый стек на основе предыдущего)
        capacity = other.capacity;
        size = other.size;
        data = new T[capacity];
        for (uint32_t i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    Stack<T>& operator=(const Stack<T>& other) { //Копирующий оператор присваивания (Если имеем два стека с данными и присваиваем один другому)
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
void doubleStack(Stack<T>& st) { //Удвоение стека при достижении лимита capacity
    uint32_t cap = st.capacity;
    T* newData = new T[cap * 2];
    st.capacity = cap * 2;
    for (uint32_t i = 0; i < st.size; i++) {
        newData[i] = st.data[i];
    }
    delete[] st.data;
    st.data = newData;
}

template <typename T>
void SPUSH(Stack<T>& st, T value) { //Добавление элемента в конец стека
    if (st.size >= st.capacity) {
        doubleStack(st);
    }
    st.data[st.size++] = value;
}

template <typename T>
void SPOP(Stack<T>& st) {
    st.size--;
}


template <typename T>
void readStack(Stack<T>& st) {
    for (uint32_t i = 0; i < st.size; i++) {
        cout << st.data[i] << " ";
    }
    cout << endl;
}

// Сохранение стека в файл
template <typename T>
void saveStackToFile(const Stack<T>& st, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для записи!" << endl;
        return;
    }
    file << st.size << endl;
    for (uint32_t i = 0; i < st.size; i++) {
        file << st.data[i] << " ";
    }
    file.close();
    cout << "Стек сохранён в файл: " << filename << endl;
}

// Загрузка стека из файла
template <typename T>
void loadStackFromFile(Stack<T>& st, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для чтения!" << endl;
        return;
    }
    uint32_t size;
    file >> size;
    st.size = 0;
    T value;
    while (file >> value && st.size < size) {
        SPUSH(st, value);
    }
    file.close();
    cout << "Стек загружен из файла: " << filename << endl;
}
