#ifndef CH_HPP
#define CH_HPP
#include <iostream>
#include <cstdint>
#include <cmath>
#include <string>
#include <algorithm>
#include <utility>
#include "array.hpp"

using namespace std;

// Структура для хранения пары ключ-значение
template <typename T>
struct HashNode {
    string key;
    T value;
    bool isOccupied;  // Занята ли ячейка

    HashNode() : key(""), value(T()), isOccupied(false) {}

    HashNode(const string& newKey, const T& newValue)
        : key(newKey), value(newValue), isOccupied(true) {
    }
};

template <typename T>
class CuckooHash {
 private:
    Array<HashNode<T>> table;
    uint32_t tableSize;   // Размер таблицы
    uint32_t elementsCount;    // Количество элементов
    // Дробная часть золотого сечения
    const double A = (sqrt(5.0) - 1.0) / 2.0;

    // Первая хэш-функция
    [[nodiscard]] auto hash1(const string& key) const -> uint32_t {
        uint64_t numKey = 0;
        for (char c : key) {
            numKey = numKey * 31 + static_cast<uint64_t>(c);
        }
        double temp = numKey * A;
        temp = temp - floor(temp);
        return static_cast<uint32_t>(floor(tableSize * temp));
    }

    // Вторая хэш-функция
    [[nodiscard]] auto hash2(const string& key) const -> uint32_t {
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

    // Функция для проверки необходимости расширения таблицы
    // Для Cuckoo Hashing нужен более низкий коэффициент загрузки. Поставим 0.5.
    [[nodiscard]] auto needResize() const -> bool {
        return (static_cast<double>(elementsCount) / tableSize) > 0.5;
    }

    // Расширение таблицы
    void resize() {
        uint32_t oldSize = tableSize;
        Array<HashNode<T>> oldTable = table;

        tableSize = tableSize * 2 + 1;

        table = Array<HashNode<T>>(tableSize + 1);
        for (uint32_t i = 0; i < tableSize; i++) {
            table[i] = HashNode<T>();
        }
        table.SetSize(tableSize);

        elementsCount = 0;

        // Перехэшируем все элементы
        for (uint32_t i = 0; i < oldSize; i++) {
            if (oldTable[i].isOccupied) {
                insert(oldTable[i].key, oldTable[i].value);
            }
        }
    }

 public:
    // Конструктор
    explicit CuckooHash(uint32_t size = 3) : tableSize(size)
                                    , elementsCount(0)
                                    , table(Array<HashNode<T>>(size + 1)) {
        for (uint32_t i = 0; i < tableSize; i++) {
            table[i] = HashNode<T>();
        }
        table.SetSize(tableSize);
    }

    // Деструктор
    ~CuckooHash() {
        // Array имеет свой деструктор
    }

    // Копирующий конструктор
    CuckooHash(CuckooHash<T>& other) : tableSize(other.tableSize)
                            , elementsCount(other.elementsCount)
                            , table(Array<HashNode<T>>(other.tableSize + 1)) {
        for (uint32_t i = 0; i < tableSize; i++) {
            table[i] = other.table[i];
        }
        table.SetSize(tableSize);
    }

    // Копирующий оператор присваивания
    auto operator=(CuckooHash<T>& other) -> CuckooHash<T>& {
        if (this == &other) {
            return *this;
        }
        tableSize = other.tableSize;
        elementsCount = other.elementsCount;
        table = Array<HashNode<T>>(tableSize + 1);
        for (uint32_t i = 0; i < tableSize; i++) {
            table[i] = other.table[i];
        }
        table.SetSize(tableSize);
        return *this;
    }

    // Вставка элемента
    void insert(const string& key, const T& value) {
        // Проверяем, существует ли ключ, и обновляем его
        uint32_t h1 = hash1(key);
        if (table[h1].isOccupied && table[h1].key == key) {
            table[h1].value = value;
            return;
        }
        uint32_t h2 = hash2(key);
        if (table[h2].isOccupied && table[h2].key == key) {
            table[h2].value = value;
            return;
        }

        // Если ключа нет, проверяем resize
        if (needResize()) {
            resize();
        }

        // Начинаем Cuckoo-вставку
        HashNode<T> currentItem(key, value);
        uint32_t currentPos = h1;

        // Ограничиваем количество "выталкиваний"
        // чтобы избежать бесконечного цикла.
        for (uint32_t i = 0; i < tableSize * 2; i++) {
            // Если ячейка свободна
            if (!table[currentPos].isOccupied) {
                table[currentPos] = currentItem;
                elementsCount++;
                return;
            }

            // Ячейка занята. "Выталкиваем" (меняем)
            swap(currentItem, table[currentPos]);

            // Теперь 'currentItem' - это "вытолкнутый" элемент.
            uint32_t pos1 = hash1(currentItem.key);
            uint32_t pos2 = hash2(currentItem.key);

            currentPos = (currentPos == pos1) ? pos2 : pos1;
        }

        // Если мы вышли из цикла, значит, обнаружили цикл.
        resize();
        // После resize пытаемся вставить "вытолкнутый" элемент,
        // который остался у нас "в руках".
        insert(currentItem.key, currentItem.value);
    }

    // Поиск элемента по ключу
    auto find(const string& key) -> T* {
        // В Cuckoo Hashing элемент может быть только в ДВУХ местах.

        // Проверяем позицию hash1
        uint32_t h1 = hash1(key);
        if (table[h1].isOccupied && table[h1].key == key) {
            return &table[h1].value;
        }

        // Проверяем позицию hash2
        uint32_t h2 = hash2(key);
        if (table[h2].isOccupied && table[h2].key == key) {
            return &table[h2].value;
        }

        // Не нашли
        return nullptr;
    }

    // Удаление элемента
    auto remove(const string& key) -> bool {
        // Ищем в ДВУХ местах.

        // Проверяем позицию hash1
        uint32_t h1 = hash1(key);
        if (table[h1].isOccupied && table[h1].key == key) {
            table[h1].isOccupied = false;
            elementsCount--;
            return true;
        }

        // Проверяем позицию hash2
        uint32_t h2 = hash2(key);
        if (table[h2].isOccupied && table[h2].key == key) {
            table[h2].isOccupied = false;
            elementsCount--;
            return true;
        }

        // Не нашли
        return false;
    }

    // Печать таблицы
    void print() const {
        cout << "=== Cuckoo Хэш-таблица ===" << endl;
        cout << "Размер: " << tableSize
        << ", Элементов: " << elementsCount << endl;
        for (uint32_t i = 0; i < tableSize; i++) {
            if (table[i].isOccupied) {
                cout << "[" << i << "] " << table[i].key
                    << " => " << table[i].value << endl;
            }
        }
        cout << "===========================" << endl;
    }

    // Получение количества элементов
    [[nodiscard]] auto size() const -> uint32_t {
        return elementsCount;
    }

    // Проверка на пустоту
    [[nodiscard]] auto empty() const -> bool {
        return elementsCount == 0;
    }

    // Очистка таблицы
    void clear() {
        for (uint32_t i = 0; i < tableSize; i++) {
            table[i] = HashNode<T>();
        }
        elementsCount = 0;
    }
};

#endif   // CH_HPP
