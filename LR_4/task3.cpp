/**
 * Реализация Алгоритма Банкира (Banker's Algorithm) на C++.
 * Тема: Многопоточность и предотвращение взаимных блокировок (Deadlock Avoidance).
 *
 * Описание:
 * Программа проверяет, находится ли система в "безопасном состоянии" (safe state).
 * Система находится в безопасном состоянии, если существует такая последовательность
 * выполнения процессов, при которой каждый процесс может получить свои максимальные
 * требуемые ресурсы, завершиться и вернуть ресурсы обратно в систему.
 *
 * Основные структуры данных:
 * - Available: Вектор доступных ресурсов.
 * - Maximum: Матрица максимальных потребностей каждого процесса.
 * - Allocation: Матрица ресурсов, уже выделенных каждому процессу.
 * - Need: Матрица оставшихся потребностей (Need = Maximum - Allocation).
 */

#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

class BankersAlgorithm {
private:
    int numProcesses; // Количество процессов
    int numResources; // Количество типов ресурсов

    // Матрицы и векторы состояния системы
    vector<vector<int>> allocation;
    vector<vector<int>> maxDemand;
    vector<vector<int>> need;
    vector<int> available;
    vector<int> safeSequence;

public:
    // Конструктор: инициализирует размеры и структуры данных
    BankersAlgorithm(int p, int r) : numProcesses(p), numResources(r) {
        allocation.resize(numProcesses, vector<int>(numResources));
        maxDemand.resize(numProcesses, vector<int>(numResources));
        need.resize(numProcesses, vector<int>(numResources));
        available.resize(numResources);
    }

    // Метод для загрузки данных с выбором сценария
    void loadExampleData(int scenario) {
        // P0, P1, P2, P3, P4
        // Ресурсы, выделенные в данный момент (Allocation)
        allocation = {
            {0, 1, 0}, // P0
            {2, 0, 0}, // P1
            {3, 0, 2}, // P2
            {2, 1, 1}, // P3
            {0, 0, 2}  // P4
        };

        // Максимальная потребность процессов (Max)
        maxDemand = {
            {7, 5, 3}, // P0
            {3, 2, 2}, // P1
            {9, 0, 2}, // P2
            {2, 2, 2}, // P3
            {4, 3, 3}  // P4
        };

        if (scenario == 1) {
            available = {3, 3, 2}; 
            cout << "\n>>> Загружен Сценарий 1: Достаточно ресурсов (безопасный) <<<\n";
        } else {
            available = {1, 1, 1}; 
            cout << "\n>>> Загружен Сценарий 2: Недостаточно ресурсов (Небезопасный) <<<\n";
        }

        calculateNeed();
    }

    // Вычисление матрицы потребностей (Need = Max - Allocation)
    void calculateNeed() {
        for (int i = 0; i < numProcesses; i++) {
            for (int j = 0; j < numResources; j++) {
                need[i][j] = maxDemand[i][j] - allocation[i][j];
            }
        }
    }

    // Основной метод проверки безопасности состояния
    bool isSystemSafe() {
        // Work - это копия доступных ресурсов, которую мы будем менять в симуляции
        vector<int> work = available;
        
        // Finish - флаг, завершил ли процесс выполнение
        vector<bool> finish(numProcesses, false);
        
        safeSequence.clear();

        int count = 0; // Количество завершенных процессов в симуляции

        // Пытаемся найти безопасную последовательность
        while (count < numProcesses) {
            bool found = false;

            for (int p = 0; p < numProcesses; p++) {
                // Если процесс еще не завершен
                if (!finish[p]) {
                    // Проверяем, может ли текущий Need быть удовлетворен текущим Work
                    // Need[p] <= Work
                    bool canProceed = true;
                    for (int j = 0; j < numResources; j++) {
                        if (need[p][j] > work[j]) {
                            canProceed = false;
                            break;
                        }
                    }

                    // Если ресурсов достаточно:
                    if (canProceed) {
                        // Предполагаем, что процесс выполняется
                        // После завершения он освобождает свои ресурсы (Allocation)
                        for (int k = 0; k < numResources; k++) {
                            work[k] += allocation[p][k];
                        }

                        // Добавляем процесс в безопасную последовательность
                        safeSequence.push_back(p);
                        finish[p] = true;
                        found = true;
                        count++;
                    }
                }
            }

            // Если за полный проход мы не нашли ни одного процесса, который можно запустить,
            // значит система в небезопасном состоянии (возможен Deadlock).
            if (!found) {
                return false;
            }
        }

        // Если мы дошли сюда, значит все процессы успешно завершились в симуляции
        return true;
    }

    void printState() {
        cout << "\n--- Текущее состояние системы ---\n";
        cout << "Процесс | Allocation |   Max   |   Need  \n";
        cout << "        |  A  B  C   | A  B  C | A  B  C \n";
        cout << "--------|------------|---------|---------\n";

        for (int i = 0; i < numProcesses; i++) {
            cout << "   P" << i << "   |  ";
            for (int val : allocation[i]) cout << val << "  ";
            cout << " | ";
            for (int val : maxDemand[i]) cout << val << "  ";
            cout << " | ";
            for (int val : need[i]) cout << val << "  ";
            cout << "\n";
        }
        
        cout << "\nДоступные ресурсы (Available): ";
        for (int val : available) cout << val << " ";
        cout << "\n---------------------------------\n";
    }

    void printResult(bool isSafe) {
        if (isSafe) {
            cout << "\n[УСПЕХ] Система находится в безопасном состоянии.\n";
            cout << "Безопасная последовательность выполнения: ";
            for (size_t i = 0; i < safeSequence.size(); i++) {
                cout << "P" << safeSequence[i];
                if (i != safeSequence.size() - 1) cout << " -> ";
            }
            cout << "\n";
        } else {
            cout << "\n[ОШИБКА] Система находится в небезопасном состоянии! Возможна взаимная блокировка.\n";
        }
    }
};

int main() {
    // Инициализация системы: 5 процессов, 3 типа ресурсов
    BankersAlgorithm bank(5, 3);

    int choice;
    cout << "Демонстрация Алгоритма Банкира\n";
    cout << "1. Загрузить Безопасный сценарий\n";
    cout << "2. Загрузить Небезопасный сценарий\n";
    cout << "Выберите вариант (1 или 2): ";
    
    if (!(cin >> choice)) {
        choice = 1;
    }

    // Загрузка данных
    bank.loadExampleData(choice);

    // Вывод таблиц
    bank.printState();

    // Запуск алгоритма
    bool safe = bank.isSystemSafe();

    // Вывод результата
    bank.printResult(safe);

    return 0;
}