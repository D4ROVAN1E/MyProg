#include "Bifid.h"

uint8_t get_row_from_char(uint8_t char_code) {
    return (char_code / 16) + 1;
}


uint8_t get_col_from_char(uint8_t char_code) {
    return (char_code % 16) + 1;
}

char get_char_from_coords(uint8_t row, uint8_t col) {
    uint8_t char_code = (row - 1) * 16 + (col - 1);
    return static_cast<char>(char_code);
}

string bifid_encrypt(const string& plaintext) {
    size_t len = plaintext.length();
    if (len == 0) {
        return "";
    }

    // --- Шаг 1: Дробление ---
    // Используем векторы для хранения координат.
    vector<uint8_t> rows;
    vector<uint8_t> cols;

    for (char c : plaintext) {
        uint8_t current_char_code = static_cast<uint8_t>(c);
        rows.push_back(get_row_from_char(current_char_code));
        cols.push_back(get_col_from_char(current_char_code));
    }

    // --- Шаг 2: Перестановка ---
    // Объединяем координаты строк и столбцов в один вектор.
    vector<uint8_t> combined_coords;
    combined_coords.reserve(len * 2);
    combined_coords.insert(combined_coords.end(), rows.begin(), rows.end());
    combined_coords.insert(combined_coords.end(), cols.begin(), cols.end());

    // --- Шаг 3: Слияние ---
    // Читаем объединенные координаты парами для формирования новых символов.
    string ciphertext = "";
    ciphertext.reserve(len);

    for (size_t i = 0; i < combined_coords.size(); i += 2) {
        uint8_t new_row = combined_coords[i];
        uint8_t new_col = combined_coords[i + 1];
        ciphertext += get_char_from_coords(new_row, new_col);
    }

    // Очистка памяти не требуется, vector сделает это автоматически.
    return ciphertext;
}

//Расшифровывает строку, зашифрованную шифром Бифида.
string bifid_decrypt(const string& ciphertext) {
    size_t len = ciphertext.length();
    if (len == 0) {
        return "";
    }

    // --- Шаг 1: Дробление ---
    // Преобразуем каждый символ шифротекста обратно в его пару координат.
    vector<uint8_t> coords;
    coords.reserve(len * 2);

    for (char c : ciphertext) {
        uint8_t current_char_code = static_cast<uint8_t>(c);
        coords.push_back(get_row_from_char(current_char_code));
        coords.push_back(get_col_from_char(current_char_code));
    }

    // --- Шаги 2 и 3: Обратная перестановка и слияние ---
    string plaintext = "";
    plaintext.reserve(len);

    size_t midpoint = coords.size() / 2;

    for (size_t i = 0; i < midpoint; ++i) {
        uint8_t original_row = coords[i];
        uint8_t original_col = coords[i + midpoint];
        plaintext += get_char_from_coords(original_row, original_col);
    }

    // Очистка памяти не требуется.
    return plaintext;
}
