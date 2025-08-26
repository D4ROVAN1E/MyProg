#pragma once

#include <iostream>
#include <vector>
#include <iomanip> 
#include "ModuloComparison.h"

using namespace std;

pair<int, int> solve_diophantine(int orig_a, int orig_b, int c);

void calculate_convergents(const vector<int>& coeffs, vector<int>& P, vector<int>& Q);

vector<int> get_continued_fraction(int a, int b);