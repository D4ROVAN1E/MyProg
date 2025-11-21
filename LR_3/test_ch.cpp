#define BOOST_TEST_MODULE CuckooHashTests
#include <boost/test/included/unit_test.hpp>

#include "ch.hpp"
#include <string>
#include <vector>
#include <cstdio> 
#include <stdexcept>

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

BOOST_AUTO_TEST_SUITE(CoreFunctionality)

// Базовая инициализация, проверка на пустоту и размер
BOOST_AUTO_TEST_CASE(ConstructionAndEmpty) {
    CuckooHash<int> hash(5);
    BOOST_CHECK(hash.empty());
    BOOST_CHECK_EQUAL(hash.size(), 0);
}

// Вставка, Поиск и Обновление
BOOST_AUTO_TEST_CASE(InsertFindUpdate) {
    CuckooHash<int> hash;

    // Вставка
    hash.insert("key1", 100);
    BOOST_CHECK(!hash.empty());
    BOOST_CHECK_EQUAL(hash.size(), 1);

    // Поиск существующего
    int* val = hash.find("key1");
    BOOST_REQUIRE(val != nullptr);
    BOOST_CHECK_EQUAL(*val, 100);

    // Поиск несуществующего
    BOOST_CHECK(hash.find("key_missing") == nullptr);

    // Обновление значения по существующему ключу
    hash.insert("key1", 200);
    BOOST_CHECK_EQUAL(hash.size(), 1); // Размер не должен меняться
    val = hash.find("key1");
    BOOST_REQUIRE(val != nullptr);
    BOOST_CHECK_EQUAL(*val, 200);
}

// Удаление элементов
BOOST_AUTO_TEST_CASE(Remove) {
    CuckooHash<string> hash;
    hash.insert("alpha", "val1");
    hash.insert("beta", "val2");

    BOOST_CHECK_EQUAL(hash.size(), 2);

    // Удаление существующего
    BOOST_CHECK(hash.remove("alpha"));
    BOOST_CHECK_EQUAL(hash.size(), 1);
    BOOST_CHECK(hash.find("alpha") == nullptr);

    // Удаление несуществующего
    BOOST_CHECK(hash.remove("omega") == false);
    BOOST_CHECK_EQUAL(hash.size(), 1);

    // Очистка
    hash.clear();
    BOOST_CHECK(hash.empty());
    BOOST_CHECK_EQUAL(hash.size(), 0);
}

// Resize и Вытеснение 
BOOST_AUTO_TEST_CASE(HeavyLoadAndResize) {
    // Начинаем с очень маленького размера, чтобы быстро вызвать resize
    CuckooHash<int> hash(2); 
    
    int n = 100;
    for(int i = 0; i < n; ++i) {
        hash.insert("key" + to_string(i), i);
    }

    BOOST_CHECK_EQUAL(hash.size(), n);

    // Проверяем, что все элементы на месте после множественных resize
    for(int i = 0; i < n; ++i) {
        int* val = hash.find("key" + to_string(i));
        BOOST_REQUIRE_MESSAGE(val != nullptr, "Key not found: key" + to_string(i));
        BOOST_CHECK_EQUAL(*val, i);
    }
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(CopySemantics)

// Конструктор копирования
BOOST_AUTO_TEST_CASE(CopyConstructor) {
    CuckooHash<int> original;
    original.insert("a", 1);
    original.insert("b", 2);

    CuckooHash<int> copy(original);

    BOOST_CHECK_EQUAL(copy.size(), 2);
    BOOST_CHECK_EQUAL(*copy.find("a"), 1);

    // Проверка глубокого копирования: меняем копию, оригинал не должен меняться
    copy.insert("a", 999);
    copy.remove("b");

    BOOST_CHECK_EQUAL(*original.find("a"), 1);
    BOOST_CHECK(original.find("b") != nullptr);
}

// Оператор присваивания
BOOST_AUTO_TEST_CASE(AssignmentOperator) {
    CuckooHash<int> h1;
    h1.insert("x", 10);
    
    CuckooHash<int> h2;
    h2.insert("y", 20);
    h2.insert("z", 30);

    h2 = h1; // h2 теперь должна быть как h1

    BOOST_CHECK_EQUAL(h2.size(), 1);
    BOOST_CHECK_EQUAL(*h2.find("x"), 10);
    BOOST_CHECK(h2.find("y") == nullptr);

    // Самоприсваивание (проверка на краш)
    h2 = h2;
    BOOST_CHECK_EQUAL(h2.size(), 1);
    BOOST_CHECK_EQUAL(*h2.find("x"), 10);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(Serialization)

const string TEXT_FILE = "test_db.txt";
const string BIN_FILE = "test_db.bin";

// Текстовая сериализация
BOOST_AUTO_TEST_CASE(TextSerialization) {
    {
        CuckooHash<double> hash;
        hash.insert("pi", 3.14159);
        hash.insert("e", 2.71828);
        hash.serialize_text(TEXT_FILE);
    } // hash уничтожается здесь

    CuckooHash<double> loadedHash;
    loadedHash.deserialize_text(TEXT_FILE);

    BOOST_CHECK_EQUAL(loadedHash.size(), 2);
    BOOST_REQUIRE(loadedHash.find("pi") != nullptr);
    BOOST_CHECK_CLOSE(*loadedHash.find("pi"), 3.14159, 0.0001);
    
    // Очистка
    remove(TEXT_FILE.c_str());
}

// Бинарная сериализация
BOOST_AUTO_TEST_CASE(BinarySerialization) {
    {
        CuckooHash<int> hash;
        // Добавляем достаточно данных, чтобы заполнить таблицу
        for(int i=0; i<10; ++i) hash.insert(to_string(i), i*10);
        hash.serialize_bin(BIN_FILE);
    }

    CuckooHash<int> loadedHash;
    loadedHash.deserialize_bin(BIN_FILE);

    BOOST_CHECK_EQUAL(loadedHash.size(), 10);
    BOOST_CHECK_EQUAL(*loadedHash.find("5"), 50);

    // Очистка
    remove(BIN_FILE.c_str());
}

// Обработка ошибок (несуществующие файлы)
BOOST_AUTO_TEST_CASE(FileErrors) {
    CuckooHash<int> hash;
    
    // Ошибка чтения текста
    BOOST_CHECK_THROW(hash.deserialize_text("non_existent_file.txt"), std::runtime_error);
    
    // Ошибка чтения бинарника
    BOOST_CHECK_THROW(hash.deserialize_bin("non_existent_file.bin"), std::runtime_error);
}

// Обработка ошибок формата (битый файл)
BOOST_AUTO_TEST_CASE(CorruptedFile) {
    // Создаем "битый" файл
    {
        ofstream badFile(TEXT_FILE);
        badFile << "NOT_A_NUMBER " << "TEXT" << endl;
        badFile.close();
    }

    CuckooHash<int> hash;
    BOOST_CHECK_THROW(hash.deserialize_text(TEXT_FILE), std::runtime_error);

    remove(TEXT_FILE.c_str());
}

BOOST_AUTO_TEST_SUITE_END()

// Print 
BOOST_AUTO_TEST_CASE(PrintMethod) {
    CuckooHash<int> hash;
    hash.insert("test1", 1);
    hash.insert("test2", 2);
    hash.insert("test3", 3);
    {
        CoutRedirect capture;
        hash.print();
        string output = capture.getString();
        BOOST_CHECK(output.find("test1 => 1") != string::npos);
        BOOST_CHECK(output.find("test2 => 2") != string::npos);
        BOOST_CHECK(output.find("test2 => 2") != string::npos);
    }
}