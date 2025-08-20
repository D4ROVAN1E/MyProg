#include <iostream> 
#include <string>  
using namespace std;

int main() {
    int N, M;        // Переменные для размеров доски (N - строки, M - столбцы)
    string S;        // Строка с командами
    cin >> N >> M >> S;

    // Проверка на некорректные размеры доски
    if (N < 1 || M < 1) {
        cout << "Invalid input" << endl; 
        return 0; 
    }

    // Проверка символов в строке команд на допустимость
    for (char c : S) {
        if (c != 'L' && c != 'R' && c != 'U' && c != 'D') {
            cout << "Invalid input" << endl; 
            return 0; 
        }
    }
    int min_x = 1, min_y = 1;
    // Подсчёт количества команд каждого типа
    int L = 0, R = 0, U = 0, D = 0; // Счётчики для команд
    for (char c : S) {
	    switch(c) {
		    case 'L': 
            L++; 
            if (min_x - L < 1) min_x++;
            if (R > 0) R--;
            break;
		    case 'R': 
            R++; 
            if (L > 0) L--;
            break;
		    case 'U': 
            U++; 
            if (min_y - U < 1) min_y++;
            if (D > 0) D--;
            break;
		    case 'D': 
            D++; 
            if (U > 0) U--;
            break;
	    }
    }



    // Проверка существования допустимой начальной позиции
    if (min_x > M || min_y > N) {
        cout << "No" << endl; 
    } else {
        // Вывод минимальных координат из допустимого диапазона (самая левая верхняя клетка)
        cout << "(" << min_x << "," << min_y << ")" << endl;
    }

    return 0; 
}
