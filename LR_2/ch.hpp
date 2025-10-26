#pragma once
#include <iostream>
#include <cstdint>
#include <cmath>
#include <string>
#include <algorithm> 
#include "array.hpp"

using namespace std;

//Структура для хранения пары ключ-значение 
template <typename T>
struct HashNode {
    string key;
    T value;
    bool isOccupied;  //Занята ли ячейка

    HashNode() : key(""), value(T()), isOccupied(false) {}

    HashNode(const string& k, const T& v)
        : key(k), value(v), isOccupied(true) {
    }
};

template <typename T>
class CuckooHash {
private:
    Array<HashNode<T>> table;
    uint32_t tableSize;        //Размер таблицы
    uint32_t elementsCount;    //Количество элементов
    const double A = (sqrt(5.0) - 1.0) / 2.0; //Дробная часть золотого сечения

    //Первая хэш-функция
    uint32_t hash1(const string& key) const {
        uint64_t numKey = 0;
        for (char c : key) {
            numKey = numKey * 31 + static_cast<uint64_t>(c);
        }
        double temp = numKey * A;
        temp = temp - floor(temp);
        return static_cast<uint32_t>(floor(tableSize * temp));
    }

    //Вторая хэш-функция
    uint32_t hash2(const string& key) const {
        uint32_t sum = 0;
        for (char c : key) {
            sum += static_cast<uint8_t>(c);
        }
        uint32_t result = (sum % (tableSize - 1)) + 1;
        if (tableSize % 2 == 0 && result % 2 == 0) {
            result++;
        }
        return result;
    }

    //Функция для проверки необходимости расширения таблицы
    //Для Cuckoo Hashing нужен более низкий коэффициент загрузки. Поставим 0.5.
    bool needResize() const {
        return (static_cast<double>(elementsCount) / tableSize) > 0.5;
    }

    //Расширение таблицы 
    void resize() {
        uint32_t oldSize = tableSize;
        Array<HashNode<T>> oldTable = table;

        tableSize = tableSize * 2 + 1;

        table = Array<HashNode<T>>(tableSize + 1);
        for (uint32_t i = 0; i < tableSize; i++) {
            table.data[i] = HashNode<T>();
        }
        table.size = tableSize;

        elementsCount = 0;

        //Перехэшируем все элементы
        for (uint32_t i = 0; i < oldSize; i++) {
            if (oldTable.data[i].isOccupied) {
                insert(oldTable.data[i].key, oldTable.data[i].value);
            }
        }
    }

public:
    //Конструктор 
    CuckooHash(uint32_t size = 3) {
        tableSize = size;
        elementsCount = 0;
        table = Array<HashNode<T>>(tableSize + 1);
        for (uint32_t i = 0; i < tableSize; i++) {
            table.data[i] = HashNode<T>();
        }
        table.size = tableSize;
    }

    //Деструктор 
    ~CuckooHash() {
        //Array имеет свой деструктор
    }

    //Копирующий конструктор 
    CuckooHash(const CuckooHash<T>& other) {
        tableSize = other.tableSize;
        elementsCount = other.elementsCount;
        table = Array<HashNode<T>>(tableSize + 1);
        for (uint32_t i = 0; i < tableSize; i++) {
            table.data[i] = other.table.data[i];
        }
        table.size = tableSize;
    }

    //Копирующий оператор присваивания 
    CuckooHash<T>& operator=(const CuckooHash<T>& other) {
        if (this == &other) {
            return *this;
        }
        tableSize = other.tableSize;
        elementsCount = other.elementsCount;
        table = Array<HashNode<T>>(tableSize + 1);
        for (uint32_t i = 0; i < tableSize; i++) {
            table.data[i] = other.table.data[i];
        }
        table.size = tableSize;
        return *this;
    }

    //Вставка элемента
    void insert(const string& key, const T& value) {
        //Проверяем, существует ли ключ, и обновляем его
        uint32_t h1 = hash1(key);
        if (table.data[h1].isOccupied && table.data[h1].key == key) {
            table.data[h1].value = value;
            return;
        }
        uint32_t h2 = hash2(key);
        if (table.data[h2].isOccupied && table.data[h2].key == key) {
            table.data[h2].value = value;
            return;
        }

        //Если ключа нет, проверяем resize
        if (needResize()) {
            resize();
        }

        //Начинаем Cuckoo-вставку
        HashNode<T> currentItem(key, value);
        uint32_t currentPos = h1;

        //Ограничиваем количество "выталкиваний", чтобы избежать бесконечного цикла.
        for (uint32_t i = 0; i < tableSize * 2; i++) {

            //Если ячейка свободна 
            if (!table.data[currentPos].isOccupied) {
                table.data[currentPos] = currentItem;
                elementsCount++;
                return;
            }

            //Ячейка занята. "Выталкиваем" (меняем)
            swap(currentItem, table.data[currentPos]);

            //Теперь 'currentItem' - это "вытолкнутый" элемент.
            uint32_t pos1 = hash1(currentItem.key);
            uint32_t pos2 = hash2(currentItem.key);

            currentPos = (currentPos == pos1) ? pos2 : pos1;
        }

        //Если мы вышли из цикла, значит, обнаружили цикл.
        resize();
        //После resize пытаемся вставить "вытолкнутый" элемент,
        //который остался у нас "в руках".
        insert(currentItem.key, currentItem.value);
    }

    //Поиск элемента по ключу 
    T* find(const string& key) {
        //В Cuckoo Hashing элемент может быть только в ДВУХ местах.

        //Проверяем позицию hash1
        uint32_t h1 = hash1(key);
        if (table.data[h1].isOccupied && table.data[h1].key == key) {
            return &table.data[h1].value;
        }

        //Проверяем позицию hash2
        uint32_t h2 = hash2(key);
        if (table.data[h2].isOccupied && table.data[h2].key == key) {
            return &table.data[h2].value;
        }

        //Не нашли
        return nullptr;
    }

    //Удаление элемента 
    bool remove(const string& key) {
        //Ищем в ДВУХ местах.

        //Проверяем позицию hash1
        uint32_t h1 = hash1(key);
        if (table.data[h1].isOccupied && table.data[h1].key == key) {
            table.data[h1].isOccupied = false;
            elementsCount--;
            return true;
        }

        //Проверяем позицию hash2
        uint32_t h2 = hash2(key);
        if (table.data[h2].isOccupied && table.data[h2].key == key) {
            table.data[h2].isOccupied = false;
            elementsCount--;
            return true;
        }

        //Не нашли
        return false;
    }

    //Печать таблицы
    void print() const {
        cout << "=== Cuckoo Хэш-таблица ===" << endl;
        cout << "Размер: " << tableSize << ", Элементов: " << elementsCount << endl;
        for (uint32_t i = 0; i < tableSize; i++) {
            if (table.data[i].isOccupied) {
                cout << "[" << i << "] " << table.data[i].key
                    << " => " << table.data[i].value << endl;
            }
        }
        cout << "===========================" << endl;
    }

    //Получение количества элементов
    uint32_t size() const {
        return elementsCount;
    }

    //Проверка на пустоту
    bool empty() const {
        return elementsCount == 0;
    }

    //Очистка таблицы
    void clear() {
        for (uint32_t i = 0; i < tableSize; i++) {
            table.data[i] = HashNode<T>();
        }
        elementsCount = 0;
    }
};