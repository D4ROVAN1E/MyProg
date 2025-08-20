#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;

// Перечисление команд
enum class Command {
    ADD,
    REMOVE,
    INFO,
    EXIT,
    UNKNOWN
};

// Функция для преобразования строки в команду
Command parseCommand(const string& cmd) {
    if (cmd == "ADD") return Command::ADD;
    if (cmd == "REMOVE") return Command::REMOVE;
    if (cmd == "INFO") return Command::INFO;
    if (cmd == "EXIT") return Command::EXIT;
    return Command::UNKNOWN;
}

// Функция для проверки корректности адреса
bool isValidAddress(const string& address) {
    if (address.size() != 4) return false;
    if (address[0] != 'A' && address[0] != 'B') return false;
    int shelf = address[1] - '0';
    int section = address[2] - '0';
    int level = address[3] - '0';
    return  shelf >= 1 && shelf <= 6 && 
            section >= 1 && section <= 4 && 
            level >= 1 && level <= 4;
}

// Функция для получения процента заполнения
double getFillPercentage(int total, int max) {
    return (static_cast<double>(total) / max) * 100;
}

// Функция для вывода информации о складе
void printWarehouseInfo(const map<string, map<string, int>>& warehouse, 
                       const map<string, int>& cellTotals, 
                       int totalItems) {
    // Вывод общей информации
    cout << "Общая загрузка склада: " << fixed << setprecision(2) 
         << getFillPercentage(totalItems, 1920) << "%" << endl;

    // Вывод информации по зонам
    for (char zone : {'A', 'B'}) {
        int zoneItems = 0;
        for (const pair<const string, int>& pair : cellTotals) {
            if (pair.first[0] == zone) {
                zoneItems += pair.second;
            }
        }
        cout << "Загрузка зоны " << zone << ": " 
             << getFillPercentage(zoneItems, 960) << "%" << endl;
    }

    // Вывод информации о заполненных ячейках
    cout << "\nЗаполненные ячейки:" << endl;
    for (const pair<const string, map<string, int>>& pair : warehouse) {
        map<string, int>::const_iterator it = cellTotals.find(pair.first);
        if (it != cellTotals.end() && (*it).second > 0) {
            cout << "Ячейка " << pair.first << ": ";
            for (const auto& productPair : pair.second) {
                cout << productPair.first << " (" << productPair.second << "), ";
            }
            cout << endl;
        }
    }

    // Вывод пустых ячеек
    cout << "\nПустые ячейки:" << endl;
    for (char zone : {'A', 'B'}) {
        for (char shelf : {'1', '2', '3', '4', '5', '6'}) {
            for (char section : {'1', '2', '3', '4'}) {
                for (char level : {'1', '2', '3', '4'}) {
                    stringstream ss;
                    ss << zone << shelf << section << level;
                    string address = ss.str();
                    map<string, int>::const_iterator it = cellTotals.find(address);
                    if (it == cellTotals.end() || (*it).second == 0) {
                        cout << address << " ";
                    }
                }
            }
        }
    }
    cout << endl;
}

int main() {
    // Хранение товаров в ячейках: адрес -> (товар -> количество)
    map<string, map<string, int>> warehouse;
    // Хранение общего количества товаров в ячейках: адрес -> количество
    map<string, int> cellTotals;
    
    // Общее количество товаров на складе
    int totalItems = 0;

    string command;
    while (true) {
        cout << "Введите команду (ADD/REMOVE/INFO/EXIT): ";
        getline(cin, command);

        // Разбиваем строку на части
        vector<string> parts;
        stringstream ss(command);
        string part;
        
        // Собираем все части команды
        while (ss >> part) {
            parts.push_back(part);
        }

        if (parts.empty()) {
            cout << "Ошибка: пустая команда" << endl;
            continue;
        }

        Command cmd = parseCommand(parts[0]);

        switch (cmd) {
            case Command::EXIT:
                return 0;

            case Command::INFO:
                printWarehouseInfo(warehouse, cellTotals, totalItems);
                break;

            case Command::ADD:
            case Command::REMOVE: {
                if (parts.size() < 4) {
                    cout << "Ошибка: недостаточно параметров" << endl;
                    break;
                }

                // Собираем название товара (может содержать пробелы)
                string product;
                bool first = true;
                for (const auto& part : vector<string>(parts.begin() + 1, parts.end() - 2)) {
                    if (!first) product += " ";
                    product += part;
                    first = false;
                }

                // Получаем количество и адрес
                int quantity;
                try {
                    quantity = stoi(parts[parts.size() - 2]);
                } catch (...) {
                    cout << "Ошибка: некорректное количество" << endl;
                    break;
                }
                string address = parts[parts.size() - 1];

                if (!isValidAddress(address)) {
                    cout << "Ошибка: некорректный адрес" << endl;
                    break;
                }

                if (cmd == Command::ADD) {
                    // Проверка на превышение лимита ячейки (максимум 10 единиц)
                    if (cellTotals[address] + quantity > 10) {
                        cout << "Ошибка: превышен лимит ячейки (максимум 10 единиц)" << endl;
                        break;
                    }
                    warehouse[address][product] += quantity;
                    cellTotals[address] += quantity;
                    totalItems += quantity;
                    cout << "Товар успешно добавлен" << endl;
                } else { // REMOVE
                    // Проверка наличия достаточного количества товара
                    if (warehouse[address][product] < quantity) {
                        cout << "Ошибка: недостаточно товара в ячейке" << endl;
                        break;
                    }
                    warehouse[address][product] -= quantity;
                    cellTotals[address] -= quantity;
                    totalItems -= quantity;
                    // Удаление записи о товаре, если его количество стало нулевым
                    if (warehouse[address][product] == 0) {
                        warehouse[address].erase(product);
                    }
                    cout << "Товар успешно удален" << endl;
                }
                break;
            }

            case Command::UNKNOWN:
                cout << "Неизвестная команда" << endl;
                break;
        }
    }

    return 0;
}
