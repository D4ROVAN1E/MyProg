#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <fcntl.h>
#include <io.h>
#include <codecvt>
#include <locale>
using namespace std;

// Перечисление команд
enum class Command {
    ADD,
    REMOVE,
    INFO,
    EXIT,
    UNKNOWN
};

// Функция для конвертации wstring в string
string wstring_to_string(const wstring& wstr) {
    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
}

// Функция для преобразования строки в команду
Command parseCommand(const wstring& cmd) {
    if (cmd == L"ADD") return Command::ADD;
    if (cmd == L"REMOVE") return Command::REMOVE;
    if (cmd == L"INFO") return Command::INFO;
    if (cmd == L"EXIT") return Command::EXIT;
    return Command::UNKNOWN;
}

// Функция для проверки корректности адреса
bool isValidAddress(const wstring& address) {
    if (address.size() != 4) return false;
    if (address[0] != L'А' && address[0] != L'Б') return false;
    int shelf = address[1] - L'0';
    int section = address[2] - L'0';
    int level = address[3] - L'0';
    return  shelf >= 1 && shelf <= 6 && 
            section >= 1 && section <= 4 && 
            level >= 1 && level <= 4;
}

// Функция для получения процента заполнения
double getFillPercentage(int total, int max) {
    return (static_cast<double>(total) / max) * 100;
}

// Функция для вывода информации о складе
void printWarehouseInfo(const map<wstring, map<wstring, int>>& warehouse, 
                       const map<wstring, int>& cellTotals, 
                       int totalItems) {
    // Вывод общей информации
    wcout << L"Общая загрузка склада: " << fixed << setprecision(2) 
         << getFillPercentage(totalItems, 1920) << L"%" << endl;

    // Вывод информации по зонам
    for (wchar_t zone : {L'А', L'Б'}) {
        int zoneItems = 0;
        for (const pair<const wstring, int>& pair : cellTotals) {
            if (pair.first[0] == zone) {
                zoneItems += pair.second;
            }
        }
        wcout << L"Загрузка зоны " << zone << L": " 
             << getFillPercentage(zoneItems, 960) << L"%" << endl;
    }

    // Вывод информации о заполненных ячейках
    wcout << L"\nЗаполненные ячейки:" << endl;
    for (const pair<const wstring, map<wstring, int>>& pair : warehouse) {
        map<wstring, int>::const_iterator it = cellTotals.find(pair.first);
        if (it != cellTotals.end() && (*it).second > 0) {
            wcout << L"Ячейка " << pair.first << L": ";
            for (const auto& productPair : pair.second) {
                wcout << productPair.first << L" (" << productPair.second << L"), ";
            }
            wcout << endl;
        }
    }

    // Вывод пустых ячеек
    wcout << L"\nПустые ячейки:" << endl;
    for (wchar_t zone : {L'А', L'Б'}) {
        for (wchar_t shelf : {L'1', L'2', L'3', L'4', L'5', L'6'}) {
            for (wchar_t section : {L'1', L'2', L'3', L'4'}) {
                for (wchar_t level : {L'1', L'2', L'3', L'4'}) {
                    wstring address = wstring(1, zone) + wstring(1, shelf) + wstring(1, section) + wstring(1, level);
                    map<wstring, int>::const_iterator it = cellTotals.find(address);
                    if (it == cellTotals.end() || (*it).second == 0) {
                        wcout << address << L" ";
                    }
                }
            }
        }
    }
    wcout << endl;
}

int main() {
    // Настройка консоли Windows для поддержки Unicode
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
    
    // Хранение товаров в ячейках: адрес -> (товар -> количество)
    map<wstring, map<wstring, int>> warehouse;
    // Хранение общего количества товаров в ячейках: адрес -> количество
    map<wstring, int> cellTotals;
    int totalItems = 0;

    wstring command;
    while (true) {
        wcout << L"Введите команду (ADD/REMOVE/INFO/EXIT): ";
        getline(wcin, command);

        // Разбиваем строку на части
        vector<wstring> parts;
        wistringstream ss(command);
        wstring part;
        
        // Собираем все части команды
        while (ss >> part) {
            parts.push_back(part);
        }

        if (parts.empty()) {
            wcout << L"Ошибка: пустая команда" << endl;
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
                    wcout << L"Ошибка: недостаточно параметров" << endl;
                    break;
                }

                // Собираем название товара (может содержать пробелы)
                wstring product;
                bool first = true;
                for (const auto& part : vector<wstring>(parts.begin() + 1, parts.end() - 2)) {
                    if (!first) product += L" ";
                    product += part;
                    first = false;
                }

                // Получаем количество и адрес
                int quantity;
                try {
                    quantity = stoi(wstring_to_string(parts[parts.size() - 2]));
                } catch (...) {
                    wcout << L"Ошибка: некорректное количество" << endl;
                    break;
                }
                wstring address = parts[parts.size() - 1];

                if (!isValidAddress(address)) {
                    wcout << L"Ошибка: некорректный адрес" << endl;
                    break;
                }

                if (cmd == Command::ADD) {
                    if (cellTotals[address] + quantity > 10) {
                        wcout << L"Ошибка: превышен лимит ячейки (максимум 10 единиц)" << endl;
                        break;
                    }
                    warehouse[address][product] += quantity;
                    cellTotals[address] += quantity;
                    totalItems += quantity;
                    wcout << L"Товар успешно добавлен" << endl;
                } else { // REMOVE
                    if (warehouse[address][product] < quantity) {
                        wcout << L"Ошибка: недостаточно товара в ячейке" << endl;
                        break;
                    }
                    warehouse[address][product] -= quantity;
                    cellTotals[address] -= quantity;
                    totalItems -= quantity;
                    if (warehouse[address][product] == 0) {
                        warehouse[address].erase(product);
                    }
                    wcout << L"Товар успешно удален" << endl;
                }
                break;
            }

            case Command::UNKNOWN:
                wcout << L"Неизвестная команда" << endl;
                break;
        }
    }

    return 0;
}
