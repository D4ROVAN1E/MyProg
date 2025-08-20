#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>
#include <fstream>
#include <random>
#include <algorithm>

// Использовать стандартное пространство имен
using namespace std;

// --- Константы и таблицы для AES ---

// Таблица подстановок S-box
const uint8_t s_box[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

// Инвертированная таблица подстановок InvS-box
const uint8_t inv_s_box[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

// Раундовые константы Rcon
const uint8_t Rcon[11] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };

// --- Вспомогательные функции ---

// Функция для печати байтового массива в HEX формате
void print_hex(const string& label, const uint8_t* data, int len) {
    cout << label << ": ";
    for (int i = 0; i < len; ++i) {
        cout << hex << setfill('0') << setw(2) << static_cast<int>(data[i]) << " ";
    }
    cout << dec << endl;
}

// Функция для печати состояния (State) в виде матрицы 4x4
void print_state(const string& label, const uint8_t state[16]) {
    cout << "--- " << label << " ---" << endl;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            // State заполняется по столбцам
            cout << hex << setfill('0') << setw(2) << static_cast<int>(state[c * 4 + r]) << " ";
        }
        cout << endl;
    }
    cout << dec << "--------------------" << endl;
}

// --- Основные операции AES ---

// SubBytes: Замена каждого байта в state, используя s_box
void sub_bytes(uint8_t state[16]) {
    for (int i = 0; i < 16; ++i) {
        state[i] = s_box[state[i]];
    }
}

// InvSubBytes: Замена каждого байта в state, используя inv_s_box
void inv_sub_bytes(uint8_t state[16]) {
    for (int i = 0; i < 16; ++i) {
        state[i] = inv_s_box[state[i]];
    }
}

// ShiftRows: Циклический сдвиг строк state
void shift_rows(uint8_t state[16]) {
    uint8_t temp[16];
    // Строка 0: без сдвига
    // Строка 1: сдвиг на 1 байт влево
    temp[1] = state[5]; temp[5] = state[9]; temp[9] = state[13]; temp[13] = state[1];
    // Строка 2: сдвиг на 2 байта влево
    temp[2] = state[10]; temp[10] = state[2];
    temp[6] = state[14]; temp[14] = state[6];
    // Строка 3: сдвиг на 3 байта влево
    temp[3] = state[15]; temp[15] = state[11]; temp[11] = state[7]; temp[7] = state[3];

    state[1] = temp[1]; state[5] = temp[5]; state[9] = temp[9]; state[13] = temp[13];
    state[2] = temp[2]; state[10] = temp[10]; state[6] = temp[6]; state[14] = temp[14];
    state[3] = temp[3]; state[15] = temp[15]; state[11] = temp[11]; state[7] = temp[7];
}


// InvShiftRows: Обратный циклический сдвиг строк state
void inv_shift_rows(uint8_t state[16]) {
    uint8_t temp[16];
    // Строка 0: без сдвига
    // Строка 1: сдвиг на 1 байт вправо
    temp[5] = state[1]; temp[9] = state[5]; temp[13] = state[9]; temp[1] = state[13];
    // Строка 2: сдвиг на 2 байта вправо
    temp[10] = state[2]; temp[2] = state[10];
    temp[14] = state[6]; temp[6] = state[14];
    // Строка 3: сдвиг на 3 байта вправо
    temp[15] = state[3]; temp[11] = state[15]; temp[7] = state[11]; temp[3] = state[7];

    state[1] = temp[1]; state[5] = temp[5]; state[9] = temp[9]; state[13] = temp[13];
    state[2] = temp[2]; state[10] = temp[10]; state[6] = temp[6]; state[14] = temp[14];
    state[3] = temp[3]; state[7] = temp[7]; state[11] = temp[11]; state[15] = temp[15];
}

// Умножение в поле Галуа GF(2^8)
uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    uint8_t hi_bit_set;
    for (int i = 0; i < 8; ++i) {
        if (b & 1) {
            p ^= a;
        }
        hi_bit_set = (a & 0x80);
        a <<= 1;
        if (hi_bit_set) {
            a ^= 0x1b; // x^8 + x^4 + x^3 + x + 1
        }
        b >>= 1;
    }
    return p;
}

