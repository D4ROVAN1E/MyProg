#include "set.hpp"
#include <iostream>

using namespace std;

int main() {
    setlocale(LC_ALL, "ru");
    // Создание множества для целых чисел
    HashSet<int> intSet(8);

    cout << "=== Тестирование множества целых чисел ===" << endl;

    // Добавление элементов
    cout << "\nДобавление элементов: 10, 20, 30, 40, 50" << endl;
    intSet.SETADD(10);
    intSet.SETADD(20);
    intSet.SETADD(30);
    intSet.SETADD(40);
    intSet.SETADD(50);

    intSet.PRINT();

    // Проверка наличия элементов
    cout << "\nПроверка наличия элементов:" << endl;
    cout << "Есть ли 30? " << (intSet.SET_AT(30) ? "Да" : "Нет") << endl;
    cout << "Есть ли 100? " << (intSet.SET_AT(100) ? "Да" : "Нет") << endl;

    // Попытка добавить дубликат
    cout << "\nПопытка добавить дубликат (20):" << endl;
    intSet.SETADD(20);
    cout << "Количество элементов: " << intSet.GetSize() << " (должно остаться прежним)" << endl;

    // Удаление элемента
    cout << "\nУдаление элемента 30:" << endl;
    intSet.SETDEL(30);
    cout << "Есть ли 30? " << (intSet.SET_AT(30) ? "Да" : "Нет") << endl;
    intSet.PRINT();

    // Добавление большего количества элементов для тестирования расширения
    cout << "\nДобавление элементов для тестирования расширения таблицы:" << endl;
    for (int i = 60; i < 100; i += 10) {
        intSet.SETADD(i);
    }
    intSet.PRINT();

    // Сохранение в файл
    cout << "\nСохранение множества в файл..." << endl;
    intSet.SETSAVE(string("set_data.txt"));

    // Создание нового множества и загрузка из файла
    cout << "\nЗагрузка множества из файла..." << endl;
    HashSet<int> loadedSet(8);
    loadedSet.SETLOAD(string("set_data.txt"));

    cout << "\nЗагруженное множество:" << endl;
    loadedSet.PRINT();

    // Проверка работы с загруженными данными
    cout << "\nПроверка загруженных данных:" << endl;
    cout << "Есть ли 50? " << (loadedSet.SET_AT(50) ? "Да" : "Нет") << endl;
    cout << "Есть ли 30? " << (loadedSet.SET_AT(30) ? "Да" : "Нет") << endl;

    // Тестирование с строками
    cout << "\n\n=== Тестирование множества строк ===" << endl;
    HashSet<string> stringSet(8);

    stringSet.SETADD(string("apple"));
    stringSet.SETADD(string("banana"));
    stringSet.SETADD(string("cherry"));
    stringSet.SETADD(string("date"));

    cout << "\nМножество строк:" << endl;
    stringSet.PRINT();

    cout << "\nПроверка наличия:" << endl;
    cout << "Есть ли 'banana'? " << (stringSet.SET_AT(string("banana")) ? "Да" : "Нет") << endl;
    cout << "Есть ли 'grape'? " << (stringSet.SET_AT(string("grape")) ? "Да" : "Нет") << endl;

    stringSet.SETDEL(string("banana"));
    cout << "\nПосле удаления 'banana':" << endl;
    cout << "Есть ли 'banana'? " << (stringSet.SET_AT(string("banana")) ? "Да" : "Нет") << endl;

    return 0;
}