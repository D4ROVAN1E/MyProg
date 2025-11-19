#define BOOST_TEST_MODULE DoubleHashTests
#include <boost/test/included/unit_test.hpp>
#include <string>
#include <cstdio> // для remove() файла
#include "dh.hpp"

// Вспомогательная функция для очистки тестовых файлов
void clean_file(const std::string& filename) {
    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_SUITE(DoubleHashSuite)

// --- Тесты Конструкторов и Базовых свойств ---

BOOST_AUTO_TEST_CASE(ConstructorTest) {
    // Проверка инициализации по умолчанию
    DoubleHash<int> dh(5);
    BOOST_CHECK(dh.empty());
    BOOST_CHECK_EQUAL(dh.size(), 0);

    // Проверка исключения при создании таблицы нулевого размера
    BOOST_CHECK_THROW(DoubleHash<int> zero_dh(0), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(InsertAndFindTest) {
    DoubleHash<int> dh;
    
    // Вставка новых элементов
    dh.insert("key1", 100);
    dh.insert("key2", 200);

    BOOST_CHECK_EQUAL(dh.size(), 2);
    BOOST_CHECK(!dh.empty());

    // Поиск существующих элементов
    int* val1 = dh.find("key1");
    BOOST_CHECK(val1 != nullptr);
    BOOST_CHECK_EQUAL(*val1, 100);

    int* val2 = dh.find("key2");
    BOOST_CHECK(val2 != nullptr);
    BOOST_CHECK_EQUAL(*val2, 200);

    // Поиск несуществующего элемента
    BOOST_CHECK(dh.find("missing_key") == nullptr);
}

BOOST_AUTO_TEST_CASE(UpdateValueTest) {
    DoubleHash<std::string> dh;
    dh.insert("user", "Alice");
    
    BOOST_CHECK_EQUAL(*dh.find("user"), "Alice");

    // Вставка того же ключа должна обновить значение, а не добавить новый узел
    dh.insert("user", "Bob");
    
    BOOST_CHECK_EQUAL(dh.size(), 1);
    BOOST_CHECK_EQUAL(*dh.find("user"), "Bob");
}

BOOST_AUTO_TEST_CASE(RemoveTest) {
    DoubleHash<int> dh;
    dh.insert("A", 1);
    dh.insert("B", 2);

    // Удаление существующего элемента
    BOOST_CHECK(dh.remove("A"));
    BOOST_CHECK_EQUAL(dh.size(), 1);
    BOOST_CHECK(dh.find("A") == nullptr);

    // Попытка удаления несуществующего элемента
    BOOST_CHECK(!dh.remove("Z"));
    BOOST_CHECK_EQUAL(dh.size(), 1);

    // Удаление последнего элемента
    BOOST_CHECK(dh.remove("B"));
    BOOST_CHECK(dh.empty());

    // Удаление из пустой таблицы
    BOOST_CHECK(!dh.remove("B"));
}

BOOST_AUTO_TEST_CASE(ClearTest) {
    DoubleHash<double> dh;
    dh.insert("pi", 3.14);
    dh.insert("e", 2.71);

    dh.clear();
    BOOST_CHECK(dh.empty());
    BOOST_CHECK_EQUAL(dh.size(), 0);
    BOOST_CHECK(dh.find("pi") == nullptr);
}

// --- Тест копирования и присваивания ---

BOOST_AUTO_TEST_CASE(CopyAndAssignTest) {
    DoubleHash<int> original(5);
    original.insert("one", 1);
    original.insert("two", 2);

    // Тест конструктора копирования
    DoubleHash<int> copy_dh(original);
    BOOST_CHECK_EQUAL(copy_dh.size(), original.size());
    BOOST_CHECK_EQUAL(*copy_dh.find("one"), 1);

    // Изменение копии не должно влиять на оригинал
    copy_dh.insert("three", 3);
    BOOST_CHECK(original.find("three") == nullptr);
    BOOST_CHECK(copy_dh.find("three") != nullptr);

    // Тест оператора присваивания
    DoubleHash<int> assigned_dh;
    assigned_dh = original;
    BOOST_CHECK_EQUAL(assigned_dh.size(), original.size());
    
    // Тест самоприсваивания
    assigned_dh = assigned_dh;
    BOOST_CHECK_EQUAL(assigned_dh.size(), original.size());
}

// --- Тест расширения (Resize) ---

BOOST_AUTO_TEST_CASE(ResizeTest) {
    // Создаем маленькую таблицу
    DoubleHash<int> dh(3); 
    
    // Вставляем элементы, чтобы превысить load factor 0.7
    // 3 * 0.7 = 2.1 -> при вставке 3-го элемента должно произойти расширение
    dh.insert("1", 1);
    dh.insert("2", 2);
    
    // Пока размер должен быть исходным (или близким, зависит от логики resize)
    // Проверяем, что вставка 3-го элемента вызывает resize и не ломает данные
    dh.insert("3", 3);
    dh.insert("4", 4); 

    BOOST_CHECK_EQUAL(dh.size(), 4);
    
    // Проверяем доступность всех элементов после перехеширования
    BOOST_CHECK_EQUAL(*dh.find("1"), 1);
    BOOST_CHECK_EQUAL(*dh.find("2"), 2);
    BOOST_CHECK_EQUAL(*dh.find("3"), 3);
    BOOST_CHECK_EQUAL(*dh.find("4"), 4);
}

// --- Тесты Сериализации (Текст) ---

BOOST_AUTO_TEST_CASE(TextSerializationTest) {
    std::string filename = "test_hash_dump.txt";
    
    {
        DoubleHash<int> dh;
        dh.insert("apple", 5);
        dh.insert("banana", 10);
        dh.serialize_text(filename);
    }

    {
        DoubleHash<int> dh_loaded;
        dh_loaded.deserialize_text(filename);

        BOOST_CHECK_EQUAL(dh_loaded.size(), 2);
        BOOST_CHECK_EQUAL(*dh_loaded.find("apple"), 5);
        BOOST_CHECK_EQUAL(*dh_loaded.find("banana"), 10);
    }

    // Тест на чтение несуществующего файла
    DoubleHash<int> bad_dh;
    BOOST_CHECK_THROW(bad_dh.deserialize_text("non_existent_file.txt"), std::runtime_error);

    clean_file(filename);
}

// --- Тесты Сериализации (Бинарной) ---

BOOST_AUTO_TEST_CASE(BinarySerializationTest) {
    std::string filename = "test_hash_dump.bin";
    
    // Важно: Бинарная сериализация в текущей реализации класса работает корректно 
    // только для POD-типов (int, double), так как использует sizeof(T).
    // std::string как T сломает логику чтения, поэтому тестируем на double.
    
    {
        DoubleHash<double> dh;
        dh.insert("x", 1.1);
        dh.insert("y", 2.2);
        dh.insert("z", 3.3);
        
        // Создаем "дырку" (removed элемент), чтобы проверить логику сохранения только occupied
        dh.remove("y"); 
        
        dh.serialize_bin(filename);
    }

    {
        DoubleHash<double> dh_loaded;
        dh_loaded.deserialize_bin(filename);

        BOOST_CHECK_EQUAL(dh_loaded.size(), 2); // x и z
        BOOST_CHECK_CLOSE(*dh_loaded.find("x"), 1.1, 0.001);
        BOOST_CHECK_CLOSE(*dh_loaded.find("z"), 3.3, 0.001);
        BOOST_CHECK(dh_loaded.find("y") == nullptr);
    }
    
    // Тест на чтение несуществующего бинарного файла
    DoubleHash<double> bad_dh;
    BOOST_CHECK_THROW(bad_dh.deserialize_bin("non_existent_bin.bin"), std::runtime_error);

    clean_file(filename);
}

// --- Тест исключений ввода-вывода ---

BOOST_AUTO_TEST_CASE(FileIOErrorTest) {
    DoubleHash<int> dh;
    dh.insert("a", 1);
    
    // Попытка записи в некорректный путь (например, директорию как файл или запрещенный путь)
    // Примечание: В Windows/Linux поведение может отличаться, используем заведомо "плохое" имя для теста.
    // Пустая строка часто вызывает ошибку открытия fstream.
    BOOST_CHECK_THROW(dh.serialize_text(""), std::runtime_error);
    
    // Создадим битый файл
    std::ofstream badFile("corrupted.txt");
    badFile << "NotANumber " << "NotANumber"; // Заголовок испорчен
    badFile.close();
    
    DoubleHash<int> loader;
    BOOST_CHECK_THROW(loader.deserialize_text("corrupted.txt"), std::runtime_error);
    
    clean_file("corrupted.txt");
}

// --- Тест функции Print ---
// Print выводит в cout, тестировать его сложно без перехвата потока.
// Обычно достаточно проверить, что он просто не падает.
BOOST_AUTO_TEST_CASE(PrintSmokeTest) {
    DoubleHash<int> dh;
    dh.insert("test", 1);
    dh.print();
}

BOOST_AUTO_TEST_SUITE_END()