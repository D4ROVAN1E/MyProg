#include "set.hpp"
#include <iostream>
#include <string>
#include <cassert>
#include <cstdio> // Для remove

using namespace std;

// Функция для запуска тестов
void run_tests() {
    cout << "--- Запуск тестов HashSet<int> ---" << endl;

    // 1. Тест конструктора и GetSize
    HashSet<int> intSet(4);
    assert(intSet.GetSize() == 0);
    cout << "Тест 1: Конструктор и GetSize пройден." << endl;

    // 2. Тест SETADD и SET_AT
    intSet.SETADD(10);
    intSet.SETADD(20);
    assert(intSet.GetSize() == 2);
    assert(intSet.SET_AT(10) == true);
    assert(intSet.SET_AT(20) == true);
    assert(intSet.SET_AT(30) == false);
    cout << "Тест 2: SETADD и SET_AT пройден." << endl;

    // 3. Тест добавления дубликата
    intSet.SETADD(10); // Добавляем 10 еще раз
    assert(intSet.GetSize() == 2); // Размер должен остаться 2
    cout << "Тест 3: Добавление дубликата проигнорировано." << endl;

    // 4. Тест SETDEL
    intSet.SETDEL(10);
    assert(intSet.GetSize() == 1);
    assert(intSet.SET_AT(10) == false); // 10 должно быть удалено
    intSet.SETDEL(30); // Удаление несуществующего элемента
    assert(intSet.GetSize() == 1); // Размер не должен измениться
    cout << "Тест 4: SETDEL пройден." << endl;

    // 5. Тест автоматического изменения размера (Resize)
    // Изначальный размер таблицы 4, коэффициент загрузки 0.75, порог = 4 * 0.75 = 3
    // Уже есть 1 элемент (20). Добавим еще 2, затем 1 для срабатывания ресайза.
    intSet.SETADD(40); // Size = 2
    intSet.SETADD(50); // Size = 3
    cout << "Перед ресайзом, размер множества: " << intSet.GetSize() << endl;
    
    // Этот вызов должен запустить checkAndResize, так как 4 > 4 * 0.75 (т.е. 3)
    intSet.SETADD(60); // Size = 4, должно произойти увеличение размера таблицы (с 4 до 8)
    
    assert(intSet.GetSize() == 4);
    assert(intSet.SET_AT(20) == true);
    assert(intSet.SET_AT(60) == true);
    // Проверить размер внутренней таблицы сложнее без доступа к приватным полям, 
    // но если все элементы на месте, ресайз, вероятно, сработал.
    cout << "Тест 5: Ресайз (Resize) пройден (проверено сохранение элементов)." << endl;
    // intSet.PRINT(); // Можно раскомментировать для визуальной проверки

    // 6. Тест HashSet<string>
    cout << "\n--- Запуск тестов HashSet<string> ---" << endl;
    HashSet<string> strSet(2);
    strSet.SETADD("apple");
    strSet.SETADD("banana");
    assert(strSet.GetSize() == 2);
    assert(strSet.SET_AT("apple") == true);
    assert(strSet.SET_AT("orange") == false);
    strSet.SETDEL("apple");
    assert(strSet.GetSize() == 1);
    assert(strSet.SET_AT("apple") == false);
    cout << "Тест 6: HashSet<string> пройден." << endl;

    // 7. Тест копирующего конструктора и оператора присваивания
    HashSet<int> original(2);
    original.SETADD(1);
    original.SETADD(2);
    
    // Копирующий конструктор
    HashSet<int> copy1 = original;
    assert(copy1.GetSize() == 2);
    assert(copy1.SET_AT(1) == true);

    // Копирующий оператор присваивания
    HashSet<int> copy2(10);
    copy2 = original;
    assert(copy2.GetSize() == 2);
    assert(copy2.SET_AT(2) == true);

    // Проверка независимости (Deep Copy)
    original.SETADD(3);
    assert(original.GetSize() == 3);
    assert(copy1.GetSize() == 2); 
    assert(copy2.SET_AT(3) == false);
    cout << "Тест 7: Копирование и независимость пройдены." << endl;

    // 8. Тест сохранения и загрузки
    cout << "\n--- Тест сохранения и загрузки ---" << endl;
    const string filename = "hashset_test.txt";
    HashSet<int> saveSet(8);
    saveSet.SETADD(100);
    saveSet.SETADD(200);
    saveSet.SETADD(300);
    saveSet.SETSAVE(filename); // Сохраняем

    HashSet<int> loadSet(16);
    assert(loadSet.GetSize() == 0);
    loadSet.SETLOAD(filename); // Загружаем

    // Проверяем, что загруженное множество имеет те же элементы
    assert(loadSet.GetSize() == 3);
    assert(loadSet.SET_AT(100) == true);
    assert(loadSet.SET_AT(200) == true);
    assert(loadSet.SET_AT(300) == true);
    assert(loadSet.SET_AT(400) == false);
    cout << "Тест 8: Сохранение и загрузка пройдены." << endl;

    // Очистка тестового файла
    if (remove(filename.c_str()) != 0) {
        perror("Ошибка при удалении тестового файла");
    }
    
    cout << "\nВсе тесты успешно пройдены!" << endl;
}

int main() {
    try {
        run_tests();
    } catch (const exception& e) {
        cerr << "Произошло исключение: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Произошло неизвестное исключение." << endl;
        return 1;
    }
    
    return 0;
}