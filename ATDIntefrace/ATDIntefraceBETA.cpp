#include <iostream>
#include <string>
#include <limits>
#include "array.h"
#include "stack.h"
#include "queue.h"
#include "singly_list.h"
#include "doubly_list.h"
#include "binary_tree.h"

using namespace std;

// Прототипы функций для каждой структуры данных
void arrayMenu();
void stackMenu();
void queueMenu();
void singlyListMenu();
void doublyListMenu();
void binaryTreeMenu();

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    setlocale(LC_ALL, "ru");

    int choice;

    while (true) {
        cout << "СИСТЕМА УПРАВЛЕНИЯ СТРУКТУРАМИ ДАННЫХ" << endl;
        cout << "\nВыберите структуру данных:" << endl;
        cout << "1. Динамический массив (Array)" << endl;
        cout << "2. Стек (Stack)" << endl;
        cout << "3. Очередь (Queue)" << endl;
        cout << "4. Односвязный список (Singly Linked List)" << endl;
        cout << "5. Двусвязный список (Doubly Linked List)" << endl;
        cout << "6. Полное бинарное дерево (Full Binary Tree)" << endl;
        cout << "0. Выход" << endl;
        cout << "\nВаш выбор: ";

        cin >> choice;

        if (cin.fail()) {
            clearInput();
            cout << "Ошибка ввода! Введите число." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            arrayMenu();
            break;
        case 2:
            stackMenu();
            break;
        case 3:
            queueMenu();
            break;
        case 4:
            singlyListMenu();
            break;
        case 5:
            doublyListMenu();
            break;
        case 6:
            binaryTreeMenu();
            break;
        case 0:
            cout << "\nЗавершение работы программы..." << endl;
            return 0;
        default:
            cout << "Неверный выбор! Попробуйте снова." << endl;
        }
    }

    return 0;
}

//Массив
void arrayMenu() {
    Array<int> arr;
    int choice, value, index;
    string filename;

    while (true) {
        cout << "\n--- МЕНЮ ДИНАМИЧЕСКОГО МАССИВА ---" << endl;
        cout << "1. Добавить элемент в конец" << endl;
        cout << "2. Добавить элемент по индексу" << endl;
        cout << "3. Получить элемент по индексу" << endl;
        cout << "4. Удалить элемент по индексу" << endl;
        cout << "5. Заменить элемент по индексу" << endl;
        cout << "6. Показать массив" << endl;
        cout << "7. Сохранить в файл" << endl;
        cout << "8. Загрузить из файла" << endl;
        cout << "0. Назад" << endl;
        cout << "Выбор: ";

        cin >> choice;

        if (cin.fail()) {
            clearInput();
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Введите значение: ";
            cin >> value;
            MPUSH_BACK(arr, value);
            cout << "Элемент добавлен!" << endl;
            break;
        case 2:
            cout << "Введите индекс: ";
            cin >> index;
            cout << "Введите значение: ";
            cin >> value;
            MPUSH_BY_IND(arr, index, value);
            break;
        case 3:
            cout << "Введите индекс: ";
            cin >> index;
            cout << "Элемент: " << MGET_BY_IND(arr, index) << endl;
            break;
        case 4:
            cout << "Введите индекс: ";
            cin >> index;
            MDEL_BY_IND(arr, index);
            break;
        case 5:
            cout << "Введите индекс: ";
            cin >> index;
            cout << "Введите новое значение: ";
            cin >> value;
            MSWAP_BY_IND(arr, index, value);
            break;
        case 6:
            cout << "Массив: ";
            PRINT(arr);
            cout << "Размер: " << arr.size << ", Ёмкость: " << arr.capacity << endl;
            break;
        case 7:
            cout << "Введите имя файла: ";
            cin >> filename;
            MSAVE(arr, filename);
            break;
        case 8:
            cout << "Введите имя файла: ";
            cin >> filename;
            MLOAD(arr, filename);
            break;
        case 0:
            return;
        default:
            cout << "Неверный выбор!" << endl;
        }
    }
}

//Стек
void stackMenu() {
    Stack<int> stack;
    int choice, value;
    string filename;

    while (true) {
        cout << "\n--- МЕНЮ СТЕКА ---" << endl;
        cout << "1. Добавить элемент (PUSH)" << endl;
        cout << "2. Удалить элемент (POP)" << endl;
        cout << "3. Показать стек" << endl;
        cout << "4. Сохранить в файл" << endl;
        cout << "5. Загрузить из файла" << endl;
        cout << "0. Назад" << endl;
        cout << "Выбор: ";

        cin >> choice;

        if (cin.fail()) {
            clearInput();
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Введите значение: ";
            cin >> value;
            SPUSH(stack, value);
            cout << "Элемент добавлен!" << endl;
            break;
        case 2:
            if (stack.size > 0) {
                SPOP(stack);
                cout << "Элемент удалён!" << endl;
            }
            else {
                cout << "Стек пуст!" << endl;
            }
            break;
        case 3:
            cout << "Стек: ";
            PRINT(stack);
            cout << "Размер: " << stack.size << endl;
            break;
        case 4:
            cout << "Введите имя файла: ";
            cin >> filename;
            SSAVE(stack, filename);
            break;
        case 5:
            cout << "Введите имя файла: ";
            cin >> filename;
            SLOAD(stack, filename);
            break;
        case 0:
            return;
        default:
            cout << "Неверный выбор!" << endl;
        }
    }
}

