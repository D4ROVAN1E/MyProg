#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

using namespace std;

// Функция для вычисления значения кусочно-заданной функции
double calculateFunction(double x) {
    if (x >= -10 && x <= -8) {
        return -3;
    }
    else if (x > -8 && x <= -3) {
        return (3.0 / 5.0) * (x + 3);
    }
    else if (x > -3 && x <= 3) {
        return -sqrt(9 - x * x);
    }
    else if (x > 3 && x <= 5) {
        return x - 3;
    }
    else if (x > 5 && x <= 8) {
        return 3;
    }
    else {
        // Если x не входит в область определения
        return 0;
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    double xStart = -10.0;  // Начальное значение x
    double xEnd = 8.0;      // Конечное значение x
    double dx = 0.5;        // Шаг изменения x

    cout << "Таблица значений кусочно-заданной функции:" << endl;
    cout << "==========================================" << endl;
    cout << setw(8) << "x" << setw(12) << "y" << setw(20) << "Ветвь функции" << endl;
    cout << "==========================================" << endl;

    vector<pair<double, double>> results;

    // Циклический процесс вычисления
    for (double x = xStart; x <= xEnd; x += dx) {
        double y = calculateFunction(x);
        results.push_back(make_pair(x, y));

        // Определение текущей ветви для вывода
        string branch;
        if (x >= -10 && x <= -8) {
            branch = "y = -3";
        }
        else if (x > -8 && x <= -3) {
            branch = "y = 3/5*(x+3)";
        }
        else if (x > -3 && x <= 3) {
            branch = "y = -sqrt(9-x^2)";
        }
        else if (x > 3 && x <= 5) {
            branch = "y = x-3";
        }
        else if (x > 5 && x <= 8) {
            branch = "y = 3";
        }
        else {
            branch = "Не определена";
        }

        cout << fixed << setprecision(1);
        cout << setw(8) << x << setw(12) << setprecision(4) << y
            << setw(20) << branch << endl;
    }

    cout << "==========================================" << endl;
    cout << "Всего вычислено точек: " << results.size() << endl;

    return 0;
}