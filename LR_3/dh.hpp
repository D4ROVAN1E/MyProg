#ifndef DH_HPP
#define DH_HPP

#include <iostream>
#include <cstdint>
#include <cmath>
#include <string>
#include <fstream> 
#include <stdexcept> 
#include "array.hpp"

using namespace std;

// Структура для хранения пары ключ-значение
template <typename T>
struct HashNode {
    string key;
    T value;
    bool isOccupied;  // Занята ли ячейка
    bool isDeleted;

    HashNode() : key(""), value(T()), isOccupied(false), isDeleted(false) {}

    HashNode(const string& newKey, const T& newValue)
        : key(newKey), value(newValue), isOccupied(true), isDeleted(false) {
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

        // Возвращаем нечётное число
        uint32_t result = (sum % (tableSize - 1)) + 1;

        return result;
    }

    // Функция для проверки необходимости расширения таблицы
    [[nodiscard]] auto needResize() const -> bool {
        if (tableSize == 0) return true; // Защита от деления на ноль
        return (static_cast<double>(elementsCount) / tableSize) > 0.7;
    }

    // Вспомогательная функция проверки на простоту
    [[nodiscard]] static bool isPrime(uint32_t n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (uint32_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
    }

    // Поиск следующего простого числа
    [[nodiscard]] static uint32_t nextPrime(uint32_t n) {
    if (n <= 1) return 2;
    while (!isPrime(n)) {
        n++;
    }
    return n;
    }

    // Расширение таблицы при достижении порога загрузки
    void resize() {
        uint32_t oldSize = tableSize;
        Array<HashNode<T>> oldTable = table;

        // Увеличиваем размер таблицы
        // Делаем нечётным для лучшего распределения
        tableSize = nextPrime(tableSize * 2);

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
        if (size == 0) {
            throw invalid_argument("Table size cannot be zero");
        }
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
        if (needResize())
            resize();

        uint32_t h1 = hash1(key);
        uint32_t h2 = hash2(key);
        uint32_t i = 0;

        int32_t firstDeletedIndex = -1; // Запоминаем первое место, куда можно вставить

        while (i < tableSize) {
            uint32_t index = (h1 + i * h2) % tableSize;

            // Нашли точное совпадение — обновляем
            if (table[index].isOccupied && table[index].key == key) {
                table[index].value = value;
                return;
            }

            // Если ячейка свободна (и не удалена) — цепочка кончилась
            if (!table[index].isOccupied && !table[index].isDeleted) {
                // Вставляем в текущую пустую
                table[index] = HashNode<T>(key, value);
                elementsCount++;
                return;
            }

            i++;
        }

        // Если мы здесь, значит не удалось вставить элемент (таблица забита или проблема хэш-функции)
        throw overflow_error("Error: Hash table is full, cannot insert key.");
    }

    // Поиск элемента по ключу
    auto find(const string& key) -> T* {
        if (elementsCount == 0) return nullptr;

        uint32_t h1 = hash1(key);
        uint32_t h2 = hash2(key);
        uint32_t i = 0;

        while (i < tableSize) {
            uint32_t index = (h1 + i * h2) % tableSize;

            // Если ячейка никогда не использовалась, элемента нет
            if (!table[index].isOccupied && !table[index].isDeleted) {
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
        if (elementsCount == 0) return false;

        uint32_t h1 = hash1(key);
        uint32_t h2 = hash2(key);
        uint32_t i = 0;

        while (i < tableSize) {
            uint32_t index = (h1 + i * h2) % tableSize;

            // Если нашли совпадение и ячейка живая
            if (table[index].isOccupied && table[index].key == key) {
            table[index].isOccupied = false;
            table[index].isDeleted = true;
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
                cout << "[" << i << "] " << table[i].key << " => " << table[i].value << endl;
            }
        }
        cout << "===================" << endl;
    }
    
    // Сериализация в текстовом формате
    void serialize_text(const string& filename) const {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            throw runtime_error("Error: Could not open file for writing: " + filename);
        }

        // Записываем заголовок
        outFile << tableSize << " " << elementsCount << endl;

        // Записываем только занятые ячейки
        for (uint32_t i = 0; i < tableSize; i++) {
            if (table[i].isOccupied) {
                outFile << i << " " << table[i].key << " " << table[i].value << endl;
            }
        }

        outFile.close();
        cout << "Таблица (текст) успешно сохранена в " << filename << endl;
    }

    // Десериализация из текстового формата
    void deserialize_text(const string& filename) {
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            throw runtime_error("Error: Could not open file for reading: " + filename);
        }

        uint32_t newTableSize = 0;
        uint32_t newElementsCount = 0;

        // Читаем заголовок
        inFile >> newTableSize >> newElementsCount;
        if (newTableSize == 0)
            throw runtime_error("Could not read data from file. Size of table equal to zero");

        // Пересоздаем таблицу
        table = Array<HashNode<T>>(newTableSize + 1);
        for (uint32_t i = 0; i < newTableSize; i++) {
            table[i] = HashNode<T>();
        }
        table.SetSize(newTableSize);

        tableSize = newTableSize;
        elementsCount = newElementsCount;

        // Читаем данные
        uint32_t idx;
        string key;
        T value;

        // Читаем данные
        while (inFile >> idx >> key >> value) {
            table[idx] = HashNode<T>(key, value);
        }

        inFile.close();
        cout << "Таблица (текст) успешно загружена из " << filename << endl;
    }

    // Сериализация в бинарном формате
    void serialize_bin(const string& filename) const {
        ofstream outFile(filename, ios::binary);
        if (!outFile.is_open()) {
            throw runtime_error("Error: Could not open binary file for writing: " + filename);
        }

        // Записываем размер таблицы и количество элементов
        outFile.write(reinterpret_cast<const char*>(&tableSize), sizeof(tableSize));
        outFile.write(reinterpret_cast<const char*>(&elementsCount), sizeof(elementsCount));

        for (uint32_t i = 0; i < tableSize; i++) {
            bool occupied = table[i].isOccupied;
            outFile.write(reinterpret_cast<const char*>(&occupied), sizeof(bool));

            if (occupied) {
                // Записываем длину ключа
                uint32_t keyLen = static_cast<uint32_t>(table[i].key.size());
                outFile.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));

                // Записываем сам ключ
                outFile.write(table[i].key.c_str(), keyLen);

                // Записываем значение (работает корректно для POD-типов)
                outFile.write(reinterpret_cast<const char*>(&table[i].value), sizeof(T));
            }
        }

