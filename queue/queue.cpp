#include <iostream>
#include <cstdint>
#include <stdexcept> // Для генерации исключений

using namespace std;

template <typename T>
struct Queue {
    T* data;         // Указатель на динамический массив для хранения данных
    uint32_t capacity; // Общая вместимость массива
    uint32_t size;     // Текущее количество элементов в очереди

    uint32_t head;     // Индекс "головы" очереди (первого элемента для извлечения)
    uint32_t tail;     // Индекс для вставки следующего элемента ("хвост")

    // Конструктор: инициализирует очередь с заданной вместимостью
    Queue(const uint32_t cap) {
        capacity = cap > 0 ? cap : 1; // Минимальная вместимость - 1
        size = 0;
        head = 0;
        tail = 0;
        data = new T[capacity];
    }

    // Деструктор: освобождает выделенную память
    ~Queue() {
        delete[] data;
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

// Добавление элемента в конец очереди (enqueue)
template <typename T>
void enqueue(Queue<T>& q, T value) {
    if (q.size >= q.capacity) {
        resize(q); // Если места нет, расширяем массив
    }
    q.data[q.tail] = value;
    q.tail = (q.tail + 1) % q.capacity; // Сдвигаем хвост по кругу
    q.size++;
}

// Извлечение элемента из начала очереди (dequeue)
template <typename T>
T dequeue(Queue<T>& q) {
    if (q.size == 0) {
        throw out_of_range("Очередь пуста!");
    }
    T value = q.data[q.head];
    q.head = (q.head + 1) % q.capacity; // Сдвигаем голову по кругу
    q.size--;
    return value;
}

// Получение первого элемента без его извлечения (peek/front)
template <typename T>
T front(Queue<T>& q) {
    if (q.size == 0) {
        throw out_of_range("Очередь пуста!");
    }
    return q.data[q.head];
}

// Проверка, пуста ли очередь
template <typename T>
bool isEmpty(Queue<T>& q) {
    return q.size == 0;
}

// Получение текущего размера очереди
template <typename T>
uint32_t getSize(Queue<T>& q) {
    return q.size;
}


// Функция для печати содержимого очереди (для демонстрации)
template <typename T>
void printQueue(Queue<T>& q) {
    cout << "Содержимое очереди (от головы к хвосту): ";
    if (isEmpty(q)) {
        cout << "пусто";
    }
    else {
        for (uint32_t i = 0; i < getSize(q); ++i) {
            cout << q.data[(q.head + i) % q.capacity] << " ";
        }
    }
    cout << endl;
}


int main() {
    setlocale(LC_ALL, "ru");

    cout << "Создаем очередь с начальной вместимостью 3..." << endl;
    Queue<int> myQueue(3);

    cout << "Добавляем элементы: 10, 20, 30" << endl;
    enqueue(myQueue, 10);
    enqueue(myQueue, 20);
    enqueue(myQueue, 30);
    printQueue(myQueue);
    cout << "Размер: " << getSize(myQueue) << ", Вместимость: " << myQueue.capacity << endl;
    cout << "------------------------------------" << endl;

    cout << "Добавляем элемент 40. Это вызовет расширение массива." << endl;
    enqueue(myQueue, 40);
    printQueue(myQueue);
    cout << "Размер: " << getSize(myQueue) << ", Вместимость: " << myQueue.capacity << endl;
    cout << "------------------------------------" << endl;

    cout << "Извлекаем два элемента:" << endl;
    cout << "Извлечено: " << dequeue(myQueue) << endl;
    cout << "Извлечено: " << dequeue(myQueue) << endl;
    printQueue(myQueue);
    cout << "Первый в очереди сейчас: " << front(myQueue) << endl;
    cout << "Размер: " << getSize(myQueue) << endl;
    cout << "------------------------------------" << endl;

    cout << "Добавляем еще три элемента: 50, 60, 70" << endl;
    enqueue(myQueue, 50);
    enqueue(myQueue, 60);
    enqueue(myQueue, 70);
    printQueue(myQueue);
    cout << "Размер: " << getSize(myQueue) << ", Вместимость: " << myQueue.capacity << endl;
    cout << "------------------------------------" << endl;

    return 0;
}