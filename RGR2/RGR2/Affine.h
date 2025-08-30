#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

int Gcd(int firstNum, int secondNum);

int ExtendedEuclidean(int firstNum, int secondNum);

int modInverse(int a, int m);

vector<uint8_t> affine_encrypt(const vector<uint8_t>& plaintext, int a, int b);

vector<uint8_t> affine_decrypt(const vector<uint8_t>& cipher, int a, int b);