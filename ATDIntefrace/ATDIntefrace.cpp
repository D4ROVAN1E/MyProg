#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>

// Подключаем заголовочные файлы
#include "array.h"
#include "stack.h"
#include "queue.h"
#include "singly_list.h"
#include "doubly_list.h"
#include "binary_tree.h"

using namespace std;

// Enum для всех возможных команд 
enum class CommandType {
    HELP, EXIT, UNKNOWN, PRINT,
    // Array
    MPUSH_BACK, MPUSH_BY_IND, MGET_BY_IND, MDEL_BY_IND, MSWAP_BY_IND, 
    // Stack
    SPUSH, SPOP, 
    // Queue
    QPUSH, QPOP, QGET, 
    // Singly List
    FCREATE, FPUSH_HEAD, FPUSH_BACK, FPUSH_BEFORE, FPUSH_FORWARD, FDEL_HEAD, FDEL_BACK, FDEL_BY_VALUE, FGET_BY_VALUE, 
    // Doubly List
    LCREATE, LPUSH_HEAD, LPUSH_BACK, LPUSH_BEFORE, LPUSH_AFTER, LDEL_HEAD, LDEL_BACK, LDEL_BY_VALUE, LGET_BY_VALUE, 
    // Binary Tree
    TINSERT, TFULL
};

// Enum для типов структур данных
enum class DS_Type { UNKNOWN, ARRAY, STACK, QUEUE, SINGLY_LIST, DOUBLY_LIST, BINARY_TREE};

//Глобальные переменные и прототипы служебных функций

map<string, CommandType> commandMap;
map<string, DS_Type> structureTypeMap;
const string META_FILENAME = "_structure_types.meta";

void initializeCommandMap();
void loadStructureTypes();
void saveStructureType(const string& name, DS_Type type);
DS_Type getImpliedType(CommandType cmd);
void printHelp();

//Основная программа

