#include "plane_system.h"

using namespace std;

// Функция для определения типа команды из строки
Command parse_command(const string& cmd) {
    if (cmd == "CREATE_PLANE") return Command::CREATE_PLANE;
    if (cmd == "PLANES_FOR_TOWN") return Command::PLANES_FOR_TOWN;
    if (cmd == "TOWNS_FOR_PLANE") return Command::TOWNS_FOR_PLANE;
    if (cmd == "PLANES") return Command::PLANES;
    return Command::INVALID;
}

// Функция для разбиения строки на отдельные слова
vector<string> split_string(const string& str) {
    vector<string> tokens;
    istringstream iss(str);
    string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

// Метод для добавления нового самолета в систему
bool PlaneSystem::add_plane(const string& plane_name, const vector<string>& towns) {
    // Проверяем уникальность имени самолета
    if (plane_names.find(plane_name) != plane_names.end()) {
        cout << "Ошибка: Самолет с именем '" << plane_name << "' уже существует!" << endl;
        return false;
    }
    // Проверяем на дублирование городов в маршруте
    set<string> unique_towns;
    for (const auto& town : towns) {
        if (unique_towns.find(town) != unique_towns.end()) {
            cout << "Ошибка: Город '" << town << "' повторяется в маршруте!" << endl;
            return false;
        }
        unique_towns.insert(town);
    }

    // Добавляем самолет в систему
    planes.emplace_back(plane_name, towns);
    plane_names.insert(plane_name);
    // Обновляем индекс городов к самолетам
    for (const auto& town : towns) {
        town_to_planes[town].insert(plane_name);
    }
    return true;
}

// Метод для отображения всех самолетов, пролетающих через указанный город
void PlaneSystem::show_planes_for_town(const string& town) const {
    auto it = town_to_planes.find(town);
    if (it != town_to_planes.end()) {
        cout << "Самолеты, пролетающие через " << town << ": ";
        for (const auto& plane : it->second) {
            cout << plane << " ";
        }
        cout << endl;
    }
    else {
        cout << "Через город " << town << " не пролетает ни один самолет." << endl;
    }
}

// Метод для отображения всех городов в маршруте указанного самолета
void PlaneSystem::show_towns_for_plane(const string& plane_name) const {
    // Поиск самолета по имени
    auto plane_it = find_if(planes.begin(), planes.end(),
        [&plane_name](const Plane& p) { 
            return p.name == plane_name; });

    if (plane_it != planes.end()) {
        cout << "Города для самолета " << plane_name << ":" << endl;

        // Вывод информации о каждом городе в маршруте
        for (const auto& town : plane_it->towns) {
            cout << "  " << town << " (другие самолеты: ";

            // Поиск других самолетов, пролетающих через этот город
            auto town_planes_it = town_to_planes.find(town);
            if (town_planes_it != town_to_planes.end()) {
                bool first = true;
                for (const auto& other_plane : town_planes_it->second) {
                    if (other_plane != plane_name) {
                        if (!first) cout << ", ";
                        cout << other_plane;
                        first = false;
                    }
                }
                if (first) {
                    cout << "нет";
                }
            }
            cout << ")" << endl;
        }
    }
    else {
        cout << "Самолет " << plane_name << " не найден." << endl;
    }
}

// Метод для отображения информации о всех самолетах в системе
void PlaneSystem::show_all_planes() const {
    cout << "Все самолеты:" << endl;
    for (const auto& plane : planes) {
        cout << plane.name << ": ";
        for (const auto& town : plane.towns) {
            cout << town << " ";
        }
        cout << endl;
    }
}