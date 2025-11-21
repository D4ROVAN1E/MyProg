#include "dh.hpp"
#include <iostream>
#include <cassert>
#include <string>

using namespace std;

// Использовать шаблонный тип, например float
using DoubleHashFloat = DoubleHash<float>;

void test_insert_find_remove() {
    cout << "--- Тест: Вставка, Поиск, Удаление ---" << endl;
    DoubleHashFloat dh(5);

    // 1. Вставка и проверка размера
    dh.insert("data1", 10.5f);
    dh.insert("data2", 20.0f);
    assert(dh.size() == 2);

    // 2. Поиск
    assert(*dh.find("data1") == 10.5f);
    assert(*dh.find("data2") == 20.0f);
    assert(dh.find("data3") == nullptr);

    // 3. Обновление значения
    dh.insert("data1", 100.1f);
    assert(*dh.find("data1") == 100.1f);
    assert(dh.size() == 2);

    // 4. Удаление
    assert(dh.remove("data2"));
    assert(dh.size() == 1);
    assert(dh.find("data2") == nullptr);

    // 5. Удаление несуществующего элемента
    assert(!dh.remove("nonexistent"));
    assert(dh.size() == 1);

    // 6. Очистка
    dh.clear();
    assert(dh.empty());

    cout << "Тест 1 пройден успешно." << endl;
}

void test_resize() {
    cout << "--- Тест: Автоматическое изменение размера (Resize) ---" << endl;
    DoubleHashFloat dh(3); // Начальный размер 3. Порог загрузки 0.7.
    
    // 3 * 0.7 = 2.1. Resize должен произойти при вставке 3-го элемента.
    
    // 1. Вставка 1 и 2
    dh.insert("key1", 1.0f);
    dh.insert("key2", 2.0f);
    assert(dh.size() == 2);
    // Resize еще не произошел

    // 2. Вставка 3. Должен сработать resize
    // Новый размер: 3 * 2 + 1 = 7.
    dh.insert("key3", 3.0f);
    assert(dh.size() == 3); 

    // Проверяем, что элементы сохранились после перехэширования
    assert(*dh.find("key1") == 1.0f);
    assert(*dh.find("key3") == 3.0f);
    
    // Вставляем больше элементов для заполнения новой таблицы
    dh.insert("key4", 4.0f);
    dh.insert("key5", 5.0f);
    dh.insert("key6", 6.0f);

    assert(dh.size() == 6);
    assert(*dh.find("key6") == 6.0f);

    cout << "Тест 2 пройден успешно." << endl;
}

void test_collision_handling() {
    cout << "--- Тест: Обработка коллизий ---" << endl;
    DoubleHashFloat dh(7); // Размер 7
    
    // Предполагаем, что эти ключи вызовут коллизии
    // Примечание: Для реального тестирования коллизий нужно знать hash1 и hash2.
    // Тут мы просто вставляем много элементов, чтобы гарантировать коллизии
    // и проверить, что find все еще работает.
    dh.insert("a", 1.1f);
    dh.insert("b", 2.2f);
    dh.insert("c", 3.3f);
    dh.insert("d", 4.4f);
    dh.insert("e", 5.5f);
    dh.insert("f", 6.6f);
    dh.insert("g", 7.7f); 

    assert(dh.size() == 7);
    assert(*dh.find("d") == 4.4f);
    assert(*dh.find("g") == 7.7f);
    
    // Удаляем элемент, который, возможно, находится в цепочке коллизий
    assert(dh.remove("a"));
    assert(dh.size() == 6);
    assert(dh.find("a") == nullptr);
    
    // Проверяем, что элементы после удаленного все еще доступны
    assert(*dh.find("c") == 3.3f);

    cout << "Тест 3 пройден успешно." << endl;
}

int main() {
    try {
        test_insert_find_remove();
        test_resize();
        test_collision_handling();
        
        cout << "\n✅ Все тесты Double Hash пройдены успешно!" << endl;
    } catch (const exception& e) {
        cerr << "\n❌ Ошибка в тесте Double Hash: " << e.what() << endl;
        return 1;
    }
    return 0;
}