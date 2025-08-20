#include <iostream>   
#include <string>     
#include <cctype>     // Для функции isdigit, используемой в проверке символов.
#include <cstdlib>    // Для функции stoi, преобразующей строку в число.

using namespace std;

bool correctDate(int day, int month) {
    return day >= 1 && day <= 31 && month >= 1 && month <= 12;
}

int main() {
    string token; 
    int nums;
    cin >> nums;
    // Цикл считывания входных данных до конца файла (EOF).
    for(int i = 0; i < nums; i++) {
        cin >> token;
        bool validInput = true; // Флаг корректности входного элемента.
        
        // проходим по каждому символу и убеждаемся, что это цифра.
        for (char c : token) {
            int symbol = c - '0';
            if (symbol < 0 || symbol > 9) {
                validInput = false;  
                break;
            }
        }
        
        // Если входная строка не прошла проверку или содержит менее двух цифр, то вставить '/' невозможно.
        if (!validInput || token.size() < 2) {
            cout << "Invalid input\n";
            continue; 
        }
        
        bool isCorrectDate = false; 
        int number = stoi(token);
        
        if (number >= 11 && number < 100) {
            int day = number / 10;
            int month = number % 10;
            isCorrectDate = correctDate(day, month);
        } else if (number >= 100 && number < 1000) {
            int day = number / 10;
            int month = number % 10;
            isCorrectDate = correctDate(day, month);
            if (!isCorrectDate) {
                int day = number / 100;
                int month = number % 100;
                isCorrectDate = correctDate(day, month);
            }
        } else if (number >= 1000 && number <= 3112) {
            int day = number / 100;
            int month = number % 100;
            isCorrectDate = correctDate(day, month);
        }

        cout << (isCorrectDate ? "YES" : "NO");
    }
    
    return 0; // Завершаем программу.
}
