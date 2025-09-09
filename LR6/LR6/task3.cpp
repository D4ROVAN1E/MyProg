#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

// Константа для размера матрицы
const int N = 4;

// Функция для вывода вектора
void printVector(const vector<double>& vec) {
    for (int i = 0; i < N; ++i) {
        cout << "x" << i + 1 << " = " << fixed << setprecision(8) << vec[i] << endl;
    }
}

// Функция для выполнения LU-разложения матрицы A
// A = L * U, где L - нижняя треугольная, U - верхняя треугольная
bool luDecomposition(const vector<vector<double>>& A, vector<vector<double>>& L, vector<vector<double>>& U) {
    for (int i = 0; i < N; i++) {
        // Вычисление верхней треугольной матрицы U
        for (int k = i; k < N; k++) {
            double sum = 0;
            for (int j = 0; j < i; j++) {
                sum += (L[i][j] * U[j][k]);
            }
            U[i][k] = A[i][k] - sum;
        }

        // Вычисление нижней треугольной матрицы L
        for (int k = i; k < N; k++) {
            if (i == k) {
                L[i][i] = 1; // Диагональные элементы L равны 1
            }
            else {
                double sum = 0;
                for (int j = 0; j < i; j++) {
                    sum += (L[k][j] * U[j][i]);
                }
                // Проверка деления на ноль
                if (abs(U[i][i]) < 1e-9) {
                    cerr << "Ошибка: Деление на ноль при LU-разложении. Метод не применим." << endl;
                    return false;
                }
                L[k][i] = (A[k][i] - sum) / U[i][i];
            }
        }
    }
    return true;
}

// Функция для решения системы Ax = b, используя LU-разложение
vector<double> solveWithLU(const vector<vector<double>>& A, const vector<double>& b) {
    vector<vector<double>> L(N, vector<double>(N, 0));
    vector<vector<double>> U(N, vector<double>(N, 0));
    vector<double> x(N, 0);
    vector<double> y(N, 0);

    if (!luDecomposition(A, L, U)) {
        return {}; // Возвращаем пустой вектор в случае ошибки
    }

    // Решаем систему Ly = b (прямой ход)
    for (int i = 0; i < N; i++) {
        double sum = 0;
        for (int j = 0; j < i; j++) {
            sum += L[i][j] * y[j];
        }
        y[i] = b[i] - sum;
    }

    // Решаем систему Ux = y (обратный ход)
    for (int i = N - 1; i >= 0; i--) {
        double sum = 0;
        for (int j = i + 1; j < N; j++) {
            sum += U[i][j] * x[j];
        }
        if (abs(U[i][i]) < 1e-9) {
            cerr << "Ошибка: Матрица является вырожденной." << endl;
            return {};
        }
        x[i] = (y[i] - sum) / U[i][i];
    }

    return x;
}

// Функция для проверки условия сходимости (диагональное преобладание)
// Возвращает true, если условие выполняется, иначе false.
bool checkConvergence(const vector<vector<double>>& A) {
    bool isConvergent = true;
    for (int i = 0; i < N; i++) {
        double diag_element = abs(A[i][i]);
        double sum_off_diag = 0;
        for (int j = 0; j < N; j++) {
            if (i != j) {
                sum_off_diag += abs(A[i][j]);
            }
        }
        if (diag_element < sum_off_diag) {
            cout << "Предупреждение: Для строки " << i + 1 << " не выполняется условие диагонального преобладания." << endl;
            isConvergent = false;
        }
    }
    return isConvergent;
}


// Функция для решения системы методом Зейделя
vector<double> seidelMethod(const vector<vector<double>>& A, const vector<double>& b, double epsilon) {
    vector<double> x(N, 0.0); // Начальное приближение - нулевой вектор
    vector<double> x_prev(N, 0.0);
    int iterations = 0;
    const int max_iterations = 1000;

    cout << "\n--- Процесс итераций (метод Зейделя) ---\n";
    cout << "----------------------------------------------------------------------------------------\n";
    cout << setw(5) << "N" << setw(18) << "X1" << setw(18) << "X2"
        << setw(18) << "X3" << setw(18) << "X4" << setw(18) << "Epsilon_n" << endl;
    cout << "----------------------------------------------------------------------------------------\n";


    do {
        x_prev = x;
        for (int i = 0; i < N; i++) {
            double sum1 = 0;
            for (int j = 0; j < i; j++) {
                sum1 += A[i][j] * x[j];
            }
            double sum2 = 0;
            for (int j = i + 1; j < N; j++) {
                sum2 += A[i][j] * x_prev[j];
            }
            if (abs(A[i][i]) < 1e-9) {
                cerr << "Ошибка: Нулевой диагональный элемент. Метод Зейделя не применим." << endl;
                return {};
            }
            x[i] = (b[i] - sum1 - sum2) / A[i][i];
        }

        // Вычисляем норму разности векторов для проверки сходимости
        double norm = 0;
        for (int i = 0; i < N; i++) {
            norm += (x[i] - x_prev[i]) * (x[i] - x_prev[i]);
        }
        double current_epsilon = sqrt(norm);

        iterations++;

        // Вывод текущей итерации в таблицу
        cout << setw(5) << iterations
            << fixed << setprecision(8) << setw(18) << x[0] << setw(18) << x[1]
            << setw(18) << x[2] << setw(18) << x[3] << setw(18) << current_epsilon << endl;

        if (current_epsilon < epsilon) {
            break;
        }

    } while (iterations < max_iterations);

    cout << "----------------------------------------------------------------------------------------\n";

    if (iterations == max_iterations) {
        cout << "\nПредупреждение: Достигнуто максимальное число итераций. Решение может быть неточным." << endl;
    }

    return x;
}

int main() {
     setlocale(LC_ALL, "Russian");

    // Исходные данные для варианта 11 (M=0.89, N=0.08, P=-1.21)
    vector<vector<double>> A = {
        {0.89, -0.04, 0.21, -18.00},
        {0.25, -1.23, 0.08, -0.09},
        {-0.21, 0.08, 0.8, -0.13},
        {0.15, -1.31, 0.06, -1.21}
    };

    vector<double> b = { -1.24, -1.21, 2.56, 0.89 };

    double epsilon = 1e-3;

    // --- Решение прямым методом (LU-разложение) ---
    cout << "1. Прямой метод: LU-разложение" << endl;
    vector<double> x_lu = solveWithLU(A, b);
    if (!x_lu.empty()) {
        cout << "\nНайденные корни системы:" << endl;
        printVector(x_lu);
    }

    cout << "\n=================================================================================\n" << endl;

    // --- Решение итерационным методом (метод Зейделя) ---
    cout << "2. Итерационный метод: Метод Зейделя" << endl;

    // Проверка сходимости для исходной матрицы
    if (checkConvergence(A)) {

        vector<double> x_seidel = seidelMethod(A, b, epsilon);
        if (!x_seidel.empty()) {
            cout << "\nНайденные корни системы (с точностью Epsilon = " << epsilon << "):" << endl;
            printVector(x_seidel);
        }
    }

    return 0;
}