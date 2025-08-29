#include "Railway fence.h"

//Шифрует текст методом "Железнодорожной изгороди"
string encryptRailFence(string text, uint32_t key) {
    // Если ключ равен 1, текст не меняется
    if (key <= 1) {
        return text;
    }

    // Создаем матрицу для "рельсов"
    vector<string> rail(key);
    uint32_t row = 0;
    bool dir_down = true;

    // Заполняем "рельсы" символами из текста
    for (uint32_t i = 0; i < text.length(); i++) {
        rail[row] += text[i];

        // Меняем направление движения по "рельсам"
        if (row == key - 1) {
            dir_down = false;
        }
        else if (row == 0) {
            dir_down = true;
        }

        if (dir_down) {
            row++;
        }
        else {
            row--;
        }
    }

    // Собираем зашифрованный текст из "рельсов"
    string result = "";
    for (uint32_t i = 0; i < key; i++) {
        result += rail[i];
    }
    return result;
}

//Дешифрует текст, зашифрованный методом "Железнодорожной изгороди".
string decryptRailFence(string cipher, uint32_t key) {
    // Если ключ равен 1, текст не меняется
    if (key <= 1) {
        return cipher;
    }

    // Создаем матрицу "рельсов" с символами-заполнителями
    vector<string> rail(key);
    uint32_t row = 0;
    bool dir_down = true;

    // Создаем "трафарет", помечая места для символов
    vector<uint32_t> rail_lengths(key, 0);
    for (uint32_t i = 0; i < cipher.length(); i++) {
        rail_lengths[row]++;

        if (row == key - 1) {
            dir_down = false;
        }
        else if (row == 0) {
            dir_down = true;
        }

        if (dir_down) {
            row++;
        }
        else {
            row--;
        }
    }

    // Заполняем "рельсы" символами из шифротекста
    uint32_t index = 0;
    for (uint32_t i = 0; i < key; i++) {
        for (uint32_t j = 0; j < rail_lengths[i]; j++) {
            rail[i] += cipher[index++];
        }
    }

    // Считываем символы в правильном порядке для получения исходного текста
    string result = "";
    row = 0;
    dir_down = true;
    vector<uint32_t> rail_read_pos(key, 0);

    for (uint32_t i = 0; i < cipher.length(); i++) {
        result += rail[row][rail_read_pos[row]++];

        if (row == key - 1) {
            dir_down = false;
        }
        else if (row == 0) {
            dir_down = true;
        }

        if (dir_down) {
            row++;
        }
        else {
            row--;
        }
    }

    return result;
}
