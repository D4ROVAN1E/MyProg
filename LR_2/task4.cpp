#include <iostream>
#include <string>
#include "array.hpp"

using namespace std;

// Структура для хранения символа и его частоты
struct CharFreq {
    char ch;
    uint32_t freq;

    CharFreq() : ch('\0'), freq(0) {}
    CharFreq(char c, uint32_t f) : ch(c), freq(f) {}
};

// Функция для поиска символа в массиве
int findChar(Array<CharFreq>& arr, char ch) {
    for (uint32_t i = 0; i < arr.size; i++) {
        if (arr.data[i].ch == ch) {
            return i;
        }
    }
    return -1;
}

// Функция сортировки пузырьком по возрастанию кода символа
void sortByChar(Array<CharFreq>& arr) {
    for (uint32_t i = 0; i < arr.size; i++) {
        for (uint32_t j = 0; j < arr.size - 1 - i; j++) {
            if (arr.data[j].ch > arr.data[j + 1].ch) {
                CharFreq temp = arr.data[j];
                arr.data[j] = arr.data[j + 1];
                arr.data[j + 1] = temp;
            }
        }
    }
}

// Поиск максимальной частоты
uint32_t findMaxFreq(Array<CharFreq>& arr) {
    uint32_t maxFreq = 0;
    for (uint32_t i = 0; i < arr.size; i++) {
        if (arr.data[i].freq > maxFreq) {
            maxFreq = arr.data[i].freq;
        }
    }
    return maxFreq;
}

int main() {
	setlocale(LC_ALL, "Russian");
    Array<CharFreq> charArray;
    string line;

    // Чтение всего ввода
    getline(cin, line);
    for (char ch : line) {
        // Игнорируем пробелы
        if (ch == ' ') {
            continue;
        }

        // Ищем символ в массиве
        int index = findChar(charArray, ch);

        if (index == -1) {
            // Символ не найден, добавляем новый
            MPUSH_BACK(charArray, CharFreq(ch, 1));
        }
        else {
            // Символ найден, увеличиваем частоту
            charArray.data[index].freq++;
        }
    }

    // Если массив пустой, выходим
    if (charArray.size == 0) {
        return 0;
    }

    // Сортируем по коду символа
    sortByChar(charArray);

    // Находим максимальную частоту для определения высоты гистограммы
    uint32_t maxFreq = findMaxFreq(charArray);

    // Выводим гистограмму сверху вниз
    for (uint32_t level = maxFreq; level > 0; level--) {
        for (uint32_t i = 0; i < charArray.size; i++) {
            if (charArray.data[i].freq >= level) {
                cout << "#";
            }
            else {
                cout << " ";
            }
        }
        cout << endl;
    }

    // Выводим символы
    for (uint32_t i = 0; i < charArray.size; i++) {
        cout << charArray.data[i].ch;
    }
    cout << endl;

    return 0;
}