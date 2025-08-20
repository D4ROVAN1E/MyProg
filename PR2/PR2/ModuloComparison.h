#ifndef MODULO_COMPARISON_H
#define MODULO_COMPARISON_H

#include <iostream>
#include <cmath>
#include <cstdint>
#include <vector>
#include "ExtendedEuclidean.h"

using namespace std;

template <typename T>
ostream& operator<<(ostream& os, vector<T> vec);

void CheckPositive(const int64_t& inputNum);

void Input(uint32_t& firstBase, uint32_t& secondBase, uint32_t& primeNum, uint64_t& firstDegree, uint64_t& secondDegree);

bool WheelTest(const uint32_t& primeNum);

uint32_t Ferm(const uint32_t& base, uint64_t degree, const uint32_t& primeNum);

bool TheoremConditions(uint32_t base, uint32_t primeNum);

vector<uint64_t> SquaresOfBase(const uint32_t& base, const uint32_t& powersOfTwo, const uint32_t& primeNum);

vector<uint8_t> DegreeToBin(uint64_t degree);

void CheckLog(uint32_t& powersOfTwo, const uint64_t& degree);

uint32_t DecompOfDegree(uint32_t base, uint64_t degree, uint32_t primeNum);


#endif // MODULO_COMPARISON_H 
