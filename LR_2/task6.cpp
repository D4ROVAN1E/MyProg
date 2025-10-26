#include <iostream>
#include <string>
#include "ch.hpp" // Подключаем нашу новую реализацию

using namespace std;

int main() {
    setlocale(LC_ALL, "Russian"); 

    cout << "--- Тестирование CuckooHash<int> ---" << endl;
    CuckooHash<int> ht(5); // Начинаем с маленького размера

    cout << "Пустая таблица? " << (ht.empty() ? "Да" : "Нет") << endl;
    cout << "Размер: " << ht.size() << endl;

    cout << "\n--- Вставка элементов ---" << endl;
    ht.insert("apple", 10);
    ht.insert("banana", 20);
    ht.insert("orange", 30);
    ht.print();

    // Эта вставка должна вызвать resize (5 * 0.5 = 2.5, 3-й элемент вызывает resize)
    cout << "\n--- Вставка, вызывающая resize ---" << endl;
    ht.insert("grape", 40);
    ht.print();

    ht.insert("melon", 50);
    ht.print();

    cout << "Пустая таблица? " << (ht.empty() ? "Да" : "Нет") << endl;
    cout << "Размер: " << ht.size() << endl;

    cout << "\n--- Поиск элементов ---" << endl;
    int* val1 = ht.find("banana");
    if (val1) {
        cout << "Найден 'banana': " << *val1 << endl;
    }
    else {
        cout << "Не найден 'banana'" << endl;
    }

    int* val2 = ht.find("coconut");
    if (val2) {
        cout << "Найден 'coconut': " << *val2 << endl;
    }
    else {
        cout << "Не найден 'coconut'" << endl;
    }

    cout << "\n--- Обновление элемента ---" << endl;
    ht.insert("apple", 15); // Обновляем значение
    int* val3 = ht.find("apple");
    if (val3) {
        cout << "Обновленное значение 'apple': " << *val3 << endl;
    }
    ht.print();

    cout << "\n--- Удаление элемента ---" << endl;
    bool removed = ht.remove("orange");
    cout << "Удален 'orange'? " << (removed ? "Да" : "Нет") << endl;
    cout << "Размер после удаления: " << ht.size() << endl;
    ht.print();

    // Пытаемся найти удаленный элемент
    int* val4 = ht.find("orange");
    if (val4) {
        cout << "Найден 'orange' (ошибка): " << *val4 << endl;
    }
    else {
        cout << "Не найден 'orange' (корректно)" << endl;
    }

    // Вставляем новый элемент (он может занять место 'orange')
    ht.insert("kiwi", 60);
    ht.print();

    // Вставка, которая может вызвать "выталкивания"
    cout << "\n--- Вставка с выталкиваниями ---" << endl;
    ht.insert("lemon", 70);
    ht.insert("peach", 80);
    ht.insert("pear", 90);
    ht.print();


    cout << "\n--- Тестирование CuckooHash<string> ---" << endl;
    CuckooHash<string> ht_str(3);
    ht_str.insert("key1", "value1");
    ht_str.insert("key2", "value2");
    ht_str.insert("key3", "value3"); // Должен вызвать resize
    ht_str.insert("key4", "value4");
    ht_str.print();

    string* str_val = ht_str.find("key2");
    if (str_val) {
        cout << "Найден 'key2': " << *str_val << endl;
    }

    cout << "\n--- Очистка таблицы ---" << endl;
    ht_str.clear();
    cout << "Размер после clear: " << ht_str.size() << endl;
    cout << "Пустая таблица после clear? " << (ht_str.empty() ? "Да" : "Нет") << endl;
    ht_str.print();

    return 0;
}