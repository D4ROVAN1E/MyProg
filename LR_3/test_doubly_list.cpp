#define BOOST_TEST_MODULE DoublyListTests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>
#include <iostream>
#include <cstdio>
#include "doubly_list.hpp"

using namespace std;

// Вспомогательная структура для перехвата cout
struct CoutRedirect {
    CoutRedirect() {
        old = cout.rdbuf(buffer.rdbuf());
    }
    ~CoutRedirect() {
        cout.rdbuf(old);
    }
    string getString() {
        return buffer.str();
    }
    stringstream buffer;
    streambuf* old;
};

BOOST_AUTO_TEST_SUITE(DoublyListSuite)

// Тесты конструктора и LCREATE
BOOST_AUTO_TEST_CASE(ConstructionAndCreate) {
    DoublyList<int> list;
    
    // Проверка инициализации
    BOOST_CHECK(list.GetHead() == nullptr);
    BOOST_CHECK(list.GetTail() == nullptr);

    // Тест LCREATE
    list.LCREATE(10);
    BOOST_CHECK(list.GetHead() != nullptr);
    BOOST_CHECK_EQUAL(list.GetHead()->key, 10);
    BOOST_CHECK_EQUAL(list.GetTail()->key, 10);

    // LCREATE должно выбрасывать исключение, если список не пуст
    BOOST_CHECK_THROW(list.LCREATE(20), logic_error);
}

// Тесты добавления элементов (HEAD/BACK)
BOOST_AUTO_TEST_CASE(PushHeadAndBack) {
    DoublyList<int> list;
    
    // Добавление в пустой список через HEAD
    list.LPUSH_HEAD(10); // [10]
    BOOST_CHECK_EQUAL(list.GetHead()->key, 10);
    BOOST_CHECK_EQUAL(list.GetTail()->key, 10);

    // Добавление в начало
    list.LPUSH_HEAD(5);  // [5, 10]
    BOOST_CHECK_EQUAL(list.GetHead()->key, 5);
    BOOST_CHECK_EQUAL(list.GetHead()->next->key, 10);

    // Добавление в конец
    list.LPUSH_BACK(20); // [5, 10, 20]
    BOOST_CHECK_EQUAL(list.GetTail()->key, 20);
    BOOST_CHECK_EQUAL(list.GetTail()->prev->key, 10);

    // Добавление в пустой список через BACK
    DoublyList<int> list2;
    list2.LPUSH_BACK(100);
    BOOST_CHECK_EQUAL(list2.GetHead()->key, 100);
    BOOST_CHECK_EQUAL(list2.GetTail()->key, 100);
}

// Тесты удаления с концов (HEAD/BACK)
BOOST_AUTO_TEST_CASE(DeleteHeadAndBack) {
    DoublyList<int> list;

    // Удаление из пустого списка
    BOOST_CHECK_THROW(list.LDEL_HEAD(), underflow_error);
    BOOST_CHECK_THROW(list.LDEL_BACK(), underflow_error);

    list.LPUSH_BACK(1);
    list.LPUSH_BACK(2);
    list.LPUSH_BACK(3); // [1, 2, 3]

    // Удаление головы
    list.LDEL_HEAD(); // [2, 3]
    BOOST_CHECK_EQUAL(list.GetHead()->key, 2);
    BOOST_CHECK(list.GetHead()->prev == nullptr);

    // Удаление хвоста
    list.LDEL_BACK(); // [2]
    BOOST_CHECK_EQUAL(list.GetTail()->key, 2);
    BOOST_CHECK(list.GetTail()->next == nullptr);

    // Удаление последнего оставшегося элемента через HEAD
    list.LDEL_HEAD(); // []
    BOOST_CHECK(list.GetHead() == nullptr);
    BOOST_CHECK(list.GetTail() == nullptr);

    // Снова наполняем для теста удаления последнего через BACK
    list.LPUSH_BACK(100);
    list.LDEL_BACK();
    BOOST_CHECK(list.GetHead() == nullptr);
}