int main() {
    setlocale(LC_ALL, "ru");
    initializeCommandMap();
    loadStructureTypes();

    string line;
    while (cout << "> " && getline(cin, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string commandStr, name;
        ss >> commandStr;

        CommandType command = commandMap.count(commandStr) ? commandMap[commandStr] : CommandType::UNKNOWN;

        // Обработка команд, не требующих имени структуры
        if (command == CommandType::HELP) { printHelp(); continue; }
        if (command == CommandType::EXIT) { cout << "Завершение работы программы..." << endl; break; }
        if (command == CommandType::UNKNOWN) { cout << "Неизвестная команда: '" << commandStr << "'.\n"; continue; }

        if (!(ss >> name)) { cout << "Ошибка: не указано имя структуры.\n"; continue; }

        // Определяем тип структуры (существующий или новый)
        DS_Type type = DS_Type::UNKNOWN;
        DS_Type impliedType = getImpliedType(command);

        if (command == CommandType::PRINT) {
            if (structureTypeMap.count(name) == 0) {
                cout << "Ошибка: структура '" << name << "' не существует.\n";
                continue;
            }
            else {
                type = structureTypeMap[name];
                goto start;
            }

		}

        if (structureTypeMap.count(name)) {
            type = structureTypeMap[name];
            if (type != impliedType) {
                cout << "Ошибка: команда '" << commandStr << "' неприменима к структуре '" << name << "'.\n";
                continue;
            }
        }
        else {
            type = impliedType;
            if (type != DS_Type::UNKNOWN) {
                saveStructureType(name, type);
            }
            else {
                cout << "Внутренняя ошибка: команда не сопоставлена с типом структуры.\n";
                continue;
            }
        }

        start:

        bool modified = false; // Флаг, показывающий, нужно ли сохранять данные

        // Внешний SWITCH по ТИПУ СТРУКТРЫ
        switch (type) {
        case DS_Type::ARRAY: {
            Array<string> arr;
            MLOAD(arr, name);

            // Внутренний SWITCH по КОМАНДЕ
            switch (command) {
            case CommandType::MPUSH_BACK: {
                string value;
                if (ss >> value) { MPUSH_BACK(arr, value); modified = true; }
                else { cout << "Ошибка: не указано значение.\n"; }
                break;
            }
            case CommandType::MPUSH_BY_IND: {
                string index_str, value;
                if (ss >> index_str >> value) { 
                    MPUSH_BY_IND(arr, stoi(index_str), value); 
                    modified = true;  
                }
                else { cout << "Ошибка: не указан индекс или значение.\n"; }
                break;
            }
            case CommandType::MGET_BY_IND: {
                string index_str;
                if (ss >> index_str) {  cout << "Элемент: " << MGET_BY_IND(arr, stoi(index_str)) << endl;  }
                else { cout << "Ошибка: не указан индекс.\n"; }
                break;
            }
            case CommandType::MDEL_BY_IND: {
                string index_str;
                if (ss >> index_str) {  
                    MDEL_BY_IND(arr, stoi(index_str)); 
                    modified = true; 
                }
                else { cout << "Ошибка: не указан индекс.\n"; }
                break;
            }
            case CommandType::MSWAP_BY_IND: {
                string index_str, value;
                if (ss >> index_str >> value) {  
                    MSWAP_BY_IND(arr, stoi(index_str), value); 
                    modified = true;  }
                else { cout << "Ошибка: не указан индекс или значение.\n"; }
                break;
            }
            case CommandType::PRINT: {
                PRINT(arr);
				break;
            }
            default: break;
            }

            if (modified) { MSAVE(arr, name); cout << "OK\n"; }
            break;
        } // case DS_Type::ARRAY

        case DS_Type::STACK: {
            Stack<string> stack;
            SLOAD(stack, name);

            switch (command) {
            case CommandType::SPUSH: {
                string value;
                if (ss >> value) { SPUSH(stack, value); modified = true; }
                else { cout << "Ошибка: не указано значение.\n"; }
                break;
            }
            case CommandType::SPOP: {
                if (stack.size > 0) { SPOP(stack); modified = true; }
                else { cout << "Стек пуст.\n"; }
                break;
            }
            case CommandType::PRINT: {
                PRINT(stack);
                break;
            }
            default: break;
            }

            if (modified) { SSAVE(stack, name); cout << "OK\n"; }
            break;
        } // case DS_Type::STACK

        case DS_Type::QUEUE: {
            Queue<string> queue;
            QLOAD(queue, name);

            switch (command) {
            case CommandType::QPUSH: {
                string value;
                if (ss >> value) { QPUSH(queue, value); modified = true; }
                else { cout << "Ошибка: не указано значение.\n"; }
                break;
            }
            case CommandType::QPOP: {
                try { cout << "Извлечено: " << QPOP(queue) << endl; modified = true; }
                catch (const exception& e) { cout << e.what() << endl; }
                break;
            }
            case CommandType::QGET: {
                try { cout << "Первый элемент: " << QGET(queue) << endl; }
                catch (const exception& e) { cout << e.what() << endl; }
                break;
            }
            case CommandType::PRINT: {
                PRINT(queue);
                break;
            }
            default: break;
            }

            if (modified) { QSAVE(queue, name); if (command != CommandType::QPOP) cout << "OK\n"; }
            break;
        } // case DS_Type::QUEUE

        case DS_Type::SINGLY_LIST: {
            ForwardList<string> list;
            FLOAD(list, name);

            switch (command) {
            case CommandType::FCREATE:
            case CommandType::FPUSH_HEAD:
            case CommandType::FPUSH_BACK: {
                string value;
                if (ss >> value) {
                    if (command == CommandType::FCREATE) FCREATE(list, value);
                    else if (command == CommandType::FPUSH_HEAD) FPUSH_HEAD(list, value);
                    else FPUSH_BACK(list, value);
                    modified = true;
                }
                else { cout << "Ошибка: не указано значение.\n"; }
                break;
            }
            case CommandType::FPUSH_BEFORE:
            case CommandType::FPUSH_FORWARD: {
                string target, value;
                if (ss >> target >> value) {
                    if (command == CommandType::FPUSH_BEFORE) FPUSH_BEFORE(list, target, value);
                    else FPUSH_FORWARD(FGET_BY_VALUE(list, target), value);
                    modified = true;
                }
                else { cout << "Ошибка: не указана цель или значение.\n"; }
                break;
            }
            case CommandType::FDEL_HEAD: { FDEL_HEAD(list); modified = true; break; }
            case CommandType::FDEL_BACK: { FDEL_BACK(list); modified = true; break; }
            case CommandType::FDEL_BY_VALUE: {
                string value;
                if (ss >> value) { FDEL_BY_VALUE(list, value); modified = true; }
                else { cout << "Ошибка: не указано значение.\n"; }
                break;
            }
            case CommandType::FGET_BY_VALUE: {
                string value;
                if (ss >> value) { cout << (FGET_BY_VALUE(list, value) ? "Элемент найден" : "Элемент не найден") << endl; }
                else { cout << "Ошибка: не указано значение.\n"; }
                break;
            }
            case CommandType::PRINT: {
                PRINT(list);
                break;
			}
            default: break;
            }

            if (modified) { FSAVE(list, name); cout << "OK\n"; }
            break;
        } // case DS_Type::SINGLY_LIST

        case DS_Type::DOUBLY_LIST: {
            DoublyList<string> list;
            LLOAD(list, name);

            switch (command) {
            case CommandType::LCREATE:
            case CommandType::LPUSH_HEAD:
            case CommandType::LPUSH_BACK: {
                string value;
                if (ss >> value) {
                    if (command == CommandType::LCREATE) LCREATE(list, value);
                    else if (command == CommandType::LPUSH_HEAD) LPUSH_HEAD(list, value);
                    else LPUSH_BACK(list, value);
                    modified = true;
                }
                else { cout << "Ошибка: не указано значение.\n"; }
                break;
            }
            case CommandType::LPUSH_BEFORE:
            case CommandType::LPUSH_AFTER: {
                string target, value;
                if (ss >> target >> value) {
                    if (command == CommandType::LPUSH_BEFORE) LPUSH_BEFORE(list, target, value);
                    else LPUSH_AFTER(list, target, value);
                    modified = true;
                }
                else { cout << "Ошибка: не указана цель или значение.\n"; }
                break;
            }
            case CommandType::LDEL_HEAD: { LDEL_HEAD(list); modified = true; break; }
            case CommandType::LDEL_BACK: { LDEL_BACK(list); modified = true; break; }
            case CommandType::LDEL_BY_VALUE: {
                string value;
                if (ss >> value) { LDEL_BY_VALUE(list, value); modified = true; }
                else { cout << "Ошибка: не указано значение.\n"; }
                break;
            }
            case CommandType::LGET_BY_VALUE: {
                string value;
                if (ss >> value) { cout << (LGET_BY_VALUE(list, value) ? "Элемент найден" : "Элемент не найден") << endl; }
                else { cout << "Ошибка: не указано значение.\n"; }
                break;
            }
            case CommandType::PRINT: {
                PRINT(list, 1);
                break;
            }
            default: break;
            }

            if (modified) { LSAVE(list, name); cout << "OK\n"; }
            break;
        } // case DS_Type::DOUBLY_LIST

        case DS_Type::BINARY_TREE: {
            FullBinaryTree<string> tree;
            TLOAD(tree, name);

            switch (command) {
            case CommandType::TINSERT: {
                string value;
                if (ss >> value) { TINSERT(tree, value); modified = true; }
                else { cout << "Ошибка: не указано значение.\n"; }
                break;
            }
            case CommandType::TFULL: {
                cout << "Дерево " << name << " является полным: " << (tree.root ? (TFULL(tree.root) ? "Да" : "Нет") : "Да (пустое)") << endl;
                break;
            }
            case CommandType::PRINT: {
                PRINT(tree, 5);
                break;
            }
            default: break;
            }

            if (modified) { TSAVE(tree, name); cout << "OK\n"; }
            break;
        } // case DS_Type::BINARY_TREE

        default:
            cout << "Структура '" << name << "' имеет неизвестный тип.\n";
            break;
        } // Внешний switch
    }
    return 0;
}

//Реализация служебных функций

DS_Type getImpliedType(CommandType cmd) { 
    switch (cmd) {
        // Array
    case CommandType::MPUSH_BACK: case CommandType::MPUSH_BY_IND: case CommandType::MGET_BY_IND:
    case CommandType::MDEL_BY_IND: case CommandType::MSWAP_BY_IND: 
        return DS_Type::ARRAY;
        // Stack
    case CommandType::SPUSH: case CommandType::SPOP: 
        return DS_Type::STACK;
        // Queue
    case CommandType::QPUSH: case CommandType::QPOP: case CommandType::QGET: 
        return DS_Type::QUEUE;
        // Singly List
    case CommandType::FCREATE: case CommandType::FPUSH_HEAD: case CommandType::FPUSH_BACK: case CommandType::FPUSH_BEFORE:
    case CommandType::FPUSH_FORWARD: case CommandType::FDEL_HEAD: case CommandType::FDEL_BACK: case CommandType::FDEL_BY_VALUE:
    case CommandType::FGET_BY_VALUE: 
        return DS_Type::SINGLY_LIST;
        // Doubly List
    case CommandType::LCREATE: case CommandType::LPUSH_HEAD: case CommandType::LPUSH_BACK: case CommandType::LPUSH_BEFORE:
    case CommandType::LPUSH_AFTER: case CommandType::LDEL_HEAD: case CommandType::LDEL_BACK: case CommandType::LDEL_BY_VALUE:
    case CommandType::LGET_BY_VALUE: 
        return DS_Type::DOUBLY_LIST;
        // Binary Tree
    case CommandType::TINSERT: case CommandType::TFULL: 
        return DS_Type::BINARY_TREE;
        // Unknown
    default:
        return DS_Type::UNKNOWN;
    }
}

void initializeCommandMap() {
    // Общие
    commandMap["HELP"] = CommandType::HELP;
    commandMap["EXIT"] = CommandType::EXIT;
	commandMap["PRINT"] = CommandType::PRINT;
    // Массив
    commandMap["MPUSH_BACK"] = CommandType::MPUSH_BACK; 
    commandMap["MPUSH_BY_IND"] = CommandType::MPUSH_BY_IND;
    commandMap["MGET_BY_IND"] = CommandType::MGET_BY_IND; 
    commandMap["MDEL_BY_IND"] = CommandType::MDEL_BY_IND;
    commandMap["MSWAP_BY_IND"] = CommandType::MSWAP_BY_IND; 
    // Стек
    commandMap["SPUSH"] = CommandType::SPUSH; 
    commandMap["SPOP"] = CommandType::SPOP; 
    // Очередь
    commandMap["QPUSH"] = CommandType::QPUSH; 
    commandMap["QPOP"] = CommandType::QPOP;
    commandMap["QGET"] = CommandType::QGET; 
    // Односвязный список
    commandMap["FCREATE"] = CommandType::FCREATE; 
    commandMap["FPUSH_HEAD"] = CommandType::FPUSH_HEAD;
    commandMap["FPUSH_BACK"] = CommandType::FPUSH_BACK; 
    commandMap["FPUSH_BEFORE"] = CommandType::FPUSH_BEFORE;
    commandMap["FPUSH_FORWARD"] = CommandType::FPUSH_FORWARD; 
    commandMap["FDEL_HEAD"] = CommandType::FDEL_HEAD;
    commandMap["FDEL_BACK"] = CommandType::FDEL_BACK; 
    commandMap["FDEL_BY_VALUE"] = CommandType::FDEL_BY_VALUE;
    commandMap["FGET_BY_VALUE"] = CommandType::FGET_BY_VALUE; 
    // Двусвязный список
    commandMap["LCREATE"] = CommandType::LCREATE; 
    commandMap["LPUSH_HEAD"] = CommandType::LPUSH_HEAD;
    commandMap["LPUSH_BACK"] = CommandType::LPUSH_BACK; 
    commandMap["LPUSH_BEFORE"] = CommandType::LPUSH_BEFORE;
    commandMap["LPUSH_AFTER"] = CommandType::LPUSH_AFTER; 
    commandMap["LDEL_HEAD"] = CommandType::LDEL_HEAD;
    commandMap["LDEL_BACK"] = CommandType::LDEL_BACK; 
    commandMap["LDEL_BY_VALUE"] = CommandType::LDEL_BY_VALUE;
    commandMap["LGET_BY_VALUE"] = CommandType::LGET_BY_VALUE; 
    // Полное бинарное дерево
    commandMap["TINSERT"] = CommandType::TINSERT; 
    commandMap["TFULL"] = CommandType::TFULL;
}

void loadStructureTypes() {
    ifstream meta_file(META_FILENAME);
    if (!meta_file.is_open()) return;
    string name, type_str;
    while (meta_file >> name >> type_str) {
        DS_Type type = DS_Type::UNKNOWN;
        if (type_str == "ARRAY") type = DS_Type::ARRAY;
        else if (type_str == "STACK") type = DS_Type::STACK;
        else if (type_str == "QUEUE") type = DS_Type::QUEUE;
        else if (type_str == "SINGLY_LIST") type = DS_Type::SINGLY_LIST;
        else if (type_str == "DOUBLY_LIST") type = DS_Type::DOUBLY_LIST;
        else if (type_str == "BINARY_TREE") type = DS_Type::BINARY_TREE;
        if (type != DS_Type::UNKNOWN) structureTypeMap[name] = type;
    }
    meta_file.close();
}

void saveStructureType(const string& name, DS_Type type) {
    if (structureTypeMap.count(name)) return;
    structureTypeMap[name] = type;
    ofstream meta_file(META_FILENAME, ios::app);
    if (!meta_file.is_open()) { cout << "Критическая ошибка: не удалось открыть метафайл для записи.\n"; return; }
    string type_str = "UNKNOWN";
    switch (type) {
    case DS_Type::ARRAY: type_str = "ARRAY"; break;
    case DS_Type::STACK: type_str = "STACK"; break;
    case DS_Type::QUEUE: type_str = "QUEUE"; break;
    case DS_Type::SINGLY_LIST: type_str = "SINGLY_LIST"; break;
    case DS_Type::DOUBLY_LIST: type_str = "DOUBLY_LIST"; break;
    case DS_Type::BINARY_TREE: type_str = "BINARY_TREE"; break;
    default: break;
    }
    meta_file << name << " " << type_str << endl;
    meta_file.close();
}

void printHelp() {
    cout << "\n--- СИСТЕМА УПРАВЛЕНИЯ СТРУКТУРАМИ ДАННЫХ\n";
    cout << "Данные сохраняются в файлы после каждой операции изменения.\n";
    cout << "Формат команд: COMMAND <StructureName> [Arguments...]\n\n";

    cout << "--- ОБЩИЕ КОМАНДЫ\n";
    cout << "HELP              - Показать это сообщение\n";
    cout << "EXIT              - Выход из программы\n";
    cout << "PRINT <Name>      - Вывести содержимое структуры <Name>.\n\n";

    cout << "--- ДИНАМИЧЕСКИЙ МАССИВ (Array) - Префикс 'M'\n";
    cout << "MPUSH_BACK <Name> <Value>   - Добавить <Value> в конец массива.\n";
    cout << "MPUSH_BY_IND <Name> <Ind> <Value> - Вставить <Value> по индексу <Ind>.\n";
    cout << "MGET_BY_IND <Name> <Ind>    - Получить элемент по индексу <Ind>.\n";
    cout << "MDEL_BY_IND <Name> <Ind>    - Удалить элемент по индексу <Ind>.\n";
    cout << "MSWAP_BY_IND <Name> <Ind> <Value> - Заменить элемент по индексу <Ind> на <Value>.\n\n";

    cout << "--- СТЕК (Stack) - Префикс 'S'\n";
    cout << "SPUSH <Name> <Value>      - Поместить <Value> в стек.\n";
    cout << "SPOP <Name>               - Извлечь (удалить) верхний элемент стека.\n\n";

    cout << "--- ОЧЕРЕДЬ (Queue) - Префикс 'Q'\n";
    cout << "QPUSH <Name> <Value>      - Добавить <Value> в конец очереди.\n";
    cout << "QPOP <Name>               - Извлечь (удалить) элемент из начала очереди и вывести его.\n";
    cout << "QGET <Name>               - Получить элемент из начала очереди (без удаления) и вывести его.\n\n";

    cout << "--- ОДНОСВЯЗНЫЙ СПИСОК (Singly Linked List) - Префикс 'F'\n";
    cout << "FCREATE <Name> <Value>    - Создать список с одним элементом <Value>.\n";
    cout << "FPUSH_HEAD <Name> <Value> - Добавить <Value> в начало списка.\n";
    cout << "FPUSH_BACK <Name> <Value> - Добавить <Value> в конец списка.\n";
    cout << "FPUSH_BEFORE <Name> <Target> <Value> - Вставить <Value> перед элементом <Target>.\n";
    cout << "FPUSH_FORWARD <Name> <Target> <Value> - Вставить <Value> после элемента <Target>.\n";
    cout << "FDEL_HEAD <Name>          - Удалить элемент из начала списка.\n";
    cout << "FDEL_BACK <Name>          - Удалить элемент из конца списка.\n";
    cout << "FDEL_BY_VALUE <Name> <Value> - Удалить первый найденный элемент со значением <Value>.\n";
    cout << "FGET_BY_VALUE <Name> <Value> - Проверить наличие элемента со значением <Value>.\n\n";

    cout << "--- ДВУСВЯЗНЫЙ СПИСОК (Doubly Linked List) - Префикс 'L'\n";
    cout << "LCREATE <Name> <Value>    - Создать список с одним элементом <Value>.\n";
    cout << "LPUSH_HEAD <Name> <Value> - Добавить <Value> в начало списка.\n";
    cout << "LPUSH_BACK <Name> <Value> - Добавить <Value> в конец списка.\n";
    cout << "LPUSH_BEFORE <Name> <Target> <Value> - Вставить <Value> перед элементом <Target>.\n";
    cout << "LPUSH_AFTER <Name> <Target> <Value> - Вставить <Value> после элемента <Target>.\n";
    cout << "LDEL_HEAD <Name>          - Удалить элемент из начала списка.\n";
    cout << "LDEL_BACK <Name>          - Удалить элемент из конца списка.\n";
    cout << "LDEL_BY_VALUE <Name> <Value> - Удалить первый найденный элемент со значением <Value>.\n";
    cout << "LGET_BY_VALUE <Name> <Value> - Проверить наличие элемента со значением <Value>.\n\n";

    cout << "--- ПОЛНОЕ БИНАРНОЕ ДЕРЕВО (Full Binary Tree) - Префикс 'T'\n";
    cout << "TINSERT <Name> <Value>    - Вставить <Value> в дерево.\n";
    cout << "TFULL <Name>              - Проверить, является ли дерево полным.\n";
}