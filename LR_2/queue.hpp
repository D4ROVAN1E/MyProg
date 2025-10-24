#pragma once
#include <iostream>
#include <cstdint>
#include <stdexcept> // Для генерации исключений
#include <fstream>
#include <sstream>

using namespace std;

template <typename T>
struct Queue {
    T* data;         // Указатель на динамический массив для хранения данных
    uint32_t capacity; // Общая вместимость массива
    uint32_t size;     // Текущее количество элементов в очереди

    uint32_t head;     // Индекс "головы" очереди (первого элемента для извлечения)
    uint32_t tail;     // Индекс для вставки следующего элемента ("хвост")

    Queue() {
        capacity = 1;
        size = 0;
        head = 0;
        tail = 0;
        data = new T[capacity];
        for (uint32_t i = 0; i < capacity; i++) {
            data[i] = T();
        }
    }

    // Конструктор: инициализирует очередь с заданной вместимостью
    Queue(const uint32_t cap) {
        capacity = cap > 0 ? cap : 1; // Минимальная вместимость - 1
        size = 0;
        head = 0;
        tail = 0;
        data = new T[capacity];
        for (uint32_t i = 0; i < capacity; i++) {
            data[i] = T();
        }
    }

    // Деструктор: освобождает выделенную память
    ~Queue() {
        delete[] data;
    }

    //Копирующий конструктор
    Queue(const Queue<T>& other) {
        capacity = other.capacity;
        size = other.size;
        head = other.head;
        tail = other.tail;
        data = new T[capacity];
        for (uint32_t i = 0; i < capacity; i++) {
            data[i] = other.data[i];
        }
    }

    //Копирующий оператор присваивания
    Queue<T>& operator=(const Queue<T>& other) {
        if (this == &other) {
            return *this;
        }

        delete[] data; // Освобождаем старую память
        capacity = other.capacity;
        size = other.size;
        head = other.head;
        tail = other.tail;
        data = new T[capacity];
        for (uint32_t i = 0; i < capacity; i++) {
            data[i] = other.data[i];
        }
        return *this;
    }

    bool empty() const {
        return size == 0;
	}

};

// Вспомогательная функция для расширения массива 
template <typename T>
void resize(Queue<T>& q) {
    uint32_t newCapacity = q.capacity * 2;
    T* newData = new T[newCapacity];

    // Копируем элементы из старого массива в новый, "распрямляя" кольцевой буфер
    for (uint32_t i = 0; i < q.size; ++i) {
        newData[i] = q.data[(q.head + i) % q.capacity];
    }

    delete[] q.data; // Освобождаем старую память

    q.data = newData;
    q.capacity = newCapacity;
    q.head = 0;       // Голова теперь в начале нового массива
    q.tail = q.size;    // Хвост следует за последним элементом
}

// Добавление элемента в конец очереди
template <typename T>
void QPUSH(Queue<T>& q, T value) {
    if (q.size >= q.capacity) {
        resize(q); // Если места нет, расширяем массив
    }
    q.data[q.tail] = value;
    q.tail = (q.tail + 1) % q.capacity; // Сдвигаем хвост по кругу
    q.size++;
}

// Извлечение элемента из начала очереди
template <typename T>
T QPOP(Queue<T>& q) {
    if (q.size == 0) {
        throw out_of_range("Очередь пуста!");
    }
    T value = q.data[q.head];
    q.head = (q.head + 1) % q.capacity; // Сдвигаем голову по кругу
    q.size--;
    return value;
}

// Получение первого элемента без его извлечения 
template <typename T>
T QGET(Queue<T>& q) {
    if (q.size == 0) {
        throw out_of_range("Очередь пуста!");
    }
    return q.data[q.head];
}

// Функция для печати содержимого очереди (для демонстрации)
template <typename T>
void PRINT(Queue<T>& q) {
    cout << "Содержимое очереди (от головы к хвосту): ";
    if (q.size == 0) {
        cout << "пусто";
    }
    else {
        for (uint32_t i = 0; i < q.size; ++i) {
            cout << q.data[(q.head + i) % q.capacity] << " ";
        }
    }
    cout << endl;
}

// Сохранение очереди в файл
template <typename T>
void QSAVE(const Queue<T>& q, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для записи!" << endl;
        return;
    }
    file << q.size << endl;
    for (uint32_t i = 0; i < q.size; i++) {
        file << q.data[(q.head + i) % q.capacity] << " ";
    }
    file.close();
    cout << "Очередь сохранена в файл: " << filename << endl;
}

// Загрузка очереди из файла
template <typename T>
void QLOAD(Queue<T>& q, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return;
    }
    // Очищаем текущую очередь
    q.size = 0;
    q.head = 0;
    q.tail = 0;

    uint32_t size;
    file >> size;
    T value;
    while (file >> value && q.size < size) {
        QPUSH(q, value);
    }
    file.close();
    cout << "Очередь загружена из файла: " << filename << endl;
}