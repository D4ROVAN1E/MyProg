#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

string encryptRailFence(string text, uint32_t key);

string decryptRailFence(string cipher, uint32_t key);

