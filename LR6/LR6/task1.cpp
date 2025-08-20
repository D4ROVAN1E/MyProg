#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <cstring>
#include <thread>
#include <chrono>
#include <random> 

using namespace std;

const int MAX_SIZE = 100;
const int GRID_SIZE = 50;

// Функция для очистки экрана
void clearScreen() {
    system("clear || cls");
}

// Задача 1: Квадратная матрица N×N, найти строку с наибольшим количеством положительных чисел
void task1() {
    cout << "=== Задача 1 ===" << endl;

    int n;
    cout << "Введите порядок квадратной матрицы N: ";
    cin >> n;

    if (n <= 0 || n > MAX_SIZE) {
        cout << "Неверный размер матрицы!" << endl;
        return;
    }

    int matrix[MAX_SIZE][MAX_SIZE];

    // Инициализация матрицы случайными числами [-100, 100]
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> distrib(-100, 100);
    cout << "\nСгенерированная матрица:" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = distrib(gen);
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    }

    // Поиск строки с наибольшим количеством положительных чисел
    int maxPositiveCount = 0;
    int bestRow = 0;

    for (int i = 0; i < n; i++) {
        int positiveCount = 0;
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] > 0) {
                positiveCount++;
            }
        }

        cout << "Строка " << i << ": положительных элементов = " << positiveCount << endl;

        if (positiveCount > maxPositiveCount) {
            maxPositiveCount = positiveCount;
            bestRow = i;
        }
    }

    // Создание нового массива с найденной строкой
    int newArray[MAX_SIZE];
    for (int j = 0; j < n; j++) {
        newArray[j] = matrix[bestRow][j];
    }

    cout << "\nСтрока с наибольшим количеством положительных чисел (строка " << bestRow
        << ", " << maxPositiveCount << " положительных): ";
    for (int j = 0; j < n; j++) {
        cout << newArray[j] << " ";
    }
    cout << endl << endl;
}

// Задача 2: Матрица M×N, найти число с максимальной частотой
void task2() {
    cout << "=== Задача 2 ===" << endl;

    int m, n;
    cout << "Введите размеры матрицы MxN: ";
    cin >> m >> n;

    if (m <= 0 || n <= 0 || m > MAX_SIZE || n > MAX_SIZE) {
        cout << "Неверные размеры матрицы!" << endl;
        return;
    }

    int matrix[MAX_SIZE][MAX_SIZE];
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> distrib(100, 150);
    // Инициализация матрицы случайными числами [100, 150]
    cout << "\nСгенерированная матрица:" << endl;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = distrib(gen);
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    }

    // Подсчет частоты каждого числа
    int frequency[151]; // Для чисел от 100 до 150
    memset(frequency, 0, sizeof(frequency));

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            frequency[matrix[i][j]]++;
        }
    }

    // Поиск числа с максимальной частотой
    int maxFreq = 0;
    int mostFrequentNum = -1;
    bool hasRepeated = false;

    for (int i = 100; i <= 150; i++) {
        if (frequency[i] > maxFreq) {
            maxFreq = frequency[i];
            mostFrequentNum = i;
        }
        if (frequency[i] > 1) {
            hasRepeated = true;
        }
    }

    if (!hasRepeated) {
        cout << "\nВ матрице нет чисел, которые встречаются более одного раза." << endl;
    }
    else {
        cout << "\nЧисло " << mostFrequentNum << " встречается наибольшее количество раз: "
            << maxFreq << " раз(а)" << endl;
    }
    cout << endl;
}

// Функции для клеточного автомата Конвея
int countNeighbors(bool grid[GRID_SIZE][GRID_SIZE], int row, int col, bool toroidal = false) {
    int count = 0;
    int directions[8][2] = { {-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,-1}, {1,0}, {1,1} };

    for (int i = 0; i < 8; i++) {
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];

        if (toroidal) {
            // Замкнутая плоскость (тор)
            newRow = (newRow + GRID_SIZE) % GRID_SIZE;
            newCol = (newCol + GRID_SIZE) % GRID_SIZE;
            if (grid[newRow][newCol]) count++;
        }
        else {
            // Ограниченная плоскость
            if (newRow >= 0 && newRow < GRID_SIZE && newCol >= 0 && newCol < GRID_SIZE) {
                if (grid[newRow][newCol]) count++;
            }
        }
    }
    return count;
}

