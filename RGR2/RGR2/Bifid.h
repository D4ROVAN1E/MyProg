#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

uint8_t get_row_from_char(uint8_t char_code);

uint8_t get_col_from_char(uint8_t char_code);

char get_char_from_coords(uint8_t row, uint8_t col);

string bifid_encrypt(const string& plaintext);

string bifid_decrypt(const string& ciphertext);