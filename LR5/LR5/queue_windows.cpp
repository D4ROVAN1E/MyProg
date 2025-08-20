#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <random>
#include <algorithm>
#include <windows.h> // Для поддержки кириллицы в Windows

using namespace std;

// Структура для хранения информации о посетителе
struct Visitor {
    string ticket;      // Номер талона
    int duration;       // Продолжительность посещения в минутах
    
    Visitor(const string& t, int d) : ticket(t), duration(d) {}
};

// Структура для хранения информации об окне обслуживания
struct Window {
    int id;                             // Номер окна (начиная с 1)
    int totalTime;                      // Общее время обслуживания
    vector<Visitor> visitors;      // Список посетителей
    
    Window(int i) : id(i), totalTime(0) {}
    
    // Добавление посетителя в окно
    void addVisitor(const Visitor& visitor) {
        visitors.push_back(visitor);
        totalTime += visitor.duration;
    }
};

// Генерация случайного номера талона (формат: TXXX, где X - случайная цифра)
string generateTicket() {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dis(0, 999);
    
    int number = dis(gen);
    char buffer[5];
    snprintf(buffer, sizeof(buffer), "%03d", number);
    return "T" + string(buffer);
}

int main() {
    // Настройка консоли Windows для поддержки кириллицы
    SetConsoleCP(1251);       // Установка кодовой страницы ввода
    SetConsoleOutputCP(1251); // Установка кодовой страницы вывода
    
    int windowCount;
    cout << "Введите кол-во окон" << endl;
    cin >> windowCount;
    
    // Очистка буфера ввода
    cin.ignore();
    
    // Список всех посетителей
    vector<Visitor> allVisitors;
    
    string command;
    while (true) {
        getline(cin, command);
        
        if (command.substr(0, 7) == "ENQUEUE") {
            int duration = stoi(command.substr(8));
            string ticket = generateTicket();
            allVisitors.emplace_back(ticket, duration);
            cout << ticket << endl;
        } 
        else if (command == "DISTRIBUTE") {
            break;
        }
    }
    
    // Создаем окна обслуживания
    vector<Window> windows;
    for (int i = 1; i <= windowCount; ++i) {
        windows.emplace_back(i);
    }
    
    // Сортируем посетителей по убыванию времени обслуживания
    sort(allVisitors.begin(), allVisitors.end(), 
              [](const Visitor& a, const Visitor& b) {
                  return a.duration > b.duration;
              });
    
    // Распределяем посетителей по окнам (жадный алгоритм)
    for (const auto& visitor : allVisitors) {
        // Находим окно с минимальным временем обслуживания
        auto minWindow = min_element(windows.begin(), windows.end(),
                                         [](const Window& a, const Window& b) {
                                             return a.totalTime < b.totalTime;
                                         });
        
        // Добавляем посетителя в это окно
        minWindow->addVisitor(visitor);
    }
    
    // Выводим результат распределения
    for (const auto& window : windows) {
        cout << "Окно " << window.id << " (" << window.totalTime << " минут): ";
        
        bool first = true;
        for (const auto& visitor : window.visitors) {
            if (!first) {
                cout << ", ";
            }
            cout << visitor.ticket;
            first = false;
        }
        cout << endl;
    }
    
    return 0;
}
