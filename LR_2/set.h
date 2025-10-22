#pragma once
#include "array.h"
#include "singly_list.h"
#include <cmath>
#include <string>

using namespace std;

template <typename T>
struct HashSet {
    Array<ForwardList<T>> table;
    uint32_t elementCount;
    const double A = (sqrt(5.0) - 1.0) / 2.0; // Константа для хэш-функции

    HashSet(uint32_t initialSize = 16) {
        table = Array<ForwardList<T>>(initialSize);
        elementCount = 0;
        // Инициализируем каждую ячейку пустым списком
        for (uint32_t i = 0; i < table.capacity; i++) {
            table.data[i] = ForwardList<T>();
        }
        table.size = table.capacity;
    }

    // Копирующий конструктор
    HashSet(const HashSet<T>& other) {
        table = Array<ForwardList<T>>(other.table.capacity);
        elementCount = other.elementCount;

        // Копируем каждый список из другого множества
        for (uint32_t i = 0; i < other.table.size; i++) {
            table.data[i] = other.table.data[i]; // Используем копирующий оператор ForwardList
        }
        table.size = other.table.size;
    }

    // Копирующий оператор присваивания
    HashSet<T>& operator=(const HashSet<T>& other) {
        if (this == &other) {
            return *this;
        }

        // Создаём новую таблицу
        table = Array<ForwardList<T>>(other.table.capacity);
        elementCount = other.elementCount;

        // Копируем каждый список
        for (uint32_t i = 0; i < other.table.size; i++) {
            table.data[i] = other.table.data[i];
        }
        table.size = other.table.size;

        return *this;
    }

    ~HashSet() {
        // Деструкторы Array и ForwardList очистят память автоматически
    }
};

// Хэш-функция методом умножения
template <typename T>
uint32_t hashFunction(const HashSet<T>& set, const T& key) {
    // Преобразуем ключ в число
    uint64_t k = 0;
    if constexpr (is_arithmetic_v<T>) {
        k = static_cast<uint64_t>(key);
    }
    else {
        // Для строк вычисляем хэш
        string str = key;
        for (char c : str) {
            k = k * 31 + static_cast<uint64_t>(c);
        }
    }

    // Применяем метод умножения: hash(k) = floor(M * ((k * A) mod 1))
    double temp = k * set.A;
    double fractional = temp - floor(temp); // (k * A) mod 1
    uint32_t hash = static_cast<uint32_t>(floor(set.table.size * fractional));

    return hash % set.table.size;
}

// Проверка необходимости расширения таблицы
template <typename T>
void checkAndResize(HashSet<T>& set) {
    // Если коэффициент загрузки > 0.75, увеличиваем таблицу
    if (set.elementCount > set.table.size * 0.75) {
        uint32_t oldSize = set.table.size;
        Array<ForwardList<T>> oldTable = set.table;

        // Создаём новую таблицу удвоенного размера
        set.table = Array<ForwardList<T>>(oldSize * 2);
        for (uint32_t i = 0; i < set.table.capacity; i++) {
            set.table.data[i] = ForwardList<T>();
        }
        set.table.size = set.table.capacity;
        set.elementCount = 0;

        // Перехэшируем все элементы
        for (uint32_t i = 0; i < oldSize; i++) {
            SNode<T>* current = oldTable.data[i].head;
            while (current != nullptr) {
                uint32_t newHash = hashFunction(set, current->key);
                FPUSH_BACK(set.table.data[newHash], current->key);
                set.elementCount++;
                current = current->next;
            }
        }
    }
}

// Добавление элемента в множество
template <typename T>
void SETADD(HashSet<T>& set, const T& key) {
    uint32_t hash = hashFunction(set, key);

    // Проверяем, есть ли уже такой элемент
    SNode<T>* found = FGET_BY_VALUE(set.table.data[hash], key);
    if (found != nullptr) {
        return; // Элемент уже существует, не добавляем дубликат
    }

    // Добавляем элемент в соответствующий список
    FPUSH_BACK(set.table.data[hash], key);
    set.elementCount++;

    // Проверяем необходимость расширения
    checkAndResize(set);
}

// Удаление элемента из множества
template <typename T>
void SETDEL(HashSet<T>& set, const T& key) {
    uint32_t hash = hashFunction(set, key);

    // Проверяем наличие элемента
    SNode<T>* found = FGET_BY_VALUE(set.table.data[hash], key);
    if (found == nullptr) {
        return; // Элемент не найден
    }

    // Удаляем элемент из списка
    FDEL_BY_VALUE(set.table.data[hash], key);
    set.elementCount--;
}

// Проверка наличия элемента в множестве
template <typename T>
bool SET_AT(HashSet<T>& set, const T& key) {
    uint32_t hash = hashFunction(set, key);
    SNode<T>* found = FGET_BY_VALUE(set.table.data[hash], key);
    return found != nullptr;
}

// Вывод множества (для отладки)
template <typename T>
void PRINT(const HashSet<T>& set) {
    cout << "HashSet (элементов: " << set.elementCount << "):" << endl;
    for (uint32_t i = 0; i < set.table.size; i++) {
        if (set.table.data[i].head != nullptr) {
            cout << "Bucket " << i << ": ";
            PRINT(set.table.data[i]);
        }
    }
}

// Сохранение множества в файл
template <typename T>
void SETSAVE(const HashSet<T>& set, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для записи!" << endl;
        return;
    }

    // Сохраняем размер таблицы и количество элементов
    file << set.table.size << " " << set.elementCount << endl;

    // Сохраняем все элементы
    for (uint32_t i = 0; i < set.table.size; i++) {
        SNode<T>* current = set.table.data[i].head;
        while (current != nullptr) {
            file << current->key << " ";
            current = current->next;
        }
    }

    file.close();
    cout << "Множество сохранено в файл: " << filename << endl;
}

// Загрузка множества из файла
template <typename T>
void SETLOAD(HashSet<T>& set, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для чтения!" << endl;
        return;
    }

    uint32_t tableSize, elemCount;
    file >> tableSize >> elemCount;

    // Создаём новое множество с нужным размером
    set = HashSet<T>(tableSize);

    // Читаем и добавляем элементы
    T value;
    while (file >> value) {
        SETADD(set, value);
    }

    file.close();
    cout << "Множество загружено из файла: " << filename << endl;
}