// MixColumns: Смешивание столбцов state
void mix_columns(uint8_t state[16]) {
    uint8_t temp[16];
    for (int c = 0; c < 4; ++c) {
        int i = c * 4;
        temp[i] = gmul(0x02, state[i]) ^ gmul(0x03, state[i + 1]) ^ state[i + 2] ^ state[i + 3];
        temp[i + 1] = state[i] ^ gmul(0x02, state[i + 1]) ^ gmul(0x03, state[i + 2]) ^ state[i + 3];
        temp[i + 2] = state[i] ^ state[i + 1] ^ gmul(0x02, state[i + 2]) ^ gmul(0x03, state[i + 3]);
        temp[i + 3] = gmul(0x03, state[i]) ^ state[i + 1] ^ state[i + 2] ^ gmul(0x02, state[i + 3]);
    }
    for (int i = 0; i < 16; ++i) {
        state[i] = temp[i];
    }
}

// InvMixColumns: Обратное смешивание столбцов state
void inv_mix_columns(uint8_t state[16]) {
    uint8_t temp[16];
    for (int c = 0; c < 4; ++c) {
        int i = c * 4;
        temp[i] = gmul(0x0e, state[i]) ^ gmul(0x0b, state[i + 1]) ^ gmul(0x0d, state[i + 2]) ^ gmul(0x09, state[i + 3]);
        temp[i + 1] = gmul(0x09, state[i]) ^ gmul(0x0e, state[i + 1]) ^ gmul(0x0b, state[i + 2]) ^ gmul(0x0d, state[i + 3]);
        temp[i + 2] = gmul(0x0d, state[i]) ^ gmul(0x09, state[i + 1]) ^ gmul(0x0e, state[i + 2]) ^ gmul(0x0b, state[i + 3]);
        temp[i + 3] = gmul(0x0b, state[i]) ^ gmul(0x0d, state[i + 1]) ^ gmul(0x09, state[i + 2]) ^ gmul(0x0e, state[i + 3]);
    }
    for (int i = 0; i < 16; ++i) {
        state[i] = temp[i];
    }
}


// AddRoundKey: XOR состояния с раундовым ключом
void add_round_key(uint8_t state[16], const uint8_t* round_key) {
    for (int i = 0; i < 16; ++i) {
        state[i] ^= round_key[i];
    }
}

// --- Расширение ключа ---

// Функция расширения ключа AES-128
void key_expansion(const uint8_t* key, uint8_t* expanded_keys) {
    // Копируем исходный ключ в начало expanded_keys
    for (int i = 0; i < 16; ++i) {
        expanded_keys[i] = key[i];
    }

    int bytes_generated = 16; // 16 байт уже есть (исходный ключ)
    int rcon_iteration = 1;
    uint8_t temp[4];

    while (bytes_generated < 176) { // 11 ключей * 16 байт = 176
        // Берем последние 4 байта из сгенерированных
        for (int i = 0; i < 4; ++i) {
            temp[i] = expanded_keys[bytes_generated - 4 + i];
        }

        // Если это начало нового раундового ключа (каждые 16 байт)
        if (bytes_generated % 16 == 0) {
            // RotWord: циклический сдвиг байтов влево
            uint8_t t = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;

            // SubWord: применяем S-box к каждому байту
            for (int i = 0; i < 4; ++i) {
                temp[i] = s_box[temp[i]];
            }

            // XOR с Rcon
            temp[0] ^= Rcon[rcon_iteration++];
        }

        // Генерируем следующие 4 байта ключа
        for (int i = 0; i < 4; ++i) {
            expanded_keys[bytes_generated] = expanded_keys[bytes_generated - 16] ^ temp[i];
            bytes_generated++;
        }
    }
}

// --- Шифрование и дешифрование одного блока ---