//Очередь
void queueMenu() {
    Queue<int> queue;
    int choice, value;
    string filename;

    while (true) {
        cout << "\n--- МЕНЮ ОЧЕРЕДИ ---" << endl;
        cout << "1. Добавить элемент (PUSH)" << endl;
        cout << "2. Извлечь элемент (POP)" << endl;
        cout << "3. Посмотреть первый элемент" << endl;
        cout << "4. Показать очередь" << endl;
        cout << "5. Сохранить в файл" << endl;
        cout << "6. Загрузить из файла" << endl;
        cout << "0. Назад" << endl;
        cout << "Выбор: ";

        cin >> choice;

        if (cin.fail()) {
            clearInput();
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Введите значение: ";
            cin >> value;
            QPUSH(queue, value);
            cout << "Элемент добавлен!" << endl;
            break;
        case 2:
            try {
                value = QPOP(queue);
                cout << "Извлечён элемент: " << value << endl;
            }
            catch (const exception& e) {
                cout << e.what() << endl;
            }
            break;
        case 3:
            try {
                value = QGET(queue);
                cout << "Первый элемент: " << value << endl;
            }
            catch (const exception& e) {
                cout << e.what() << endl;
            }
            break;
        case 4:
            PRINT(queue);
            cout << "Размер: " << getSize(queue) << endl;
            break;
        case 5:
            cout << "Введите имя файла: ";
            cin >> filename;
            QSAVE(queue, filename);
            break;
        case 6:
            cout << "Введите имя файла: ";
            cin >> filename;
            QLOAD(queue, filename);
            break;
        case 0:
            return;
        default:
            cout << "Неверный выбор!" << endl;
        }
    }
}

//Односвязный список
void singlyListMenu() {
    ForwardList<int> list;
    int choice, value, target;
    string filename;

    while (true) {
        cout << "\n--- МЕНЮ ОДНОСВЯЗНОГО СПИСКА ---" << endl;
        cout << "1. Создать список с начальным элементом" << endl;
        cout << "2. Добавить в начало" << endl;
        cout << "3. Добавить в конец" << endl;
        cout << "4. Добавить перед элементом" << endl;
		cout << "5. Добавить после элемента" << endl;
        cout << "6. Удалить первый элемент" << endl;
        cout << "7. Удалить последний элемент" << endl;
        cout << "8. Удалить по значению" << endl;
        cout << "9. Найти элемент" << endl;
        cout << "10. Показать список" << endl;
        cout << "11. Сохранить в файл" << endl;
        cout << "12. Загрузить из файла" << endl;
        cout << "0. Назад" << endl;
        cout << "Выбор: ";

        cin >> choice;

        if (cin.fail()) {
            clearInput();
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Введите значение: ";
            cin >> value;
            FCREATE(list, value);
            cout << "Список создан!" << endl;
            break;
        case 2:
            cout << "Введите значение: ";
            cin >> value;
            FPUSH_HEAD(list, value);
            break;
        case 3:
            cout << "Введите значение: ";
            cin >> value;
            FPUSH_BACK(list, value);
            break;
        case 4:
            cout << "Введите значение целевого элемента: ";
            cin >> target;
            cout << "Введите новое значение: ";
            cin >> value;
            FPUSH_BEFORE(list, target, value);
            break;
        case 5:
            cout << "Введите значение целевого элемента: ";
            cin >> target;
            cout << "Введите новое значение: ";
            cin >> value;
            FPUSH_FORWARD(FGET_BY_VALUE(list, target), value);
			break;
        case 6:
            FDEL_HEAD(list);
            break;
        case 7:
            FDEL_BACK(list);
            break;
        case 8:
            cout << "Введите значение для удаления: ";
            cin >> value;
            FDEL_BY_VALUE(list, value);
            break;
        case 9: {
            cout << "Введите значение для поиска: ";
            cin >> value;
            SNode<int>* foundNode = FGET_BY_VALUE(list, value);
            if (foundNode) {
                cout << "Элемент найден по адресу: " << foundNode << endl;
            }
            else {
                cout << "Элемент не найден!" << endl;
            }
            break;
        }
        case 10:
            PRINT(list);
            break;
        case 11:
            cout << "Введите имя файла: ";
            cin >> filename;
            FSAVE(list, filename);
            break;
        case 12:
            cout << "Введите имя файла: ";
            cin >> filename;
            FLOAD(list, filename);
            break;
        case 0:
            return;
        default:
            cout << "Неверный выбор!" << endl;
        }
    }
}

