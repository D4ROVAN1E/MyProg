#include "plane_system.h"
using namespace std;

int main() {
    // Создание экземпляра системы управления самолетами
    PlaneSystem system;
    string input;

    cout << "=== Система управления графиком движения самолетов ===" << endl;
    cout << "Доступные команды:" << endl;
    cout << "  CREATE_PLANE plane_name town1 town2 ... - создать самолет с маршрутом" << endl;
    cout << "  PLANES_FOR_TOWN town_name - показать самолеты для города" << endl;
    cout << "  TOWNS_FOR_PLANE plane_name - показать города для самолета" << endl;
    cout << "  PLANES - показать все самолеты" << endl;
    cout << "  exit - выход из программы" << endl;
    cout << "========================================================" << endl;

    // Основной цикл обработки команд
    while (true) {
        cout << "\nВведите команду: ";
        getline(cin, input);

        if (input == "exit" || input == "EXIT") {
            cout << "Завершение работы программы." << endl;
            break;
        }

        // Пропуск пустых команд
        if (input.empty()) {
            continue;
        }

        // Обработка введенной команды
        process_command(system, input);
    }

    return 0;
}