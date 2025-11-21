#include "ch.hpp"
#include <iostream>
#include <cassert>
#include <string>

using namespace std;

// Использовать шаблонный тип, например int
using CuckooHashInt = CuckooHash<int>;

void test_insert_find_remove() {
    cout << "--- Тест: Вставка, Поиск, Удаление ---" << endl;
    CuckooHashInt ch(5);

    // 1. Вставка и проверка размера
    ch.insert("apple", 10);
    ch.insert("banana", 20);
    assert(ch.size() == 2);

    // 2. Поиск
    assert(*ch.find("apple") == 10);
    assert(*ch.find("banana") == 20);
    assert(ch.find("orange") == nullptr);

    // 3. Обновление значения (перезапись)
    ch.insert("apple", 100);
    assert(*ch.find("apple") == 100);
    assert(ch.size() == 2); // Размер не должен измениться

    // 4. Удаление
    assert(ch.remove("banana"));
    assert(ch.size() == 1);
    assert(ch.find("banana") == nullptr);

    // 5. Удаление несуществующего элемента
    assert(!ch.remove("banana"));
    assert(ch.size() == 1);

    // 6. Очистка
    ch.clear();
    assert(ch.empty());
    assert(ch.size() == 0);

    cout << "Тест 1 пройден успешно." << endl;
}

void test_resize() {
    cout << "--- Тест: Автоматическое изменение размера (Resize) ---" << endl;
    CuckooHashInt ch(3); // Начальный размер 3. Порог загрузки 0.5 (1 элемент).

    // Вставка 1-го элемента, resize не нужен
    ch.insert("a", 1);
    assert(ch.size() == 1);

    // Вставка 2-го элемента, должен сработать resize, так как load_factor > 0.5
    // Новый размер станет 3 * 2 + 1 = 7.
    ch.insert("b", 2);
    assert(ch.size() == 2);

    // Вставляем больше элементов для заполнения новой таблицы
    ch.insert("c", 3);
    ch.insert("d", 4);
    ch.insert("e", 5);

    assert(ch.size() == 5);
    assert(*ch.find("a") == 1);
    assert(*ch.find("e") == 5);

    cout << "Тест 2 пройден успешно." << endl;
}

void test_copy_constructors() {
    cout << "--- Тест: Копирование и Присваивание ---" << endl;
    CuckooHashInt original(5);
    original.insert("one", 1);
    original.insert("two", 2);

    // Копирующий конструктор
    CuckooHashInt copy_constructor = original;
    assert(copy_constructor.size() == 2);
    assert(*copy_constructor.find("one") == 1);
    
    // Изменяем копию, оригинал должен остаться неизменным
    copy_constructor.insert("three", 3);
    assert(copy_constructor.size() == 3);
    assert(original.size() == 2);

    // Копирующий оператор присваивания
    CuckooHashInt assigned_copy;
    assigned_copy.insert("temp", 99); // Добавляем элемент для проверки очистки
    assigned_copy = original;
    assert(assigned_copy.size() == 2);
    assert(*assigned_copy.find("one") == 1);
    assert(assigned_copy.find("temp") == nullptr);

    cout << "Тест 3 пройден успешно." << endl;
}

int main() {
    try {
        test_insert_find_remove();
        test_resize();
        test_copy_constructors();
        
        cout << "\n✅ Все тесты Cuckoo Hash пройдены успешно!" << endl;
    } catch (const exception& e) {
        cerr << "\n❌ Ошибка в тесте Cuckoo Hash: " << e.what() << endl;
        return 1;
    }
    return 0;
}