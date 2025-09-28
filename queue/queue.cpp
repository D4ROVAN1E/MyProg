#include <iostream>
#include <cstdint>

using namespace std;

template <typename T>
struct Queue {
    T* data;
    uint32_t capacity;
    uint32_t size;

    Queue(const uint32_t cap) { //Конструктор (инициализация и создание)
        capacity = cap;
        size = 0;
        data = new T[cap];
        for (uint32_t i = 0; i < cap; i++) {
            data[i] = 0;
        }
    }

    ~Queue() { //Деструктор (очищает помять когда выходим из области видимости с массивом)
        delete[] data;
    }

    Queue(const Queue<T>& other) { //Копирующий конструктор (Если создаём новый массив на основе предыдущего)
        capacity = other.capacity;
        size = other.size;
        data = new T[capacity];
        for (uint32_t i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    Queue<T>& operator=(const Queue<T>& other) { //Копирующий оператор присваивания (Если имеем два массива с данными и присваиваем один другому)
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
void doubleQueue(Queue<T>& ar) { //Удвоение массива при достижении лимита capacity
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
void QPUSH_BACK(Queue<T>& ar, T value) { //Добавление элемента в конец массива
    if (ar.size >= ar.capacity) {
        doubleQueue(ar);
    }
    ar.data[ar.size++] = value;
}

template <typename T>
void QPUSH_BY_IND(Queue<T>& ar, uint32_t index, T value) { //Добавление элемента по индексу
    if (ar.size >= ar.capacity) {
        doubleQueue(ar);
    }
    if (index >= 0 && index < ar.capacity) {
        if (index < ar.size) {
            for (uint32_t j = ar.size - 1; j >= index; j--) {
                ar.data[j + 1] = ar.data[j];
            }
        }
        ar.data[index] = value;
        ar.size = ++index;
    }
}

template <typename T>
T QGET_BY_IND(Queue<T>& ar, uint32_t index) { //Получение элемента по индексу
    if (index >= 0 && index < ar.size)
        return ar.data[index];
}

template <typename T>
void QDEL_BY_IND(Queue<T>& ar, uint32_t index) {
    if (index >= 0 && index < ar.size) {
        for (uint32_t i = index; i < ar.size; i++) {
            ar.data[i] = ar.data[i + 1];
        }
        ar.size--;
    }
}

template <typename T>
void QSWAP_BY_IND(Queue<T>& ar, uint32_t index, T value) {
    if (index >= 0 && index < ar.size) {
        ar.data[index] = value;
    }
}


template <typename T>
void readQueue(Queue<T>& ar) {
    for (uint32_t i = 0; i < ar.size; i++) {
        cout << ar.data[i] << " ";
    }
    cout << endl;
}

int main()
{
    setlocale(LC_ALL, "ru");
    Queue<int> test(12);
    QPUSH_BY_IND(test, 2, 11);
    QPUSH_BACK(test, 12);
    readQueue(test);
    cout << test.size;
    return 0;
}

