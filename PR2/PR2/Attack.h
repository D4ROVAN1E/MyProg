#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "ExtendedEuclidean.h"
#include "Shamir.h"

using namespace std;

void meet_in_the_middle_attack(const uint32_t p, const uint8_t M, const uint32_t C, const uint32_t key_limit);

