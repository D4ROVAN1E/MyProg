#include <iostream>
#include <cstdint>

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
            data[i] = T();
        }
    }

    Array(const uint32_t cap) { //Конструктор
        size = capacity = cap - 1;
        data = new T[cap];
        for (uint32_t i = 0; i < cap; i++) {
            data[i] = T();
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
            for (uint32_t j = ar.size - 1; j > index; j--) {
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
        return NULL;
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
void readArray(Array<T>& ar) {
    for (uint32_t i = 0; i < ar.size; i++) {
        cout << ar.data[i] << " ";
    }
    cout << endl;
}

int main()
{
    setlocale(LC_ALL, "ru");
    Array<int> test(5);
    MPUSH_BY_IND(test, 2, 11);
    MPUSH_BACK(test, 12);
    MPUSH_BACK(test, 22);
    MPUSH_BACK(test, 23);
    MPUSH_BACK(test, 24);
	//MDEL_BY_IND(test, 2);
    readArray(test);
    cout << test.size;
    return 0;
}

