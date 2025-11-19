#ifndef DH_HPP
#define DH_HPP
#include <iostream>
#include <cstdint>
#include <cmath>
#include <string>
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
class DoubleHash {
 private:
    Array<HashNode<T>> table;
    uint32_t tableSize;        // Размер таблицы
    uint32_t elementsCount;    // Количество элементов
    // Дробная часть золотого сечения
    const double A = (sqrt(5.0) - 1.0) / 2.0;

    // Первая хэш-функция: метод умножения
    [[nodiscard]] auto hash1(const string& key) const -> uint32_t {
        // Преобразуем строку в число
        uint64_t numKey = 0;
        for (char c : key) {
            numKey = numKey * 31 + static_cast<uint64_t>(c);
        }

        // hash(k) = floor(M * ((k * A) mod 1))
        double temp = numKey * A;
        temp = temp - floor(temp);   // Получаем дробную часть
        return static_cast<uint32_t>(floor(tableSize * temp));
    }

    // Вторая хэш-функция: метод свёртки
    [[nodiscard]] auto hash2(const string& key) const -> uint32_t {
        // Разбиваем ключ на части и складываем
        uint32_t sum = 0;
        for (char c : key) {
            sum += static_cast<uint8_t>(c);
        }

        // Возвращаем нечётное число (чтобы избежать совпадения с hash1)
        // и не равное 0 (для корректной работы двойного хэширования)
        uint32_t result = (sum % (tableSize - 1)) + 1;

        // Делаем результат нечётным, если размер таблицы чётный
        if (tableSize % 2 == 0 && result % 2 == 0) {
            result++;
        }

        return result;
    }

    // Функция для проверки необходимости расширения таблицы
    [[nodiscard]] auto needResize() const -> bool {
        return (static_cast<double>(elementsCount) / tableSize) > 0.7;
    }

    // Расширение таблицы при достижении порога загрузки
    void resize() {
        uint32_t oldSize = tableSize;
        Array<HashNode<T>> oldTable = table;

        // Увеличиваем размер таблицы
        // Делаем нечётным для лучшего распределения
        tableSize = tableSize * 2 + 1;

        // Создаём новую таблицу
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
    explicit DoubleHash(uint32_t size = 3) : tableSize(size)
                                    , elementsCount(0)
                                    , table(Array<HashNode<T>>(size + 1)) {
        for (uint32_t i = 0; i < tableSize; i++) {
            table[i] = HashNode<T>();
        }
        table.SetSize(tableSize);
    }

    // Деструктор
    ~DoubleHash() {
        // Array имеет свой деструктор, который освободит память
    }

    // Копирующий конструктор
    DoubleHash(const DoubleHash<T>& other) : tableSize(other.tableSize)
                            , elementsCount(other.elementsCount)
                            , table(Array<HashNode<T>>(other.tableSize + 1)) {
        // Копируем все элементы таблицы
        for (uint32_t i = 0; i < tableSize; i++) {
            table[i] = other.table[i];
        }
        table.SetSize(tableSize);
    }

    // Копирующий оператор присваивания
    auto operator=(const DoubleHash<T>& other) -> DoubleHash<T>& {
        // Защита от самоприсваивания
        if (this == &other) {
            return *this;
        }

        // Копируем данные из other
        tableSize = other.tableSize;
        elementsCount = other.elementsCount;

        // Создаём новую таблицу нужного размера
        table = Array<HashNode<T>>(tableSize + 1);

        // Копируем все элементы
        for (uint32_t i = 0; i < tableSize; i++) {
            table[i] = other.table[i];
        }
        table.SetSize(tableSize);

        return *this;
    }

    // Вставка элемента
    void insert(const string& key, const T& value) {
        if (needResize()) {
            resize();
        }

        uint32_t h1 = hash1(key);
        uint32_t h2 = hash2(key);
        uint32_t i = 0;

        while (i < tableSize) {
            uint32_t index = (h1 + i * h2) % tableSize;

            // Если ячейка свободна или была удалена, вставляем
            if (!table[index].isOccupied) {
                table[index] = HashNode<T>(key, value);
                elementsCount++;
                return;
            }

            // Если ключ уже существует, обновляем значение
            if (table[index].key == key) {
                table[index].value = value;
                return;
            }

            i++;
        }

        cout << "Ошибка: таблица переполнена!" << endl;
    }

    // Поиск элемента по ключу
    auto find(const string& key) -> T* {
        uint32_t h1 = hash1(key);
        uint32_t h2 = hash2(key);
        uint32_t i = 0;

        while (i < tableSize) {
            uint32_t index = (h1 + i * h2) % tableSize;

            // Если ячейка никогда не использовалась, элемента нет
            if (!table[index].isOccupied) {
                return nullptr;
            }

            // Если нашли ключ и элемент не удалён
            if (table[index].key == key && table[index].isOccupied) {
                return &table[index].value;
            }

            i++;
        }

        return nullptr;
    }

    // Удаление элемента
    auto remove(const string& key) -> bool {
        uint32_t h1 = hash1(key);
        uint32_t h2 = hash2(key);
        uint32_t i = 0;

        while (i < tableSize) {
            uint32_t index = (h1 + i * h2) % tableSize;

            if (!table[index].isOccupied) {
                return false;
            }

            if (table[index].key == key && table[index].isOccupied) {
                table[index].isOccupied = false;
                elementsCount--;
                return true;
            }

            i++;
        }

        return false;
    }

    // Печать таблицы
    void print() const {
        cout << "=== Хэш-таблица ===" << endl;
        cout << "Размер: " << tableSize
        << ", Элементов: " << elementsCount << endl;
        for (uint32_t i = 0; i < tableSize; i++) {
            if (table[i].isOccupied) {
                cout << "[" << i << "] " << table[i].key
                    << " => " << table[i].value << endl;
            }
        }
        cout << "===================" << endl;
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

#endif   // DH_HPP
