#ifndef LFU_HPP
#define LFU_HPP
#include <string>
#include <sstream>
#include "dh.hpp"
#include "array.hpp"

using namespace std;

template <typename T>
struct CacheNode {
    T value;
    int frequency;
    uint32_t lastUsedTimestamp;  // Метка времени для LRU-приоритета

    // Конструктор по умолчанию
    CacheNode() : value(T()), frequency(0), lastUsedTimestamp(0) {}

    // Рабочий конструктор
    CacheNode(T newVal, int newFreq, uint32_t newTimeUsed)
        : value(newVal), frequency(newFreq), lastUsedTimestamp(newTimeUsed) {
    }
};

template <typename T>
class LFUCache {
 private:
    // Хэш-таблица для быстрого доступа O(1) к узлам кэша по ключу.
    // Ключ - string, Значение - CacheNode
    DoubleHash<CacheNode<T>> cacheMap;

    // Массив для хранения всех ключей, находящихся в кэше.
    // Он нужен, чтобы мы могли итерировать по всем ключам
    // во время вытеснения LFU-элемента.
    Array<string> keyStore;

    int capacity;             // Максимальная ёмкость кэша
    uint32_t globalTimestamp;  // Глобальный счетчик для LRU-приоритета

    // Находит и удаляет "наименее часто используемый" (LFU) элемент.
    void evict() {
        if (keyStore.size == 0) return;  // Нечего вытеснять

        int minFreq = 2147483647;  // "Бесконечность"
        uint32_t minTimestamp = 4294967295;
        string lfuKey = "";
        int lfuKeyIndexInArray = -1;

        // Итерируем по всем ключам в кэше, чтобы найти кандидата на вытеснение
        for (uint32_t i = 0; i < keyStore.size; i++) {
            string currentKey = MGET_BY_IND(keyStore, i);
            CacheNode<T>* node = cacheMap.find(currentKey);

            if (node) {
                // Нашли новый LFU
                if (node->frequency < minFreq) {
                    minFreq = node->frequency;
                    minTimestamp = node->lastUsedTimestamp;
                    lfuKey = currentKey;
                    lfuKeyIndexInArray = i;
                // Частоты равны, применяем LRU-правило
                } else if (node->frequency == minFreq) {
                    if (node->lastUsedTimestamp < minTimestamp) {
                        minTimestamp = node->lastUsedTimestamp;
                        lfuKey = currentKey;
                        lfuKeyIndexInArray = i;
                    }
                }
            }
        }

        // Удаляем найденный LFU/LRU элемент из обеих структур
        if (lfuKeyIndexInArray != -1) {
            cacheMap.remove(lfuKey);
            // Удаляем из массива по индексу
            MDEL_BY_IND(keyStore, lfuKeyIndexInArray);
        }
    }

    // Вспомогательная функция для преобразования int в string.
    auto intToString(int n) -> string {
        ostringstream ss;
        ss << n;
        return ss.str();
    }

 public:
    // Конструктор.
    explicit LFUCache(int cap) :
        cacheMap(),
        keyStore(),
        capacity(cap),
        globalTimestamp(0) {
    }

    // Получает значение по ключу.
    auto get(int keyInt) -> int {
        if (capacity == 0) return -1;

        string key = intToString(keyInt);
        CacheNode<T>* node = cacheMap.find(key);

        if (node == nullptr) {
            return -1;  // Не найдено
        }

        // Элемент найден.
        // Увеличиваем его частоту и обновляем метку времени.
        node->frequency++;
        node->lastUsedTimestamp = ++globalTimestamp;
        return node->value;
    }

    // Устанавливает/обновляет значение по ключу.
    void set(int keyInt, T value) {
        if (capacity == 0) return;

        string key = intToString(keyInt);
        CacheNode<T>* node = cacheMap.find(key);

        if (node != nullptr) {
            // Ключ уже существует.
            // Обновляем значение, увеличиваем частоту, обновляем метку времени.
            node->value = value;
            node->frequency++;
            node->lastUsedTimestamp = ++globalTimestamp;
        } else {
            // Новый ключ.
            // Проверяем, не переполнен ли кэш.
            if (cacheMap.size() >= capacity) {
                // Кэш полон, нужно вытеснить LFU-элемент.
                evict();
            }

            // Добавляем новый элемент.
            // Частота = 1, метка времени = текущая.
            CacheNode<T> newNode(value, 1, ++globalTimestamp);
            cacheMap.insert(key, newNode);
            // Добавляем ключ в наш массив для отслеживания
            MPUSH_BACK(keyStore, key);
        }
    }
};

#endif  // LFU_HPP
