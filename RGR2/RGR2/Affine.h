#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

int Gcd(int firstNum, int secondNum);

int ExtendedEuclidean(int firstNum, int secondNum);

int modInverse(int a, int m);

vector<uint8_t> encrypt(string plaintext, int a, int b);

string decrypt(vector<uint8_t> cipher, int a, int b);