//Двусвязный список
void doublyListMenu() {
    DoublyList<int> list;
    int choice, value, target;
    string filename;

    while (true) {
        cout << "\n--- МЕНЮ ДВУСВЯЗНОГО СПИСКА ---" << endl;
        cout << "1. Создать список с начальным элементом" << endl;
        cout << "2. Добавить в начало" << endl;
        cout << "3. Добавить в конец" << endl;
        cout << "4. Добавить перед элементом" << endl;
		cout << "5. Добавить после элемента" << endl;
        cout << "6. Удалить первый элемент" << endl;
        cout << "7. Удалить последний элемент" << endl;
        cout << "8. Удалить по значению" << endl;
        cout << "9. Найти элемент" << endl;
        cout << "10. Показать список (прямо)" << endl;
        cout << "11. Показать список (обратно)" << endl;
        cout << "12. Сохранить в файл" << endl;
        cout << "13. Загрузить из файла" << endl;
        cout << "0. Назад" << endl;
        cout << "Выбор: ";

        cin >> choice;

        if (cin.fail()) {
            clearInput();
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Введите значение: ";
            cin >> value;
            LCREATE(list, value);
            cout << "Список создан!" << endl;
            break;
        case 2:
            cout << "Введите значение: ";
            cin >> value;
            LPUSH_HEAD(list, value);
            break;
        case 3:
            cout << "Введите значение: ";
            cin >> value;
            LPUSH_BACK(list, value);
            break;
        case 4:
            cout << "Введите значение целевого элемента: ";
            cin >> target;
            cout << "Введите новое значение: ";
            cin >> value;
            LPUSH_BEFORE(list, target, value);
            break;
        case 5:
            cout << "Введите значение целевого элемента: ";
            cin >> target;
            cout << "Введите новое значение: ";
            cin >> value;
			LPUSH_AFTER(list, target, value);
            break;
        case 6:
            LDEL_HEAD(list);
            break;
        case 7:
            LDEL_BACK(list);
            break;
        case 8:
            cout << "Введите значение для удаления: ";
            cin >> value;
            LDEL_BY_VALUE(list, value);
            break;
        case 9: {
            cout << "Введите значение для поиска: ";
            cin >> value;
            DNode<int>* foundNode = LGET_BY_VALUE(list, value);
            if (foundNode) {
                cout << "Элемент найден по адресу " << foundNode << endl;
            }
            else {
                cout << "Элемент не найден!" << endl;
            }
            break;
        }
        case 10:
			PRINT(list, 1);
            break;
        case 11:
			PRINT(list, 2);
            break;
        case 12:
            cout << "Введите имя файла: ";
            cin >> filename;
            LSAVE(list, filename);
            break;
        case 13:
            cout << "Введите имя файла: ";
            cin >> filename;
            LLOAD(list, filename);
            break;
        case 0:
            return;
        default:
            cout << "Неверный выбор!" << endl;
        }
    }
}

//Полное бинарное дерево
void binaryTreeMenu() {
    FullBinaryTree<int> tree;
    int choice, value;
    string filename;

    while (true) {
        cout << "\n--- МЕНЮ ПОЛНОГО БИНАРНОГО ДЕРЕВА ---" << endl;
        cout << "1. Вставить элемент" << endl;
        cout << "2. Проверить, является ли дерево полным" << endl;
        cout << "3. Показать дерево (обход в ширину)" << endl;
        cout << "4. Показать дерево (прямой обход)" << endl;
        cout << "5. Показать дерево (центрированный обход)" << endl;
        cout << "6. Показать дерево (обратный обход)" << endl;
        cout << "7. Паказать дерево наглядно" << endl;
        cout << "8. Сохранить в файл" << endl;
        cout << "9. Загрузить из файла" << endl;
        cout << "0. Назад" << endl;
        cout << "Выбор: ";

        cin >> choice;

        if (cin.fail()) {
            clearInput();
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Введите значение: ";
            cin >> value;
            TINSERT(tree, value);
            cout << "Элемент добавлен!" << endl;
            break;
        case 2:
            if (tree.root) {
                cout << "Дерево является полным: "
                    << (TFULL(tree.root) ? "Да" : "Нет") << endl;
            }
            else {
                cout << "Дерево пусто!" << endl;
            }
            break;
        case 3:
            cout << "Обход в ширину: ";
			PRINT(tree, 1);
            break;
        case 4:
            cout << "Прямой обход: ";
			PRINT(tree, 2);
            break;
        case 5:
            cout << "Центрированный обход: ";
			PRINT(tree, 3);
            break;
        case 6:
            cout << "Обратный обход: ";
			PRINT(tree, 4);
            break;
        case 7:
            cout << "Наглядный вывод: \n";
            PRINT(tree, 5);
            break;
        case 8:
            cout << "Введите имя файла: ";
            cin >> filename;
            TSAVE(tree, filename);
            break;
        case 9:
            cout << "Введите имя файла: ";
            cin >> filename;
            TLOAD(tree, filename);
            break;
        case 0:
            return;
        default:
            cout << "Неверный выбор!" << endl;
        }
    }
}
