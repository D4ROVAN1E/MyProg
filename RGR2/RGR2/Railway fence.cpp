#include "Railway fence.h"

//Шифрует текст методом "Железнодорожной изгороди"
vector<uint8_t> railFence_encrypt(vector<uint8_t> text, uint32_t key) {
    // Если ключ равен 1, текст не меняется
    if (key <= 1) {
        return text;
    }

    // Создаем матрицу для "рельсов"
    vector<vector<uint8_t>> rail(key);
    uint32_t row = 0;
    bool dir_down = true;

    // Заполняем "рельсы" символами из текста
    for (uint32_t i = 0; i < text.size(); i++) {
        rail[row].push_back(text[i]);

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
    vector<uint8_t> result;
    for (uint32_t i = 0; i < key; i++) {
        for (uint8_t c : rail[i]) {
            result.push_back(c);
        }
    }
    return result;
}

//Дешифрует текст, зашифрованный методом "Железнодорожной изгороди".
vector<uint8_t> railFence_decrypt(vector<uint8_t> cipher, uint32_t key) {
    // Если ключ равен 1, текст не меняется
    if (key <= 1) {
        return cipher;
    }

    // Создаем матрицу "рельсов" с символами-заполнителями
    vector<vector<uint8_t>> rail(key);
    uint32_t row = 0;
    bool dir_down = true;

    // Создаем "трафарет", помечая места для символов
    vector<uint32_t> rail_lengths(key, 0);
    for (uint32_t i = 0; i < cipher.size(); i++) {
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
            rail[i].push_back(cipher[index++]);
        }
    }

    // Считываем символы в правильном порядке для получения исходного текста
    vector<uint8_t> result;
    row = 0;
    dir_down = true;
    vector<uint32_t> rail_read_pos(key, 0);

    for (uint32_t i = 0; i < cipher.size(); i++) {
        result.push_back(rail[row][rail_read_pos[row]++]);

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
