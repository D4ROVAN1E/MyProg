#ifndef SET_HPP
#define SET_HPP
#include <cmath>
#include <string>
#include <fstream>
#include <iostream>
#include <type_traits>
#include "array.hpp"
#include "singly_list.hpp"

using namespace std;

template <typename T>
class HashSet {
 private:
    Array<ForwardList<T>> table;
    uint32_t elementCount;
    // Константа для хэш-функцииs
    const double A = (sqrt(5.0) - 1.0) / 2.0;

    // Хэш-функция методом умножения
    [[nodiscard]] auto hashFunction(const T& key) const -> uint32_t {
        // Преобразуем ключ в число
        uint64_t k = 0;
        if constexpr (is_arithmetic_v<T>) {
            k = static_cast<uint64_t>(key);
        } else {
            // Для строк вычисляем хэш
            string str = key;
            for (char c : str) {
                k = k * 31 + static_cast<uint64_t>(c);
            }
        }

        // Применяем метод умножения: hash(k) = floor(M * ((k * A) mod 1))
        double temp = k * A;
        temp = temp - floor(temp);  // (k * A) mod 1

        return static_cast<uint32_t>(floor(table.GetSize() * temp));
    }

    // Проверка необходимости расширения таблицы
    void checkAndResize() {
        // Если коэффициент загрузки > 0.75, увеличиваем таблицу
        if (elementCount > table.GetSize() * 0.75) {
            uint32_t oldSize = table.GetSize();
            Array<ForwardList<T>> oldTable = table;

            // Создаём новую таблицу удвоенного размера
            table = Array<ForwardList<T>>(oldSize * 2);
            for (uint32_t i = 0; i < table.GetCapacity(); i++) {
                table[i] = ForwardList<T>();
            }
            table.SetSize(table.GetCapacity());
            elementCount = 0;

            // Перехэшируем все элементы
            for (uint32_t i = 0; i < oldSize; i++) {
                SNode<T>* current = oldTable[i].GetHead();
                while (current != nullptr) {
                    uint32_t newHash = hashFunction(current->key);
                    table[newHash].FPUSH_BACK(current->key);
                    elementCount++;
                    current = current->next;
                }
            }
        }
    }

 public:
    explicit HashSet(uint32_t initialSize = 16) :
                table(Array<ForwardList<T>>(initialSize))
                , elementCount(0) {
        // Инициализируем каждую ячейку пустым списком
        for (uint32_t i = 0; i < table.GetCapacity(); i++) {
            table[i] = ForwardList<T>();
        }
        table.SetSize(table.GetCapacity());
    }

    // Копирующий конструктор
    HashSet(HashSet<T>& other) :
                    table(Array<ForwardList<T>>(other.table.GetCapacity()))
                    , elementCount(other.elementCount) {
        // Копируем каждый список из другого множества
        for (uint32_t i = 0; i < other.table.GetSize(); i++) {
            // Используем копирующий оператор ForwardList
            table[i] = other.table[i];
        }
        table.SetSize(other.table.GetSize());
    }

    // Копирующий оператор присваивания
    auto operator=(HashSet<T>& other) -> HashSet<T>& {
        if (this == &other) {
            return *this;
        }

        // Создаём новую таблицу
        table = Array<ForwardList<T>>(other.table.GetCapacity());
        elementCount = other.elementCount;

        // Копируем каждый список
        for (uint32_t i = 0; i < other.table.GetSize(); i++) {
            table[i] = other.table[i];
        }
        table.SetSize(other.table.GetSize());

        return *this;
    }

    ~HashSet() {
        // Деструкторы Array и ForwardList очистят память автоматически
    }

    // Добавление элемента в множество
    void SETADD(const T& key) {
        uint32_t hash = hashFunction(key);

        // Проверяем, есть ли уже такой элемент
        SNode<T>* found = table[hash].FGET_BY_VALUE(key);
        if (found != nullptr) {
            return;  // Элемент уже существует, не добавляем дубликат
        }

        // Добавляем элемент в соответствующий список
        table[hash].FPUSH_BACK(key);
        elementCount++;

        // Проверяем необходимость расширения
        checkAndResize();
    }

    // Удаление элемента из множества
    void SETDEL(const T& key) {
        uint32_t hash = hashFunction(key);

        // Проверяем наличие элемента
        SNode<T>* found = table[hash].FGET_BY_VALUE(key);
        if (found == nullptr) {
            return;  // Элемент не найден
        }

        // Удаляем элемент из списка
        table[hash].FDEL_BY_VALUE(key);
        elementCount--;
    }

    // Проверка наличия элемента в множестве
    auto SET_AT(const T& key) -> bool {
        uint32_t hash = hashFunction(key);
        SNode<T>* found = table[hash].FGET_BY_VALUE(key);
        return found != nullptr;
    }

    // Сохранение множества в файл
    void SETSAVE(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            return;
        }

        // Сохраняем размер таблицы и количество элементов
        file << table.GetSize() << " " << elementCount << endl;

        // Сохраняем все элементы
        for (uint32_t i = 0; i < table.GetSize(); i++) {
            SNode<T>* current = table[i].GetHead();
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
        for (uint32_t i = 0; i < table.GetSize(); i++) {
            if (table[i].GetHead() != nullptr) {
                cout << "Bucket " << i << ": ";
                table[i].PRINT();
            }
        }
    }

    auto GetSize() -> uint32_t {
        return elementCount;
    }
};

#endif  // SET_HPP