void encrypt_block(uint8_t block[16], const uint8_t* expanded_keys) {
    print_state("Исходное состояние блока", block);
    add_round_key(block, expanded_keys); // Initial round

    for (int round = 1; round < 10; ++round) {
        print_state("Начало раунда " + to_string(round), block);
        sub_bytes(block);
        print_state("После SubBytes", block);
        shift_rows(block);
        print_state("После ShiftRows", block);
        mix_columns(block);
        print_state("После MixColumns", block);
        add_round_key(block, expanded_keys + round * 16);
    }

    // Final round (без MixColumns)
    print_state("Начало финального раунда (10)", block);
    sub_bytes(block);
    print_state("После SubBytes", block);
    shift_rows(block);
    print_state("После ShiftRows", block);
    add_round_key(block, expanded_keys + 10 * 16);
    print_state("Зашифрованный блок", block);
}

void decrypt_block(uint8_t block[16], const uint8_t* expanded_keys) {
    print_state("Исходный шифроблок", block);
    add_round_key(block, expanded_keys + 10 * 16); // Initial round (с последним ключом)

    for (int round = 9; round >= 1; --round) {
        print_state("Начало раунда " + to_string(round), block);
        inv_shift_rows(block);
        print_state("После InvShiftRows", block);
        inv_sub_bytes(block);
        print_state("После InvSubBytes", block);
        add_round_key(block, expanded_keys + round * 16);
        print_state("После AddRoundKey", block);
        inv_mix_columns(block);
        print_state("После InvMixColumns", block);
    }

    // Final round (без InvMixColumns)
    print_state("Начало финального раунда (0)", block);
    inv_shift_rows(block);
    print_state("После InvShiftRows", block);
    inv_sub_bytes(block);
    print_state("После InvSubBytes", block);
    add_round_key(block, expanded_keys);
    print_state("Расшифрованный блок", block);
}


// --- Режим CBC ---

// Дополнение PKCS#7
void pad_pkcs7(vector<uint8_t>& data) {
    size_t pad_len = 16 - (data.size() % 16);
    if (pad_len == 0) pad_len = 16;
    for (size_t i = 0; i < pad_len; ++i) {
        data.push_back(static_cast<uint8_t>(pad_len));
    }
}

// Удаление дополнения PKCS#7
bool unpad_pkcs7(vector<uint8_t>& data) {
    if (data.empty()) return false;
    uint8_t pad_len = data.back();
    if (pad_len == 0 || pad_len > 16 || pad_len > data.size()) return false;
    for (size_t i = 0; i < pad_len; ++i) {
        if (data[data.size() - 1 - i] != pad_len) return false;
    }
    data.resize(data.size() - pad_len);
    return true;
}


vector<uint8_t> aes_cbc_encrypt(const vector<uint8_t>& plaintext, const uint8_t* key, const uint8_t* iv) {
    uint8_t expanded_keys[176];
    key_expansion(key, expanded_keys);

    cout << "\n>>> ГЕНЕРАЦИЯ РАУНДОВЫХ КЛЮЧЕЙ (для шифрования) <<<\n";
    for (int i = 0; i < 11; ++i) {
        print_hex("Ключ раунда " + to_string(i), expanded_keys + i * 16, 16);
    }

    vector<uint8_t> padded_plaintext = plaintext;
    pad_pkcs7(padded_plaintext);

    vector<uint8_t> ciphertext;
    uint8_t prev_block[16];
    copy(iv, iv + 16, prev_block);

    cout << "\n>>> ПРОЦЕСС ШИФРОВАНИЯ CBC <<<\n";
    for (size_t i = 0; i < padded_plaintext.size(); i += 16) {
        cout << "\n--- Шифрование блока " << (i / 16) + 1 << " ---\n";
        uint8_t current_block[16];
        copy(padded_plaintext.begin() + i, padded_plaintext.begin() + i + 16, current_block);

        // XOR с предыдущим зашифрованным блоком (или IV)
        for (int j = 0; j < 16; ++j) {
            current_block[j] ^= prev_block[j];
        }

        encrypt_block(current_block, expanded_keys);

        ciphertext.insert(ciphertext.end(), current_block, current_block + 16);
        copy(current_block, current_block + 16, prev_block);
    }
    return ciphertext;
}

