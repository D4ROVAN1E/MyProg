#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <random>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <ctime>

// Структура для хранения информации о посетителе
struct Visitor {
    std::string ticket;      // Номер талона
    int duration;            // Продолжительность посещения в минутах
    
    Visitor(const std::string& t, int d) : ticket(t), duration(d) {}
};

// Структура для хранения информации об окне обслуживания
struct Window {
    int id;                             // Номер окна (начиная с 1)
    int totalTime;                      // Общее время обслуживания
    std::vector<Visitor> visitors;      // Список посетителей
    
    Window(int i) : id(i), totalTime(0) {}
    
    // Добавление посетителя в окно
    void addVisitor(const Visitor& visitor) {
        visitors.push_back(visitor);
        totalTime += visitor.duration;
    }
};

// Генерация случайного номера талона (формат: TXXX, где X - случайная цифра)
std::string generateTicket() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 999);
    
    int number = dis(gen);
    char buffer[5];
    snprintf(buffer, sizeof(buffer), "%03d", number);
    return "T" + std::string(buffer);
}

int main() {
    // Установка локали для поддержки кириллицы
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());
    
    int windowCount;
    std::cout << "Введите кол-во окон" << std::endl;
    std::cin >> windowCount;
    
    // Очистка буфера ввода
    std::cin.ignore();
    
    // Список всех посетителей
    std::vector<Visitor> allVisitors;
    
    std::string command;
    while (true) {
        std::getline(std::cin, command);
        
        if (command.substr(0, 7) == "ENQUEUE") {
            int duration = std::stoi(command.substr(8));
            std::string ticket = generateTicket();
            allVisitors.emplace_back(ticket, duration);
            std::cout << ticket << std::endl;
        } 
        else if (command == "DISTRIBUTE") {
            break;
        }
    }
    
    // Создаем окна обслуживания
    std::vector<Window> windows;
    for (int i = 1; i <= windowCount; ++i) {
        windows.emplace_back(i);
    }
    
    // Сортируем посетителей по убыванию времени обслуживания
    std::sort(allVisitors.begin(), allVisitors.end(), 
              [](const Visitor& a, const Visitor& b) {
                  return a.duration > b.duration;
              });
    
    // Распределяем посетителей по окнам (жадный алгоритм)
    for (const auto& visitor : allVisitors) {
        // Находим окно с минимальным временем обслуживания
        auto minWindow = std::min_element(windows.begin(), windows.end(),
                                         [](const Window& a, const Window& b) {
                                             return a.totalTime < b.totalTime;
                                         });
        
        // Добавляем посетителя в это окно
        minWindow->addVisitor(visitor);
    }
    
    // Выводим результат распределения
    for (const auto& window : windows) {
        std::cout << "Окно " << window.id << " (" << window.totalTime << " минут): ";
        
        bool first = true;
        for (const auto& visitor : window.visitors) {
            if (!first) {
                std::cout << ", ";
            }
            std::cout << visitor.ticket;
            first = false;
        }
        std::cout << std::endl;
    }
    
    return 0;
}
