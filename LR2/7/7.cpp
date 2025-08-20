#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// Функция проверяет корректность входной строки
// Формат строки: чередующиеся символы цвета (R, B, W) и цифры (0-9)
bool correctInput(const string& input) {
    for (size_t i = 1; i < input.size(); i += 2) {
        int rod = input[i] - '0';  // Преобразование символа цифры в число
        int color = input[i - 1];  // Получение символа цвета
        if (rod >= 0 && rod <= 9) {
            if (color != 'R' && color != 'B' && color != 'W')
                return false;  // Проверка допустимых цветов
        }
        else return false;  // Проверка допустимых номеров стержней
    }
    return true;
}

// Функция создает двумерный массив данных из входной строки
// Первое измерение - номер стержня (0-9)
// Второе измерение - цвета (R, W, B)
vector<vector<bool>> makeData(const string& input) {
    vector<vector<bool>> result(10, vector<bool>(3, false));  // Инициализация массива 10x3
    for (size_t i = 1; i < input.size(); i += 2) {
        int rod = input[i] - '0';  // Номер стержня
        char color = input[i - 1];  // Цвет
        switch (color) {
        case 'R': result[rod][0] = true; break;  // Красный цвет
        case 'W': result[rod][1] = true; break;  // Белый цвет
        case 'B': result[rod][2] = true; break;  // Синий цвет
        }
    }
    return result;
}

// Функция подсчитывает количество стержней, на которых есть все три цвета
int countRods(const vector<vector<bool>>& data) {
    int sum = 0;

    for (vector<bool> rods : data) {
        if (count(rods.begin(), rods.end(), true) == 3)  // Проверка наличия всех трех цветов
            sum++;
    }
    return sum;
}

int main() {
    vector<vector<bool>> data;
    cout << "Input: ";
    string input;
    cin >> input;
    if (!correctInput(input)) {
        cout << "Invalid input";
        return 0;
    }
    cout << countRods(makeData(input)) << endl;
    return 0;
}