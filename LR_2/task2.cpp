#include "set.h"
#include <iostream>

using namespace std;

int main() {
    setlocale(LC_ALL, "ru");
    // Создание множества для целых чисел
    HashSet<int> intSet(8);

    cout << "=== Тестирование множества целых чисел ===" << endl;

    // Добавление элементов
    cout << "\nДобавление элементов: 10, 20, 30, 40, 50" << endl;
    SETADD(intSet, 10);
    SETADD(intSet, 20);
    SETADD(intSet, 30);
    SETADD(intSet, 40);
    SETADD(intSet, 50);

    PRINT(intSet);

    // Проверка наличия элементов
    cout << "\nПроверка наличия элементов:" << endl;
    cout << "Есть ли 30? " << (SET_AT(intSet, 30) ? "Да" : "Нет") << endl;
    cout << "Есть ли 100? " << (SET_AT(intSet, 100) ? "Да" : "Нет") << endl;

    // Попытка добавить дубликат
    cout << "\nПопытка добавить дубликат (20):" << endl;
    SETADD(intSet, 20);
    cout << "Количество элементов: " << intSet.elementCount << " (должно остаться прежним)" << endl;

    // Удаление элемента
    cout << "\nУдаление элемента 30:" << endl;
    SETDEL(intSet, 30);
    cout << "Есть ли 30? " << (SET_AT(intSet, 30) ? "Да" : "Нет") << endl;
    PRINT(intSet);

    // Добавление большего количества элементов для тестирования расширения
    cout << "\nДобавление элементов для тестирования расширения таблицы:" << endl;
    for (int i = 60; i < 100; i += 10) {
        SETADD(intSet, i);
    }
    PRINT(intSet);

    // Сохранение в файл
    cout << "\nСохранение множества в файл..." << endl;
    SETSAVE(intSet, string("set_data.txt"));

    // Создание нового множества и загрузка из файла
    cout << "\nЗагрузка множества из файла..." << endl;
    HashSet<int> loadedSet(8);
    SETLOAD(loadedSet, string("set_data.txt"));

    cout << "\nЗагруженное множество:" << endl;
    PRINT(loadedSet);

    // Проверка работы с загруженными данными
    cout << "\nПроверка загруженных данных:" << endl;
    cout << "Есть ли 50? " << (SET_AT(loadedSet, 50) ? "Да" : "Нет") << endl;
    cout << "Есть ли 30? " << (SET_AT(loadedSet, 30) ? "Да" : "Нет") << endl;

    // Тестирование с строками
    cout << "\n\n=== Тестирование множества строк ===" << endl;
    HashSet<string> stringSet(8);

    SETADD(stringSet, string("apple"));
    SETADD(stringSet, string("banana"));
    SETADD(stringSet, string("cherry"));
    SETADD(stringSet, string("date"));

    cout << "\nМножество строк:" << endl;
    PRINT(stringSet);

    cout << "\nПроверка наличия:" << endl;
    cout << "Есть ли 'banana'? " << (SET_AT(stringSet, string("banana")) ? "Да" : "Нет") << endl;
    cout << "Есть ли 'grape'? " << (SET_AT(stringSet, string("grape")) ? "Да" : "Нет") << endl;

    SETDEL(stringSet, string("banana"));
    cout << "\nПосле удаления 'banana':" << endl;
    cout << "Есть ли 'banana'? " << (SET_AT(stringSet, string("banana")) ? "Да" : "Нет") << endl;

    return 0;
}