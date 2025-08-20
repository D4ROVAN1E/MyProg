#include "plane_system.h"

using namespace std;

// Функция обработки команд пользователя
void process_command(PlaneSystem& system, const string& input) {
    // Разбиваем входную строку на отдельные слова
    vector<string> tokens = split_string(input);

    if (tokens.empty()) {
        cout << "Пустая команда." << endl;
        return;
    }

    Command cmd = parse_command(tokens[0]);

    // Обработка различных типов команд
    switch (cmd) {
    case Command::CREATE_PLANE: {
        if (tokens.size() < 3) {
            cout << "Неверный формат команды CREATE_PLANE. Использование: CREATE_PLANE plane_name town1 town2 ..." << endl;
            return;
        }

        // Извлечение имени самолета и списка городов
        string plane_name = tokens[1];
        vector<string> towns;

        for (auto& town : vector<string>(tokens.begin() + 2, tokens.end())) {
            towns.push_back(town);
        }

        // Попытка добавить самолет в систему
        if (system.add_plane(plane_name, towns)) {
            cout << "Самолет " << plane_name << " создан с маршрутом через ";
            for (const auto& town : towns) {
                cout << town << " ";
            }
            cout << endl;
        }
        break;
    }

    case Command::PLANES_FOR_TOWN: {
        if (tokens.size() != 2) {
            cout << "Неверный формат команды PLANES_FOR_TOWN. Использование: PLANES_FOR_TOWN town_name" << endl;
            return;
        }

        // Отображение самолетов для указанного города
        system.show_planes_for_town(tokens[1]);
        break;
    }

    case Command::TOWNS_FOR_PLANE: {
        if (tokens.size() != 2) {
            cout << "Неверный формат команды TOWNS_FOR_PLANE. Использование: TOWNS_FOR_PLANE plane_name" << endl;
            return;
        }

        // Отображение городов для указанного самолета
        system.show_towns_for_plane(tokens[1]);
        break;
    }

    case Command::PLANES: {
        // Отображение информации о всех самолетах
        system.show_all_planes();
        break;
    }

    case Command::INVALID: {
        cout << "Неизвестная команда: " << tokens[0] << endl;
        cout << "Доступные команды:" << endl;
        cout << "  CREATE_PLANE plane_name town1 town2 ..." << endl;
        cout << "  PLANES_FOR_TOWN town_name" << endl;
        cout << "  TOWNS_FOR_PLANE plane_name" << endl;
        cout << "  PLANES" << endl;
        break;
    }
    }
}