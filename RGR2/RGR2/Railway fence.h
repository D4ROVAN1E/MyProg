#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

vector<uint8_t> railFence_encrypt(vector<uint8_t> text, uint32_t key);

vector<uint8_t> railFence_decrypt(vector<uint8_t> cipher, uint32_t key);