        outFile.close();
        cout << "Таблица (бинарн.) успешно сохранена в " << filename << endl;
    }

    // Десериализация из бинарного формата
    void deserialize_bin(const string& filename) {
        ifstream inFile(filename, ios::binary);
        if (!inFile.is_open()) {
            throw runtime_error("Error: Could not open binary file for reading: " + filename);
        }

        // Читаем размеры
        uint32_t newTableSize = 0;
        uint32_t newElementsCount = 0;

        inFile.read(reinterpret_cast<char*>(&newTableSize), sizeof(newTableSize));
        inFile.read(reinterpret_cast<char*>(&newElementsCount), sizeof(newElementsCount));

        table = Array<HashNode<T>>(newTableSize + 1);
        for (uint32_t i = 0; i < newTableSize; i++) {
            table[i] = HashNode<T>();
        }
        table.SetSize(newTableSize);

        tableSize = newTableSize;
        elementsCount = newElementsCount;

        // Читаем данные ячеек
        for (uint32_t i = 0; i < tableSize; i++) {
            bool occupied = false;
            inFile.read(reinterpret_cast<char*>(&occupied), sizeof(bool));

            if (occupied) {
                // Читаем длину ключа
                uint32_t keyLen = 0;
                inFile.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));

                char* keyBuf = new char[keyLen + 1];
                inFile.read(keyBuf, keyLen);
                
                keyBuf[keyLen] = '\0';
                string loadedKey(keyBuf);
                delete[] keyBuf;

                // Читаем значение
                T loadedValue;
                inFile.read(reinterpret_cast<char*>(&loadedValue), sizeof(T));

                table[i] = HashNode<T>(loadedKey, loadedValue);
            } else {
                table[i].isOccupied = false;
            }
        }

        inFile.close();
        cout << "Таблица (бинарн.) успешно загружена из " << filename << endl;
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
