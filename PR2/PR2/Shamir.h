#pragma once

#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <random>
#include "ExtendedEuclidean.h";

using namespace std;

vector<uint32_t> encrypt(const uint32_t cA, const uint32_t cB, const uint32_t p, const vector<uint8_t>& message);

vector<uint8_t> decrypt(const uint32_t cA, const uint32_t cB, const uint32_t p, vector<uint32_t> cyphertext);