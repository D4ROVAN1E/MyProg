#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <iomanip>
#include <numeric>
#include <cstdint>

using namespace std;

const int CURRENT_YEAR = 2026;

enum Grade {
    JUNIOR = 0,
    MIDDLE = 1,
    SENIOR = 2
};

struct Employee {
    string fio;
    char gender;        // 'M' или 'F'
    int birthYear;      // Год рождения
    Grade grade;
};

// Структура для хранения промежуточных результатов подсчета
struct GradeStats {
    int64_t totalAgeJunior = 0;
    int64_t totalAgeMiddle = 0;
    int64_t totalAgeSenior = 0;
    
    int countJunior = 0;
    int countMiddle = 0;
    int countSenior = 0;

    // Перегрузка оператора сравнения для удобства проверки
    bool operator==(const GradeStats& other) const {
        return totalAgeJunior == other.totalAgeJunior &&
               countJunior == other.countJunior &&
               totalAgeMiddle == other.totalAgeMiddle &&
               countMiddle == other.countMiddle &&
               totalAgeSenior == other.totalAgeSenior &&
               countSenior == other.countSenior;
    }
};

// Функция для получения случайного числа в диапазоне
int getRandomInt(int min, int max) {
    static mt19937 gen(random_device{}());
    uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

// Генерация массива сотрудников
vector<Employee> generateData(size_t size) {
    vector<Employee> employees;
    employees.reserve(size);

    for (size_t i = 0; i < size; ++i) {
        Employee emp;
        emp.fio = "Employee_" + to_string(i);
        emp.gender = (getRandomInt(0, 1) == 0) ? 'M' : 'F';
        
        // Генерируем грейд
        int gradeVal = getRandomInt(0, 2);
        emp.grade = static_cast<Grade>(gradeVal);

        // Генерируем возраст в зависимости от грейда для правдоподобности
        if (emp.grade == JUNIOR) {
            emp.birthYear = CURRENT_YEAR - getRandomInt(18, 26);
        } else if (emp.grade == MIDDLE) {
            emp.birthYear = CURRENT_YEAR - getRandomInt(24, 35);
        } else {
            emp.birthYear = CURRENT_YEAR - getRandomInt(30, 60);
        }

        employees.push_back(emp);
    }
    return employees;
}

// Функция, которую выполняет поток (обработка части массива)
void worker(const vector<Employee>& data, size_t start, size_t end, GradeStats& result) {
    for (size_t i = start; i < end; ++i) {
        int age = CURRENT_YEAR - data[i].birthYear;
        
        switch (data[i].grade) {
            case JUNIOR:
                result.totalAgeJunior += age;
                result.countJunior++;
                break;
            case MIDDLE:
                result.totalAgeMiddle += age;
                result.countMiddle++;
                break;
            case SENIOR:
                result.totalAgeSenior += age;
                result.countSenior++;
                break;
        }
    }
}

void printAvg(const string& label, int64_t sum, int count) {
        if (count == 0) cout << label << ": Нет данных" << endl;
        else cout << label << ": " << fixed << setprecision(2) << (double)sum / count << " лет" << endl;
};

int main() {
    // Настройка вывода для кириллицы
    setlocale(LC_ALL, "ru");

    size_t dataSize;
    int threadCount;

    cout << "Введите размер массива данных: ";
    cin >> dataSize;

    cout << "Введите количество потоков: ";
    cin >> threadCount;

    if (threadCount < 1) threadCount = 1;

    // Генерация данных
    cout << "Генерация данных..." << endl;
    auto data = generateData(dataSize);
    cout << "Данные сгенерированы.\n" << endl;

    // Однопоточная обработка
    cout << "Запуск однопоточной обработки..." << endl;
    GradeStats singleResult;
    
    auto startSingle = chrono::high_resolution_clock::now();
    
    // Выполняем работу в одном потоке (от 0 до конца)
    worker(data, 0, dataSize, singleResult);
    
    auto endSingle = chrono::high_resolution_clock::now();
    chrono::duration<double> durationSingle = endSingle - startSingle;


    // Многопоточная обработка
    cout << "Запуск многопоточной обработки (" << threadCount << " потоков)..." << endl;
    
    vector<thread> threads;
    vector<GradeStats> threadResults(threadCount); // Вектор результатов для каждого потока
    
    size_t chunkSize = dataSize / threadCount;
    
    auto startMulti = chrono::high_resolution_clock::now();

    for (int i = 0; i < threadCount; ++i) {
        size_t start = i * chunkSize;
        size_t end = (i == threadCount - 1) ? dataSize : (i + 1) * chunkSize; // Последний поток берет остаток
        
        // Запускаем поток, передавая ему ссылку на его личную структуру результатов
        // Это позволяет избежать мьютексов внутри цикла, что критично для скорости
        threads.emplace_back(worker, cref(data), start, end, ref(threadResults[i]));
    }

    // Ожидаем завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }

    // Агрегация результатов от всех потоков
    GradeStats multiResult;
    for (const auto& res : threadResults) {
        multiResult.totalAgeJunior += res.totalAgeJunior;
        multiResult.countJunior += res.countJunior;
        multiResult.totalAgeMiddle += res.totalAgeMiddle;
        multiResult.countMiddle += res.countMiddle;
        multiResult.totalAgeSenior += res.totalAgeSenior;
        multiResult.countSenior += res.countSenior;
    }

    auto endMulti = chrono::high_resolution_clock::now();
    chrono::duration<double> durationMulti = endMulti - startMulti;

    cout << "\nПроверка корректности данных... ";
    if (singleResult == multiResult) {
        cout << "Результаты совпадают!" << endl;
    } else {
        cout << "Результаты однопоточного и многопоточного вычисления различаются!" << endl;
        cout << "Single Junior: " << singleResult.countJunior << ", Multi Junior: " << multiResult.countJunior << endl;
    }

    // Вывод результатов

    cout << "\n------------------------------------------------" << endl;
    cout << "РЕЗУЛЬТАТЫ ОБРАБОТКИ (Проверка корректности по Multi-Thread):" << endl;
    printAvg("Средний возраст Junior", multiResult.totalAgeJunior, multiResult.countJunior);
    printAvg("Средний возраст Middle", multiResult.totalAgeMiddle, multiResult.countMiddle);
    printAvg("Средний возраст Senior", multiResult.totalAgeSenior, multiResult.countSenior);
    cout << "------------------------------------------------" << endl;

    cout << "\nСРАВНЕНИЕ ПРОИЗВОДИТЕЛЬНОСТИ:" << endl;
    cout << "Время (1 поток):        " << fixed << setprecision(6) << durationSingle.count() << " сек." << endl;
    cout << "Время (" << threadCount << " потоков):      " << fixed << setprecision(6) << durationMulti.count() << " сек." << endl;

    cout << "Ускорение:              " << setprecision(2) << durationSingle.count() / durationMulti.count() << " раз(а)" << endl;

    return 0;
}