// Тесты поиска и вставки рядом (BEFORE/AFTER)
BOOST_AUTO_TEST_CASE(InsertRelative) {
    DoublyList<int> list;
    list.LPUSH_BACK(10);
    list.LPUSH_BACK(30); // [10, 30]

    // LPUSH_BEFORE
    BOOST_CHECK_THROW(list.LPUSH_BEFORE(99, 5), invalid_argument); // Не найден
    
    // Вставка перед головой (особый случай)
    list.LPUSH_BEFORE(10, 5); // [5, 10, 30]
    BOOST_CHECK_EQUAL(list.GetHead()->key, 5);

    // Вставка в середину
    list.LPUSH_BEFORE(30, 20); // [5, 10, 20, 30]
    auto node20 = list.LGET_BY_VALUE(20);
    BOOST_CHECK(node20 != nullptr);
    BOOST_CHECK_EQUAL(node20->prev->key, 10);
    BOOST_CHECK_EQUAL(node20->next->key, 30);

    // LPUSH_AFTER
    BOOST_CHECK_THROW(list.LPUSH_AFTER(99, 5), invalid_argument); // Не найден

    // Вставка после хвоста (особый случай)
    list.LPUSH_AFTER(30, 40); // [5, 10, 20, 30, 40]
    BOOST_CHECK_EQUAL(list.GetTail()->key, 40);

    // Вставка в середину
    list.LPUSH_AFTER(10, 15); // [5, 10, 15, 20, 30, 40]
    auto node15 = list.LGET_BY_VALUE(15);
    BOOST_CHECK_EQUAL(node15->prev->key, 10);
    BOOST_CHECK_EQUAL(node15->next->key, 20);
}

// Тесты удаления рядом (BEFORE/AFTER) и по значению
BOOST_AUTO_TEST_CASE(DeleteRelativeAndByValue) {
    DoublyList<int> list;
    list.LPUSH_BACK(1);
    list.LPUSH_BACK(2);
    list.LPUSH_BACK(3);
    list.LPUSH_BACK(4);
    list.LPUSH_BACK(5); // [1, 2, 3, 4, 5]

    // LDEL_AFTER
    BOOST_CHECK_THROW(list.LDEL_AFTER(99), invalid_argument); // Не найден
    BOOST_CHECK_THROW(list.LDEL_AFTER(5), logic_error);       // После хвоста нечего удалять

    // Удаление после элемента (середина)
    list.LDEL_AFTER(2); // Удаляем 3 -> [1, 2, 4, 5]
    BOOST_CHECK(list.LGET_BY_VALUE(3) == nullptr);
    BOOST_CHECK_EQUAL(list.LGET_BY_VALUE(2)->next->key, 4);

    // Удаление после элемента 
    list.LDEL_AFTER(4); // Удаляем 5 -> [1, 2, 4]
    BOOST_CHECK_EQUAL(list.GetTail()->key, 4);

    // LDEL_BEFORE
    BOOST_CHECK_THROW(list.LDEL_BEFORE(99), invalid_argument); // Не найден
    BOOST_CHECK_THROW(list.LDEL_BEFORE(1), logic_error);       // Перед головой нечего удалять

    // Удаление перед элементом (середина)
    list.LDEL_BEFORE(4); // Удаляем 2 -> [1, 4]
    BOOST_CHECK(list.LGET_BY_VALUE(2) == nullptr);
    BOOST_CHECK_EQUAL(list.GetHead()->next->key, 4);

    // Удаление перед элементом (это голова)
    list.LDEL_BEFORE(4); // Удаляем 1 -> [4]
    BOOST_CHECK_EQUAL(list.GetHead()->key, 4);

    // LDEL_BY_VALUE
    list.LPUSH_BACK(5);
    list.LPUSH_BACK(6); // [4, 5, 6]

    BOOST_CHECK_THROW(list.LDEL_BY_VALUE(99), invalid_argument);

    list.LDEL_BY_VALUE(4); // Удаление головы -> [5, 6]
    BOOST_CHECK_EQUAL(list.GetHead()->key, 5);

    list.LDEL_BY_VALUE(6); // Удаление хвоста -> [5]
    BOOST_CHECK_EQUAL(list.GetTail()->key, 5);

    list.LPUSH_BACK(7);
    list.LPUSH_BACK(8); // [5, 7, 8]
    list.LDEL_BY_VALUE(7); // Удаление из середины -> [5, 8]
    BOOST_CHECK_EQUAL(list.GetHead()->next->key, 8);
    BOOST_CHECK_EQUAL(list.GetTail()->prev->key, 5);
}

