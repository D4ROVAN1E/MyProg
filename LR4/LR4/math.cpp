#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

using namespace std;

// Исходная функция f(x) = cos(x) - (x-1)²
double f(double x) {
    return cos(x) - pow(x - 1, 2);
}

// Первая производная f'(x) = -sin(x) - 2(x-1)
double df(double x) {
    return -sin(x) - 2 * (x - 1);
}

// Вторая производная f''(x) = -cos(x) - 2
double d2f(double x) {
    return -cos(x) - 2;
}

// Функция для графического отделения корней
void findRootIntervals(vector<pair<double, double>>& intervals, double start, double end, double step) {
    cout << "\n=== ГРАФИЧЕСКОЕ ОТДЕЛЕНИЕ КОРНЕЙ ===\n";
    cout << "Поиск интервалов, где f(x) меняет знак:\n";
    cout << setw(8) << "x" << setw(15) << "f(x)" << setw(10) << "Знак" << endl;
    cout << string(33, '-') << endl;

    for (double x = start; x <= end; x += step) {
        double fx = f(x);
        cout << setw(8) << fixed << setprecision(2) << x
            << setw(15) << setprecision(6) << fx
            << setw(10) << (fx > 0 ? "+" : (fx < 0 ? "-" : "0")) << endl;

        if (x > start) {
            double prev_x = x - step;
            double prev_fx = f(prev_x);
            if (prev_fx * fx < 0) {
                intervals.push_back({ prev_x, x });
                cout << ">>> НАЙДЕН КОРЕНЬ НА ИНТЕРВАЛЕ [" << prev_x << ", " << x << "]\n";
            }
        }
    }
}

// Метод половинного деления
double bisectionMethod(double a, double b, double eps, vector<vector<double>>& table) {
    cout << "\n=== МЕТОД ПОЛОВИННОГО ДЕЛЕНИЯ ===\n";
    cout << "Начальный интервал: [" << a << ", " << b << "]\n";
    cout << "Точность: " << eps << "\n\n";

    cout << setw(5) << "N" << setw(12) << "a_n" << setw(12) << "b_n"
        << setw(12) << "c_n" << setw(12) << "f(c_n)" << setw(12) << "b_n - a_n" << endl;
    cout << string(65, '-') << endl;

    int n = 0;
    while (abs(b - a) > eps) {
        double c = (a + b) / 2.0;
        double fc = f(c);
        double interval_length = b - a;

        cout << setw(5) << n << setw(12) << fixed << setprecision(6) << a
            << setw(12) << b << setw(12) << c << setw(12) << fc
            << setw(12) << interval_length << endl;

        table.push_back({ (double)n, a, b, c, fc, interval_length });

        if (f(a) * fc < 0) {
            b = c;
        }
        else {
            a = c;
        }
        n++;
    }

    double result = (a + b) / 2.0;
    cout << "\nРезультат: x* = " << fixed << setprecision(8) << result << endl;
    cout << "Проверка: f(x*) = " << f(result) << endl;
    cout << "Количество итераций: " << n << endl;

    return result;
}

// Метод Ньютона
double newtonMethod(double x0, double eps, vector<vector<double>>& table) {
    cout << "\n=== МЕТОД НЬЮТОНА ===\n";
    cout << "Начальное приближение: x0 = " << x0 << endl;
    cout << "Точность: " << eps << "\n";

    // Проверка условия выбора начального приближения
    double fx0 = f(x0);
    double d2fx0 = d2f(x0);
    cout << "Проверка условия f(x0)*f''(x0) > 0: " << fx0 << " * " << d2fx0
        << " = " << fx0 * d2fx0 << (fx0 * d2fx0 > 0 ? " > 0 sqrt" : " ≤ 0 X") << "\n\n";

    cout << setw(5) << "N" << setw(15) << "x_n" << setw(15) << "f(x_n)"
        << setw(15) << "f'(x_n)" << setw(15) << "x_{n+1}" << setw(15) << "|x_{n+1} - x_n|" << endl;
    cout << string(80, '-') << endl;

    double x = x0;
    int n = 0;

    while (true) {
        double fx = f(x);
        double dfx = df(x);

        double x_new = x - fx / dfx;
        double diff = abs(x_new - x);

        cout << setw(5) << n << setw(15) << fixed << setprecision(8) << x
            << setw(15) << fx << setw(15) << dfx << setw(15) << x_new
            << setw(15) << diff << endl;

        table.push_back({ (double)n, x, fx, dfx, x_new, diff });

        if (diff < eps) {
            cout << "\nРезультат: x* = " << fixed << setprecision(8) << x_new << endl;
            cout << "Проверка: f(x*) = " << f(x_new) << endl;
            cout << "Количество итераций: " << n + 1 << endl;
            return x_new;
        }

        x = x_new;
        n++;

        if (n > 100) {
            cout << "Превышено максимальное количество итераций!" << endl;
            break;
        }
    }

    return x;
}

// Функция итерации (адаптивная)
double phi_adaptive(double x) {
    double fx = f(x);
    double dfx = df(x);

    // Адаптивный выбор коэффициента α
    double alpha;
    if (abs(dfx) > 1e-12) {
        alpha = 1.0 / abs(dfx);
        if (alpha > 0.5) alpha = 0.5;
        if (alpha < 0.1) alpha = 0.1;
    }
    else {
        alpha = 0.3;
    }

    return x - alpha * fx;
}

