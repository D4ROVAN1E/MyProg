#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

uint8_t get_row_from_char(uint8_t char_code);

uint8_t get_col_from_char(uint8_t char_code);

uint8_t get_char_from_coords(uint8_t row, uint8_t col);

vector<uint8_t> bifid_encrypt(const vector<uint8_t>& plaintext);

vector<uint8_t> bifid_decrypt(const vector<uint8_t>& ciphertext);