// Тест копирования и присваивания 
BOOST_AUTO_TEST_CASE(CopyAndAssign) {
    DoublyList<int> list1;
    list1.LPUSH_BACK(1);
    list1.LPUSH_BACK(2);

    // Конструктор копирования
    DoublyList<int> list2(list1);
    BOOST_CHECK_EQUAL(list2.GetHead()->key, 1);
    BOOST_CHECK_EQUAL(list2.GetTail()->key, 2);

    // Проверка глубокой копии
    list1.LDEL_HEAD();
    BOOST_CHECK_EQUAL(list1.GetHead()->key, 2);
    BOOST_CHECK_EQUAL(list2.GetHead()->key, 1); // list2 остался прежним

    // Оператор присваивания
    DoublyList<int> list3;
    list3 = list2;
    BOOST_CHECK_EQUAL(list3.GetHead()->key, 1);
    BOOST_CHECK_EQUAL(list3.GetTail()->key, 2);
    
    // Самоприсваивание
    list3 = list3;
    BOOST_CHECK_EQUAL(list3.GetHead()->key, 1);

    // Копирование пустого списка
    DoublyList<int> empty1;
    DoublyList<int> empty2(empty1);
    BOOST_CHECK(empty2.GetHead() == nullptr);
}

// Тест вывода
BOOST_AUTO_TEST_CASE(PrintFunction) {
    DoublyList<int> list;
    {
        CoutRedirect capture;
        // Пустой список
        list.PRINT(1);
        BOOST_CHECK(capture.getString().find("Список пуст") != string::npos);

        list.PRINT(2);
        BOOST_CHECK(capture.getString().find("Список пуст") != string::npos);

        // Заполненный список
        list.LPUSH_BACK(10);
        list.LPUSH_BACK(20);

        // Прямой вывод
        list.PRINT(1);
        string out1 = capture.getString();
        BOOST_CHECK(out1.find("Голова -> 10 <-> 20 <-> nullptr") != string::npos);

        // Обратный вывод
        list.PRINT(2);
        string out2 = capture.getString();
        BOOST_CHECK(out2.find("Хвост -> 20 <-> 10 <-> nullptr") != string::npos);
    }

    // Неверный выбор
    BOOST_CHECK_THROW(list.PRINT(3), invalid_argument);
}

// Тест файлового ввода/вывода (Текстовый)
BOOST_AUTO_TEST_CASE(FileIO_Text) {
    DoublyList<int> list;
    list.LPUSH_BACK(100);
    list.LPUSH_BACK(200);
    
    string filename = "test_list.txt";

    // Тест сохранения
    CoutRedirect capture; // Чтобы не мусорить в консоль
    list.LSAVE(filename);
    
    // Тест загрузки
    DoublyList<int> list2;
    list2.LLOAD(filename); // Это должно очистить текущий список и загрузить новый
    
    BOOST_CHECK_EQUAL(list2.GetHead()->key, 100);
    BOOST_CHECK_EQUAL(list2.GetTail()->key, 200);

    // Удаление файла
    remove(filename.c_str());
    
    // Тест ошибок открытия файлов
    BOOST_CHECK_THROW(list.LLOAD("non_existent_file.txt"), runtime_error);
}

// Тест Файлового ввода/вывода (Бинарный)
BOOST_AUTO_TEST_CASE(FileIO_Binary) {
    DoublyList<double> list;
    list.LPUSH_BACK(1.1);
    list.LPUSH_BACK(2.2);
    list.LPUSH_BACK(3.3);

    string filename = "test_list.bin";
    CoutRedirect capture;

    list.LSAVE_BIN(filename);

    DoublyList<double> list2;
    // Добавим мусор, чтобы проверить, что LLOAD_BIN очищает список
    list2.LPUSH_BACK(999.9); 
    
    list2.LLOAD_BIN(filename);

    BOOST_CHECK_CLOSE(list2.GetHead()->key, 1.1, 0.001);
    BOOST_CHECK_CLOSE(list2.GetTail()->key, 3.3, 0.001);
    BOOST_CHECK_EQUAL(list2.GetHead()->next->key, 2.2);

    remove(filename.c_str());
    
    BOOST_CHECK_THROW(list.LLOAD_BIN("non_existent.bin"), runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()