vector<uint8_t> aes_cbc_decrypt(const vector<uint8_t>& ciphertext, const uint8_t* key, const uint8_t* iv) {
    uint8_t expanded_keys[176];
    key_expansion(key, expanded_keys);

    cout << "\n>>> ГЕНЕРАЦИЯ РАУНДОВЫХ КЛЮЧЕЙ (для дешифрования) <<<\n";
    for (int i = 0; i < 11; ++i) {
        print_hex("Ключ раунда " + to_string(i), expanded_keys + i * 16, 16);
    }

    vector<uint8_t> plaintext;
    uint8_t prev_block[16];
    copy(iv, iv + 16, prev_block);

    cout << "\n>>> ПРОЦЕСС ДЕШИФРОВАНИЯ CBC <<<\n";
    for (size_t i = 0; i < ciphertext.size(); i += 16) {
        cout << "\n--- Дешифрование блока " << (i / 16) + 1 << " ---\n";
        uint8_t current_block[16];
        copy(ciphertext.begin() + i, ciphertext.begin() + i + 16, current_block);

        decrypt_block(current_block, expanded_keys);

        // XOR с предыдущим зашифрованным блоком (или IV)
        for (int j = 0; j < 16; ++j) {
            current_block[j] ^= prev_block[j];
        }

        plaintext.insert(plaintext.end(), current_block, current_block + 16);
        copy(ciphertext.begin() + i, ciphertext.begin() + i + 16, prev_block);
    }

    if (!unpad_pkcs7(plaintext)) {
        cerr << "Ошибка: неверное дополнение (padding)!" << endl;
    }

    return plaintext;
}


// --- Главная функция ---

int main() {
    setlocale(LC_ALL, "Russian");

    // 1. Исходные данные
    //string plain_text_str = "Тестовое сообщение для AES CBC. Test message for AES CBC!";
    string plain_text_str = "Я люблю илюшу он плюшка";
    vector<uint8_t> plaintext(plain_text_str.begin(), plain_text_str.end());

    // 2. Генерация случайного ключа и вектора инициализации (IV)
    uint8_t key[16];
    uint8_t iv[16];
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, 255);
    for (int i = 0; i < 16; ++i) {
        key[i] = distrib(gen);
        iv[i] = distrib(gen);
    }

    // 3. Запись ключа в файл
    ofstream key_file("key.txt");
    if (key_file.is_open()) {
        for (int i = 0; i < 16; ++i) {
            key_file << hex << setfill('0') << setw(2) << static_cast<int>(key[i]);
        }
        key_file << endl;
        key_file.close();
        cout << "Случайный ключ был сгенерирован и записан в key.txt" << endl;
    }
    else {
        cerr << "Не удалось открыть файл key.txt для записи." << endl;
    }


    cout << "====================================================================\n";
    cout << "                    AES-128 CBC ДЕМОНСТРАЦИЯ                      \n";
    cout << "====================================================================\n\n";

    cout << "Исходный текст: " << plain_text_str << endl;
    print_hex("Исходный ключ (128 бит)", key, 16);
    print_hex("Вектор инициализации (IV)", iv, 16);
    cout << "\n--------------------------------------------------------------------\n";

    // 4. Шифрование
    vector<uint8_t> ciphertext = aes_cbc_encrypt(plaintext, key, iv);
    cout << "\n================== РЕЗУЛЬТАТ ШИФРОВАНИЯ ==================\n";
    cout << "Зашифрованный текст (HEX):" << endl;
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        cout << hex << setfill('0') << setw(2) << static_cast<int>(ciphertext[i]) << " ";
        if ((i + 1) % 16 == 0) cout << endl;
    }
    cout << dec << "\n--------------------------------------------------------------------\n";
    int a;
    cin >> a;

    // 5. Дешифрование
    vector<uint8_t> decryptedtext = aes_cbc_decrypt(ciphertext, key, iv);
    string decrypted_str(decryptedtext.begin(), decryptedtext.end());
    cout << "\n================== РЕЗУЛЬТАТ ДЕШИФРОВАНИЯ ==================\n";
    cout << "Расшифрованный текст: " << decrypted_str << endl;

    cout << "\n====================================================================\n";

    // 6. Проверка
    if (plain_text_str == decrypted_str) {
        cout << "ПРОВЕРКА ПРОЙДЕНА: Исходный и расшифрованный тексты совпадают." << endl;
    }
    else {
        cout << "ПРОВЕРКА НЕ ПРОЙДЕНА: Тексты не совпадают!" << endl;
    }

    return 0;
}