// Метод простых итераций
double simpleIterationMethod(double x0, double eps, vector<vector<double>>& table) {
    cout << "\n=== МЕТОД ПРОСТЫХ ИТЕРАЦИЙ ===\n";
    cout << "Начальное приближение: x0 = " << x0 << endl;
    cout << "Функция итерации: f(x) = x - a(x)·f(x)" << endl;
    cout << "Точность: " << eps << "\n\n";

    cout << setw(5) << "N" << setw(15) << "x_n" << setw(15) << "f(x_n)"
        << setw(15) << "α_n" << setw(15) << "x_{n+1}" << setw(15) << "|x_{n+1} - x_n|" << endl;
    cout << string(85, '-') << endl;

    double x = x0;
    int n = 0;

    while (true) {
        double fx = f(x);
        double dfx = df(x);

        // Вычисляем адаптивный коэффициент
        double alpha;
        if (abs(dfx) > 1e-12) {
            alpha = 1.0 / abs(dfx);
            if (alpha > 0.5) alpha = 0.5;
            if (alpha < 0.1) alpha = 0.1;
        }
        else {
            alpha = 0.3;
        }

        double x_new = phi_adaptive(x);
        double diff = abs(x_new - x);

        cout << setw(5) << n << setw(15) << fixed << setprecision(8) << x
            << setw(15) << fx << setw(15) << alpha << setw(15) << x_new
            << setw(15) << diff << endl;

        table.push_back({ (double)n, x, fx, alpha, x_new, diff });

        if (diff < eps) {
            cout << "\nРезультат: x* = " << fixed << setprecision(8) << x_new << endl;
            cout << "Проверка: f(x*) = " << f(x_new) << endl;
            cout << "Количество итераций: " << n + 1 << endl;
            return x_new;
        }

        x = x_new;
        n++;

        if (n > 100) {
            cout << "\nПревышено максимальное количество итераций!" << endl;
            break;
        }
    }

    return x;
}

int main() {
    setlocale(LC_ALL, "ru");
    cout << "Решение нелинейного уравнения: cos(x) - (x-1)² = 0\n";
    cout << string(60, '=') << endl;

    const double eps = 1e-4;
    vector<pair<double, double>> root_intervals;

    // Графическое отделение корней
    findRootIntervals(root_intervals, -1.0, 3.0, 0.1);

    cout << "\nНайденные интервалы с корнями:\n";
    for (const auto& interval : root_intervals) {
        cout << "[" << interval.first << ", " << interval.second << "]\n";
    }

    // Обработка каждого найденного интервала
    vector<double> all_roots;

    for (const auto& interval : root_intervals) {
        double a = interval.first;
        double b = interval.second;

        cout << "\n" << string(70, '=') << endl;
        cout << "ОБРАБОТКА ИНТЕРВАЛА [" << a << ", " << b << "]" << endl;
        cout << string(70, '=') << endl;

        // Таблицы для хранения результатов
        vector<vector<double>> bisection_table;
        vector<vector<double>> newton_table;
        vector<vector<double>> iteration_table;

        // Метод половинного деления
        double root_bisection = bisectionMethod(a, b, eps, bisection_table);
        all_roots.push_back(root_bisection);

        // Выбор начального приближения для метода Ньютона
        double x0_newton = a;
        if (f(a) * d2f(a) <= 0) {
            x0_newton = b;
        }

        // Метод Ньютона  
        double root_newton = newtonMethod(x0_newton, eps, newton_table);

        // Метод простых итераций с лучшим начальным приближением
        double x0_iteration = root_bisection;  // Используем результат метода деления
        double root_iteration = simpleIterationMethod(x0_iteration, eps, iteration_table);

        // Сравнение результатов
        cout << "\n=== СРАВНЕНИЕ МЕТОДОВ ===\n";
        cout << "Метод половинного деления: x* = " << fixed << setprecision(8) << root_bisection
            << ", итераций: " << bisection_table.size() << endl;
        cout << "Метод Ньютона:            x* = " << fixed << setprecision(8) << root_newton
            << ", итераций: " << newton_table.size() << endl;
        cout << "Метод простых итераций:   x* = " << fixed << setprecision(8) << root_iteration
            << ", итераций: " << iteration_table.size() << endl;

        cout << "\nАнализ точности:\n";
        cout << "f(x_bisection) = " << scientific << setprecision(2) << f(root_bisection) << endl;
        cout << "f(x_newton)    = " << scientific << setprecision(2) << f(root_newton) << endl;
        cout << "f(x_iteration) = " << scientific << setprecision(2) << f(root_iteration) << endl;
    }

    // Итоговые результаты
    cout << "\n" << string(50, '=') << endl;
    cout << "ИТОГОВЫЕ РЕЗУЛЬТАТЫ" << endl;
    cout << string(50, '=') << endl;
    cout << "Все найденные корни уравнения cos(x) - (x-1)² = 0:\n";

    int root_num = 1;
    for (const auto& root : all_roots) {
        cout << "x" << root_num << " = " << fixed << setprecision(8) << root
            << " (проверка: f(x) = " << setprecision(2) << scientific << f(root) << ")" << endl;
        root_num++;
    }

    return 0;
}