#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <numeric>

using namespace std;

// Структура для хранения информации о посетителе
struct Visitor {
    string ticket;    // Номер билета
    int duration;     // Время обслуживания

    Visitor(const string& t, int d) {
        ticket = t;
        duration = d;
    }
};

// Структура для хранения информации об окне обслуживания
struct Window {
    int id;                  // Номер окна
    int totalTime;           // Общее время обслуживания
    vector<string> tickets;  // Список билетов, обслуживаемых в этом окне

    Window(int windowId) {
        id = windowId;
        totalTime = 0;
    }
};

// Функция для генерации случайного номера билета
string generateTicket() {
    mt19937 rng(random_device{}());
    uniform_int_distribution<int> dist(0, 999);
    int number = dist(rng);
    stringstream ss;
    ss << "T" << setfill('0') << setw(3) << number;
    return ss.str();
}

// Функция для создания окон обслуживания
void MakeWindows(int numWindows, vector<Window>& windows) {
    vector<int> windowIds(numWindows);
    iota(windowIds.begin(), windowIds.end(), 1);  // Заполняем вектор последовательными числами
    for (const auto& id : windowIds) {
        windows.emplace_back(id);
    }
}

// Функция для добавления нового посетителя в очередь
string Enqueue(int duration, vector<Visitor>& visitors) {
    string ticket = generateTicket();
    visitors.emplace_back(ticket, duration);
    return ticket;
}

// Функция для распределения посетителей по окнам
void Distribute(vector<Visitor>& visitors, vector<Window>& windows) {
    // Создаем копию посетителей для сортировки
    vector<Visitor> sortedVisitors = visitors;

    // Сортируем по убыванию времени (самые долгие первыми)
    sort(sortedVisitors.begin(), sortedVisitors.end(),
        [](const Visitor& a, const Visitor& b) {
            return a.duration > b.duration;
        });

    // Распределяем посетителей по окнам
    for (const auto& visitor : sortedVisitors) {
        // Находим окно с минимальным временем
        auto minWindow = min_element(windows.begin(), windows.end(),
            [](const Window& a, const Window& b) {
                return a.totalTime < b.totalTime;
            });

        minWindow->tickets.push_back(visitor.ticket);
        minWindow->totalTime += visitor.duration;
    }

    // Выводим результат
    for (const auto& window : windows) {
        cout << ">>> Окно " << window.id << " (" << window.totalTime << " минут): ";

        bool first = true;
        for (const auto& ticket : window.tickets) {
            if (!first) {
                cout << ", ";
            }
            cout << ticket;
            first = false;
        }
        cout << endl;
    }
}

void isValidNum(const string& day_str) {
    if (day_str.empty())
        throw "Введенная страка пуста\n";

    // Проверяем, что строка содержит только цифры
    for (const auto& c : day_str) {
        if (!isdigit(c))
            throw "Введенное значение не является числом\n";
    }

    int day = stoi(day_str);
    if (day < 0)
        throw "Количество окон должно быть положительным\n";
}

int main() {
    vector<Visitor> visitors;  // Вектор для хранения посетителей
    vector<Window> windows;    // Вектор для хранения окон
    try {
        cout << ">>> Введите кол-во окон" << endl;

        string numWindows;
        cin >> numWindows;
        isValidNum(numWindows);
        MakeWindows(stoi(numWindows), windows);

        // Основной цикл обработки команд
        string command;
        while (cin >> command) {
            if (command == "ENQUEUE") {
                int duration;
                cin >> duration;
                string ticket = Enqueue(duration, visitors);
                cout << ">>> " << ticket << endl;
            }
            else if (command == "DISTRIBUTE") {
                Distribute(visitors, windows);
                break;
            }
        }
    } catch (const char* s) {
        cerr << s;
    }

    return 0;
}