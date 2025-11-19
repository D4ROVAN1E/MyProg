#ifndef QUEUE_HPP
#define QUEUE_HPP
#include <iostream>
#include <cstdint>
#include <stdexcept>  // Для генерации исключений
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

template <typename T>
class Queue {
 private:        // Указатель на динамический массив для хранения данных
    uint32_t capacity;  // Общая вместимость массива
    uint32_t size;
    T* data;     // Текущее количество элементов в очереди

    uint32_t head;  // Индекс "головы" очереди (первого элемента для извлечения)
    uint32_t tail;  // Индекс для вставки следующего элемента ("хвост")

    // Вспомогательная функция для расширения массива
    void resize() {
        uint32_t newCapacity = capacity * 2;
        T* newData = new T[newCapacity];

        // Копируем элементы из старого массива в новый,
        // "распрямляя" кольцевой буфер
        for (uint32_t i = 0; i < size; ++i) {
            newData[i] = data[(head + i) % capacity];
        }

        delete[] data;  // Освобождаем старую память

        data = newData;
        capacity = newCapacity;
        head = 0;       // Голова теперь в начале нового массива
        tail = size;    // Хвост следует за последним элементом
    }

 public:
    Queue() : capacity(1)
            , size(0)
            , head(0)
            , tail(0)
            , data(new T[1]) {
        for (uint32_t i = 0; i < capacity; i++) {
            data[i] = T();
        }
    }

    // Конструктор: инициализирует очередь с заданной вместимостью
    explicit Queue(const uint32_t cap) : size(0)
                                , capacity(cap > 0 ? cap : 1)
                                , head(0)
                                , tail(0)
                                , data(new T[cap > 0 ? cap : 1]) {
        for (uint32_t i = 0; i < capacity; i++) {
            data[i] = T();
        }
    }

    // Деструктор: освобождает выделенную память
    ~Queue() {
        delete[] data;
    }

    // Копирующий конструктор
    Queue(const Queue<T>& other) : capacity(other.capacity)
                                    , size(other.size)
                                    , head(other.head)
                                    , tail(other.tail)
                                    , data(new T[other.capacity]) {
        for (uint32_t i = 0; i < capacity; i++) {
            data[i] = other.data[i];
        }
    }

    // Копирующий оператор присваивания
    auto operator=(const Queue<T>& other) -> Queue<T>& {
        if (this == &other) {
            return *this;
        }

        delete[] data;  // Освобождаем старую память
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

    // Добавление элемента в конец очереди
    void QPUSH(T value) {
        if (size >= capacity) {
            resize();  // Если места нет, расширяем массив
        }
        data[tail] = value;
        tail = (tail + 1) % capacity;  // Сдвигаем хвост по кругу
        size++;
    }

    // Извлечение элемента из начала очереди
    auto QPOP() -> T {
        if (size == 0) {
            throw out_of_range("Очередь пуста!");
        }
        T value = data[head];
        head = (head + 1) % capacity;  // Сдвигаем голову по кругу
        size--;
        return value;
    }

    // Получение первого элемента без его извлечения
    auto QGET() const -> T {
        if (size == 0) {
            throw out_of_range("Очередь пуста!");
        }
        return data[head];
    }

    // Функция для печати содержимого очереди (для демонстрации)
    void PRINT() const {
        cout << "Содержимое очереди (от головы к хвосту): ";
        if (size == 0) {
            cout << "пусто";
        } else {
            for (uint32_t i = 0; i < size; ++i) {
                cout << data[(head + i) % capacity] << " ";
            }
        }
        cout << endl;
    }

    // Сохранение очереди в файл
    void QSAVE(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Ошибка открытия файла для записи!" << endl;
            return;
        }
        file << size << endl;
        for (uint32_t i = 0; i < size; i++) {
            file << data[(head + i) % capacity] << " ";
        }
        file.close();
        cout << "Очередь сохранена в файл: " << filename << endl;
    }

    // Загрузка очереди из файла
    void QLOAD(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            return;
        }
        // Очищаем текущую очередь
        size = 0;
        head = 0;
        tail = 0;

        uint32_t NewSize;
        file >> NewSize;
        T value;
        while (file >> value && size < NewSize) {
            QPUSH(value);
        }
        file.close();
        cout << "Очередь загружена из файла: " << filename << endl;
    }

    // Сохранение очереди в бинарный файл
    void QSAVE_BINARY(const string& filename) const {
        ofstream file(filename, ios::binary);
        if (!file.is_open()) {
            cout << "Ошибка открытия файла для бинарной записи!" << endl;
            return;
        }

        // Записываем количество элементов (size)
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));

        // Записываем сами элементы
        // идем логически от head до tail, чтобы сохранить порядок
        for (uint32_t i = 0; i < size; i++) {
            const T& item = data[(head + i) % capacity];
            file.write(reinterpret_cast<const char*>(&item), sizeof(T));
        }

        file.close();
        cout << "Очередь сохранена (bin): " << filename << endl;
    }

    // Загрузка очереди из бинарного файла
    void QLOAD_BINARY(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            cout << "Ошибка открытия бинарного файла для чтения!" << endl;
            return;
        }

        // Сбрасываем текущее состояние очереди
        size = 0;
        head = 0;
        tail = 0;
        // capacity не сбрасываем, будем расширять при необходимости через QPUSH

        uint32_t newSize = 0;
        // Считываем количество элементов
        file.read(reinterpret_cast<char*>(&newSize), sizeof(newSize));

        // Считываем элементы один за другим и добавляем в очередь
        for (uint32_t i = 0; i < newSize; ++i) {
            T value;
            // Считываем сырые байты в переменную value
            file.read(reinterpret_cast<char*>(&value), sizeof(T));
            
            // Если файл оборван или данные повреждены, прекращаем чтение
            if (!file) {
                cout << "Ошибка чтения данных (файл поврежден или короче ожидаемого)." << endl;
                break;
            }
            
            QPUSH(value);
        }

        file.close();
        cout << "Очередь загружена (bin): " << filename << endl;
    }

    [[nodiscard]] auto empty() const -> bool {
        return size == 0;
    }

    [[nodiscard]] auto GetSize() const -> uint32_t {
        return size;
    }
};

#endif  // QUEUE_HPP
