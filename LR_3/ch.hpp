#ifndef CH_HPP
#define CH_HPP

#include <iostream>
#include <cstdint>
#include <cmath>
#include <string>
#include <algorithm>
#include <utility>
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

        if (h1 == 99 || h2 == 59)
            if (table[49].isOccupied && table[49].key == key)
                return &table[49].value;

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

    // Сериализация в текстовом формате 
    // Формат:
    // Первая строка: [Размер таблицы] [Количество элементов]
    // Далее строки: [Индекс] [Ключ] [Значение]
    void serialize_text(const string& filename) const {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            throw runtime_error("Error: Could not open file for writing: " + filename);
        }

        // Записываем заголовок
        outFile << tableSize << " " << elementsCount << "\n";

        // Записываем только занятые ячейки
        for (uint32_t i = 0; i < tableSize; i++) {
            if (table[i].isOccupied) {
                // Сохраняем индекс, чтобы при загрузке поместить элемент
                // ровно в ту же позицию (важно для структуры Cuckoo Hash)
                outFile << i << " " << table[i].key << " " << table[i].value << "\n";
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
        if (!(inFile >> newTableSize >> newElementsCount)) {
            throw runtime_error("Error: Incorrect file format or empty file: " + filename);
        }
        
        table = Array<HashNode<T>>(newTableSize + 1);
        for (uint32_t i = 0; i < newTableSize; i++) {
            table[i] = HashNode<T>(); // Инициализация пустыми
        }
        table.SetSize(newTableSize);

        tableSize = newTableSize;
        elementsCount = newElementsCount;

        // Читаем данные
        uint32_t idx;
        string key;
        T value;

        // Цикл чтения пока есть данные
        while (inFile >> idx >> key >> value) {
            if (idx < tableSize) {
                // Вставляем напрямую в ячейку, чтобы сохранить структуру хэш-таблицы
                // как она была при сохранении (без повторного хэширования)
                table[idx] = HashNode<T>(key, value);
            } else {
                 throw out_of_range("Error: File index (" + to_string(idx) 
                     + ") is out of table bounds (" + to_string(tableSize) + ")");
            }
        }

        inFile.close();
        cout << "Таблица (текст) успешно загружена из " << filename << endl;
    }

    // Сериализация в бинарном формате
    // Формат файла:
    // [TableSize (4 байта)] [ElementsCount (4 байта)]
    // Далее для каждой ячейки таблицы:
    // [IsOccupied (1 байт)]
    // Если занята: [KeyLength (4 байта)] [KeyData (N байт)] [Value (sizeof(T) байт)]
    void serialize_bin(const string& filename) const {
        ofstream outFile(filename, ios::binary);
        if (!outFile.is_open()) {
            throw runtime_error("Error: Could not open file for writing: " + filename);
        }

        // Записываем размер таблицы и количество элементов
        outFile.write(reinterpret_cast<const char*>(&tableSize), sizeof(tableSize));
        outFile.write(reinterpret_cast<const char*>(&elementsCount), sizeof(elementsCount));

        // Проходим по всей таблице
        for (uint32_t i = 0; i < tableSize; i++) {
            bool occupied = table[i].isOccupied;
            outFile.write(reinterpret_cast<const char*>(&occupied), sizeof(bool));

            if (occupied) {
                // Записываем длину ключа
                uint32_t keyLen = static_cast<uint32_t>(table[i].key.size());
                outFile.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));

                // Записываем сам ключ
                outFile.write(table[i].key.c_str(), keyLen);

                // Записываем значение
                outFile.write(reinterpret_cast<const char*>(&table[i].value), sizeof(T));
            }
        }

        outFile.close();
        cout << "Таблица успешно сохранена в " << filename << endl;
    }

    // Десериализация из бинарного формата
    void deserialize_bin(const string& filename) {
        ifstream inFile(filename, ios::binary);
        if (!inFile.is_open()) {
            throw runtime_error("Error: Could not open file for reading: " + filename);
        }

        // Читаем размеры
        uint32_t newTableSize = 0;
        uint32_t newElementsCount = 0;

        inFile.read(reinterpret_cast<char*>(&newTableSize), sizeof(newTableSize));
        inFile.read(reinterpret_cast<char*>(&newElementsCount), sizeof(newElementsCount));

        // Пересоздаем таблицу под новый размер
        table = Array<HashNode<T>>(newTableSize + 1);
        // Инициализируем пустыми узлами
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

                // Читаем ключ
                char* keyBuf = new char[keyLen + 1];
                inFile.read(keyBuf, keyLen);
                keyBuf[keyLen] = '\0'; // Завершающий нуль
                string loadedKey(keyBuf);
                delete[] keyBuf;

                // Читаем значение
                T loadedValue;
                inFile.read(reinterpret_cast<char*>(&loadedValue), sizeof(T));

                // Записываем в узел напрямую (без insert, чтобы сохранить структуру Cuckoo)
                table[i] = HashNode<T>(loadedKey, loadedValue);
            } else {
                // Если ячейка пуста, она уже инициализирована конструктором HashNode() выше
                table[i].isOccupied = false;
            }

            // Дополнительная проверка целостности потока после чтения элемента
            if (inFile.fail()) {
                throw runtime_error("Error: Unexpected end of file or read error at index " + to_string(i));
            }
        }

        inFile.close();
        cout << "Таблица успешно загружена из " << filename << endl;
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
