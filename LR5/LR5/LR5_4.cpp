#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

// Перечисление возможных команд
enum class Command {
    CLASS,
    NEXT,
    VIEW,
    UNKNOWN,
    EXIT
};

// Структура для хранения расписания
struct Schedule {
    map<int, int> numDays = {
    {1, 31},  // Январь
    {2, 28},  // Февраль (в невисокосный год)
    {3, 31},  // Март
    {4, 30},  // Апрель
    {5, 31},  // Май
    {6, 30},  // Июнь
    {7, 31},  // Июль
    {8, 31},  // Август
    {9, 30},  // Сентябрь
    {10, 31}, // Октябрь
    {11, 30}, // Ноябрь
    {12, 31}  // Декабрь
    };
    map<int, map<int, vector<string>>> months; // месяц -> день -> предметы
    int current_month = 1;
};

// Функция для определения типа команды из строки
Command parseCommand(const string& cmd) {
    if (cmd == "CLASS") return Command::CLASS;
    if (cmd == "NEXT") return Command::NEXT;
    if (cmd == "VIEW") return Command::VIEW;
    if (cmd == "EXIT") return Command::EXIT;
    return Command::UNKNOWN;
}

// Проверка корректности введенного дня
bool isValidDay(const string& day_str) {
    if (day_str.empty()) return false;

    // Проверяем, что строка содержит только цифры
    for (const auto& c : day_str) {
        if (!isdigit(c)) return false;
    }

    int day = stoi(day_str);
    return day > 0;
}

// Добавление занятия в расписание
void addClass(Schedule& schedule, int day, const string& subject) {
    auto& day_classes = schedule.months[schedule.current_month][day];

    // Проверяем, есть ли уже такой предмет в этот день
    auto it = find(day_classes.begin(), day_classes.end(), subject);

    // Добавляем только если предмета еще нет
    if (it == day_classes.end()) {
        day_classes.push_back(subject);
    }
}

// Переход к следующему месяцу
void nextMonth(Schedule& schedule) {
    int current_month = schedule.current_month;

    //Находим в календаре текущий месяц
    auto month_it = schedule.months.find(current_month);

    //Если на текущий месяц существуют занятия
    if (month_it != schedule.months.end()) {

        int next_month;
        (current_month + 1 > 12) ? next_month = 1 : next_month = current_month + 1;
        int next_days = schedule.numDays[next_month];
        auto& classes = month_it->second;
        
        for (auto& days : classes) {

            //Переносим предметы на предпоследний день
            for (auto& subjects : days.second) {

                //Если текущий день отсутсвует в следующем месяце, то переносим на следующий месяц
                if (days.first > next_days) {
                    schedule.months[current_month + 1][next_days - 1].push_back(subjects);
                }
                else {
                    schedule.months[current_month + 1][days.first].push_back(subjects);
                }
            }
        }
        
    }
    schedule.current_month++;
}

// Просмотр расписания на конкретный день
void viewDay(const Schedule& schedule, int day) {
    auto month_it = schedule.months.find(schedule.current_month);

    // Если месяц не найден, значит в этот день нет занятий
    if (month_it == schedule.months.end()) {
        cout << "In " << day << " day We are free!" << endl;
        return;
    }

    auto day_it = month_it->second.find(day);

    // Если день не найден, значит в этот день нет занятий
    if (day_it == month_it->second.end()) {
        cout << "In " << day << " day We are free!" << endl;
        return;
    }

    // Выводим информацию о занятиях
    const auto& classes = day_it->second;
    cout << "In " << day << " day " << classes.size() << " classes in university: ";

    // Выводим список предметов через запятую
    bool first = true;
    for (const auto& cls : classes) {
        if (!first) {
            cout << ", ";
        }
        cout << cls;
        first = false;
    }
    cout << "." << endl;
}

int main() {
    Schedule schedule;
    string line;
    int numOfCommands;
    int couterOfCommands = 0;
    cin >> numOfCommands;
    if (numOfCommands > 0) {
        while (numOfCommands >= couterOfCommands++) {
            getline(cin, line);
            istringstream iss(line);
            string command_str;
            iss >> command_str;

            Command cmd = parseCommand(command_str);

            switch (cmd) {
            case Command::EXIT: {
                return 0;
                break;
            }
            case Command::CLASS: {
                string day_str;
                string subject;
                iss >> day_str >> subject;

                if (!isValidDay(day_str)) {
                    cout << "Incorrect input\n";
                    break; // Некорректный ввод, пропускаем команду
                }

                int day = stoi(day_str);
                addClass(schedule, day, subject);
                break;
            }

            case Command::NEXT: {
                nextMonth(schedule);
                break;
            }

            case Command::VIEW: {
                string day_str;
                iss >> day_str;

                if (!isValidDay(day_str)) {
                    cout << "Incorrect input\n";
                    break; // Некорректный ввод, пропускаем команду
                }

                int day = stoi(day_str);
                viewDay(schedule, day);
                break;
            }

            case Command::UNKNOWN:
            default:
                break; // Неизвестная команда, пропускаем
            }
        }
    }

    return 0;
}