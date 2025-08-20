#ifndef PLANE_SYSTEM_H
#define PLANE_SYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

// Перечисление доступных команд системы
enum class Command {
    CREATE_PLANE,
    PLANES_FOR_TOWN,
    TOWNS_FOR_PLANE,
    PLANES,
    INVALID
};

// Структура, представляющая самолет и его маршрут
struct Plane {
    string name;           
    vector<string> towns;  // Список городов в маршруте

    // Конструктор для создания самолета
    Plane(const string& n, const vector<string>& t) {
        name = n;
        towns = t;
    }
};

// Основной класс системы управления самолетами
struct PlaneSystem {
    vector<Plane> planes;                          // Список всех самолетов
    map<string, set<string>> town_to_planes;      // Индекс городов к самолетам
    set<string> plane_names;                      // Множество имен самолетов для проверки уникальности

    // Методы системы
    bool add_plane(const string& plane_name, const vector<string>& towns);  // Добавление нового самолета
    void show_planes_for_town(const string& town) const;                    // Показать самолеты для города
    void show_towns_for_plane(const string& plane_name) const;              // Показать города для самолета
    void show_all_planes() const;                                          // Показать все самолеты
};

// Вспомогательные функции
Command parse_command(const string& cmd);                    // Разбор команды из строки
vector<string> split_string(const string& str);             // Разбиение строки на токены
void process_command(PlaneSystem& system, const string& input);  // Обработка команды

#endif