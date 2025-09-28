#include <iostream>
#include <cstdint>

using namespace std;

template <typename T>
struct Stack {
    T* data;
    uint32_t capacity;
    uint32_t size;

    Stack(const uint32_t cap) { //Конструктор (инициализация и создание)
        capacity = cap;
        size = 0;
        data = new T[cap];
        for (uint32_t i = 0; i < cap; i++) {
            data[i] = 0;
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
void doubleStack(Stack<T>& ar) { //Удвоение стека при достижении лимита capacity
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
void SPUSH(Stack<T>& ar, T value) { //Добавление элемента в конец стека
    if (ar.size >= ar.capacity) {
        doubleStack(ar);
    }
    ar.data[ar.size++] = value;
}

template <typename T>
void SPOP(Stack<T>& ar) {
    ar.size--;
}


template <typename T>
void readStack(Stack<T>& ar) {
    for (uint32_t i = 0; i < ar.size; i++) {
        cout << ar.data[i] << " ";
    }
    cout << endl;
}

int main()
{
    setlocale(LC_ALL, "ru");
    Stack<int> test(12);
    SPUSH(test, 10);
    SPUSH(test, 20);
    SPUSH(test, 30);
    SPUSH(test, 40);
    readStack(test);
    SPOP(test);
    SPOP(test);
    SPOP(test);
    readStack(test);
    return 0;
}

