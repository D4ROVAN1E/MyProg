#pragma once
#include "array.hpp"
#include "singly_list.hpp"
#include <cmath>
#include <string>
#include <fstream>
#include <iostream> 
#include <type_traits>

using namespace std;

template <typename T>
class HashSet {
private:
    Array<ForwardList<T>> table;
    uint32_t elementCount;
    const double A = (sqrt(5.0) - 1.0) / 2.0; // Константа для хэш-функции

    // Хэш-функция методом умножения
    uint32_t hashFunction(const T& key) {
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
        double temp = k * A;
        temp = temp - floor(temp); // (k * A) mod 1

        return static_cast<uint32_t>(floor(table.size * temp));
    }

    // Проверка необходимости расширения таблицы
    void checkAndResize() {
        // Если коэффициент загрузки > 0.75, увеличиваем таблицу
        if (elementCount > table.size * 0.75) {
            uint32_t oldSize = table.size;
            Array<ForwardList<T>> oldTable = table;

            // Создаём новую таблицу удвоенного размера
            table = Array<ForwardList<T>>(oldSize * 2);
            for (uint32_t i = 0; i < table.capacity; i++) {
                table.data[i] = ForwardList<T>();
            }
            table.size = table.capacity;
            elementCount = 0;

            // Перехэшируем все элементы
            for (uint32_t i = 0; i < oldSize; i++) {
                SNode<T>* current = oldTable.data[i].head;
                while (current != nullptr) {
                    uint32_t newHash = hashFunction(current->key);
                    FPUSH_BACK(table.data[newHash], current->key);
                    elementCount++;
                    current = current->next;
                }
            }
        }
    }

public:
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

    uint32_t GetSize() {
        return elementCount;
    }

    // Добавление элемента в множество
    void SETADD(const T& key) {
        uint32_t hash = hashFunction(key);

        // Проверяем, есть ли уже такой элемент
        SNode<T>* found = FGET_BY_VALUE(table.data[hash], key);
        if (found != nullptr) {
            return; // Элемент уже существует, не добавляем дубликат
        }

        // Добавляем элемент в соответствующий список
        FPUSH_BACK(table.data[hash], key);
        elementCount++;

        // Проверяем необходимость расширения
        checkAndResize();
    }

    // Удаление элемента из множества
    void SETDEL(const T& key) {
        uint32_t hash = hashFunction(key);

        // Проверяем наличие элемента
        SNode<T>* found = FGET_BY_VALUE(table.data[hash], key);
        if (found == nullptr) {
            return; // Элемент не найден
        }

        // Удаляем элемент из списка
        FDEL_BY_VALUE(table.data[hash], key);
        elementCount--;
    }

    // Проверка наличия элемента в множестве
    bool SET_AT(const T& key) {
        uint32_t hash = hashFunction(key);
        SNode<T>* found = FGET_BY_VALUE(table.data[hash], key);
        return found != nullptr;
    }

    // Сохранение множества в файл
    void SETSAVE(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            return;
        }

        // Сохраняем размер таблицы и количество элементов
        file << table.size << " " << elementCount << endl;

        // Сохраняем все элементы
        for (uint32_t i = 0; i < table.size; i++) {
            SNode<T>* current = table.data[i].head;
            while (current != nullptr) {
                file << current->key << " ";
                current = current->next;
            }
        }

        file.close();
        cout << "Множество сохранено в файл: " << filename << endl;
    }

    // Загрузка множества из файла
    void SETLOAD(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            return;
        }

        uint32_t tableSize, elemCount;
        file >> tableSize >> elemCount;

        // Читаем и добавляем элементы
        T value;
        while (file >> value) {
            SETADD(value);
        }

        file.close();
        cout << "Множество загружено из файла: " << filename << endl;
    }

    // Вывод множества
    void PRINT() {
        cout << "HashSet (элементов: " << elementCount << "):" << endl;
        for (uint32_t i = 0; i < table.size; i++) {
            if (table.data[i].head != nullptr) {
                cout << "Bucket " << i << ": ";
                ::PRINT(table.data[i]);
            }
        }
    }

};