void updateGrid(bool current[GRID_SIZE][GRID_SIZE], bool next[GRID_SIZE][GRID_SIZE], bool toroidal = false, bool customRules = false) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int neighbors = countNeighbors(current, i, j, toroidal);

            if (customRules) {
                // Собственные правила: живая клетка выживает при 2-4 соседях, рождается при 3-4 соседях
                if (current[i][j]) {
                    next[i][j] = (neighbors >= 2 && neighbors <= 4);
                }
                else {
                    next[i][j] = (neighbors == 3 || neighbors == 4);
                }
            }
            else {
                // Классические правила Конвея
                if (current[i][j]) {
                    next[i][j] = (neighbors == 2 || neighbors == 3);
                }
                else {
                    next[i][j] = (neighbors == 3);
                }
            }
        }
    }
}

void printGrid(bool grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            cout << (grid[i][j] ? "1" : "0");
        }
        cout << endl;
    }
}

void initializeGlider(bool grid[GRID_SIZE][GRID_SIZE], int startRow, int startCol) {
    // Очищаем сетку
    memset(grid, false, GRID_SIZE * GRID_SIZE * sizeof(bool));

    // Глайдер
    if (startRow + 2 < GRID_SIZE && startCol + 2 < GRID_SIZE) {
        grid[startRow][startCol + 1] = true;
        grid[startRow + 1][startCol + 2] = true;
        grid[startRow + 2][startCol] = true;
        grid[startRow + 2][startCol + 1] = true;
        grid[startRow + 2][startCol + 2] = true;
    }
}

void initializeRandomColony(bool grid[GRID_SIZE][GRID_SIZE], double density = 0.3) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> distrib(0, 1);
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = distrib(gen);
        }
    }
}

void task3() {
    cout << "=== Задача 3: Клеточный автомат Конвея ===" << endl;

    bool grid1[GRID_SIZE][GRID_SIZE], grid2[GRID_SIZE][GRID_SIZE];
    bool (*currentGrid)[GRID_SIZE] = grid1;
    bool (*nextGrid)[GRID_SIZE] = grid2;

    int choice;
    cout << "Выберите демонстрацию:" << endl;
    cout << "1 - Глайдер (классические правила, замкнутая плоскость)" << endl;
    cout << "2 - Случайная колония (классические правила)" << endl;
    cout << "3 - Случайная колония (собственные правила)" << endl;
    cout << "Ваш выбор: ";
    cin >> choice;

    switch (choice) {
    case 1:
        cout << "\nДемонстрация глайдера на замкнутой плоскости:" << endl;
        initializeGlider(currentGrid, 5, 5);
        break;
    case 2:
        cout << "\nДемонстрация случайной колонии (классические правила):" << endl;
        initializeRandomColony(currentGrid);
        break;
    case 3:
        cout << "\nДемонстрация случайной колонии (собственные правила):" << endl;
        initializeRandomColony(currentGrid);
        break;
    default:
        cout << "Неверный выбор!" << endl;
        return;
    }

    cout << "Нажмите Enter для начала симуляции...";
    cin.ignore();
    cin.get();

    // Симуляция
    for (int generation = 0; generation < 100; generation++) {
        clearScreen();
        cout << "Поколение: " << generation << endl;
        cout << "Правила: " << (choice == 3 ? "Собственные" : "Классические Конвея") << endl;
        cout << "Плоскость: " << (choice == 1 ? "Замкнутая" : "Ограниченная") << endl;
        cout << endl;

        printGrid(currentGrid);

        // Обновляем сетку
        bool toroidal = (choice == 1);
        bool customRules = (choice == 3);
        updateGrid(currentGrid, nextGrid, toroidal, customRules);

        // Меняем указатели
        bool (*temp)[GRID_SIZE] = currentGrid;
        currentGrid = nextGrid;
        nextGrid = temp;

        // Задержка
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    cout << "\nСимуляция завершена." << endl << endl;
}

int main() {
    setlocale(LC_ALL, "ru");
    cout << "Программа для варианта 12" << endl << endl;

    int choice;
    do {
        cout << "Выберите задачу:" << endl;
        cout << "1 - Задача 1 (Квадратная матрица, строка с максимумом положительных)" << endl;
        cout << "2 - Задача 2 (Матрица MxN, число с максимальной частотой)" << endl;
        cout << "3 - Задача 3 (Клеточный автомат Конвея)" << endl;
        cout << "0 - Выход" << endl;
        cout << "Ваш выбор: ";
        cin >> choice;

        switch (choice) {
        case 1:
            task1();
            break;
        case 2:
            task2();
            break;
        case 3:
            task3();
            break;
        case 0:
            cout << "До свидания!" << endl;
            break;
        default:
            cout << "Неверный выбор!" << endl;
        }

        if (choice != 0) {
            cout << "Нажмите Enter для продолжения...";
            cin.ignore();
            cin.get();
        }

    } while (choice != 0);

    return 0;
}