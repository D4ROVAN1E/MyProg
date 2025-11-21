#define BOOST_TEST_MODULE StackTests
#include <boost/test/included/unit_test.hpp>

#include "stack.hpp"
#include <cstdio>   
#include <iostream>
#include <string>

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

BOOST_AUTO_TEST_SUITE(StackTestSuite)

// Тестирование конструкторов и базовых операций
BOOST_AUTO_TEST_CASE(DefaultConstructorAndPushPop) {
    Stack<int> s;
    BOOST_CHECK_EQUAL(s.GetSize(), 0);

    s.SPUSH(10);
    s.SPUSH(20);

    BOOST_CHECK_EQUAL(s.GetSize(), 2);
    BOOST_CHECK_EQUAL(s.SPOP(), 20);
    BOOST_CHECK_EQUAL(s.GetSize(), 1);
    BOOST_CHECK_EQUAL(s.SPOP(), 10);
    BOOST_CHECK_EQUAL(s.GetSize(), 0);
}

// Тестирование конструктора с параметром и валидация исключений
BOOST_AUTO_TEST_CASE(ExplicitConstructorAndExceptions) {
    // Емкость > 0
    Stack<int> s(10);
    BOOST_CHECK_EQUAL(s.GetSize(), 0);

    // Емкость == 0 
    BOOST_CHECK_THROW(Stack<int> s_invalid(0), invalid_argument);

    // SPOP из пустого стека
    BOOST_CHECK_THROW(s.SPOP(), out_of_range);
}

// Тестирование автоматического расширения (doubleStack)
BOOST_AUTO_TEST_CASE(AutoResize) {
    // Создаем стек с маленькой емкостью
    Stack<int> s(2);
    
    s.SPUSH(1);
    s.SPUSH(2);

    s.SPUSH(3);
    s.SPUSH(4);
    s.SPUSH(5);

    BOOST_CHECK_EQUAL(s.GetSize(), 5);
    BOOST_CHECK_EQUAL(s.SPOP(), 5);
    BOOST_CHECK_EQUAL(s.SPOP(), 4);
}

// Тестирование конструктора копирования
BOOST_AUTO_TEST_CASE(CopyConstructor) {
    Stack<int> original;
    original.SPUSH(100);
    original.SPUSH(200);

    Stack<int> copy(original); // Вызов конструктора копирования

    // Проверяем, что данные скопировались
    BOOST_CHECK_EQUAL(copy.GetSize(), 2);
    BOOST_CHECK_EQUAL(copy.SPOP(), 200);

    // Проверяем глубокое копирование
    BOOST_CHECK_EQUAL(original.GetSize(), 2); 
    BOOST_CHECK_EQUAL(original.SPOP(), 200);
}

// Тестирование оператора присваивания
BOOST_AUTO_TEST_CASE(AssignmentOperator) {
    Stack<int> s1;
    s1.SPUSH(1);
    s1.SPUSH(2);

    Stack<int> s2;
    s2.SPUSH(99); // Старые данные должны удалиться

    s2 = s1; // Присваивание

    BOOST_CHECK_EQUAL(s2.GetSize(), 2);
    BOOST_CHECK_EQUAL(s2.SPOP(), 2);

    // Тест на самоприсваивание 
    s1 = s1;
    BOOST_CHECK_EQUAL(s1.GetSize(), 2);
    BOOST_CHECK_EQUAL(s1.SPOP(), 2);
}

// Тестирование текстового сохранения и загрузки (SSAVE / SLOAD)
BOOST_AUTO_TEST_CASE(FileIO_Text) {
    const string filename = "test_stack.txt";
    
    {
        Stack<int> sOut;
        sOut.SPUSH(42);
        sOut.SPUSH(123);
        sOut.SSAVE(filename);
    } // sOut уничтожается

    Stack<int> sIn;
    sIn.SLOAD(filename);

    BOOST_CHECK_EQUAL(sIn.GetSize(), 2);
    BOOST_CHECK_EQUAL(sIn.SPOP(), 123);
    BOOST_CHECK_EQUAL(sIn.SPOP(), 42);

    // Тест на загрузку несуществующего файла
    BOOST_CHECK_THROW(sIn.SLOAD("non_existent_file.txt"), runtime_error);

    remove(filename.c_str());
}

// Тестирование бинарного сохранения и загрузки (SSAVE_BINARY / SLOAD_BINARY)
BOOST_AUTO_TEST_CASE(FileIO_Binary) {
    const string filename = "test_stack.bin";
    
    {
        Stack<double> sOut;
        sOut.SPUSH(3.14);
        sOut.SPUSH(2.71);
        sOut.SSAVE_BINARY(filename);
    }

    Stack<double> sIn;
    sIn.SLOAD_BINARY(filename);

    BOOST_CHECK_EQUAL(sIn.GetSize(), 2);
    // Используем BOOST_CHECK_CLOSE для float/double
    BOOST_CHECK_CLOSE(sIn.SPOP(), 2.71, 0.001);
    BOOST_CHECK_CLOSE(sIn.SPOP(), 3.14, 0.001);

    // Тест на загрузку несуществующего файла
    BOOST_CHECK_THROW(sIn.SLOAD_BINARY("non_existent.bin"), runtime_error);

    remove(filename.c_str());
}

// Тестирование метода PRINT 
BOOST_AUTO_TEST_CASE(PrintMethod) {
    {
        CoutRedirect capture;
    Stack<int> s;
    s.SPUSH(1);
    s.SPUSH(2);
    s.PRINT();
    string output = capture.getString();
    
    // Проверяем наличие чисел в выводе
    BOOST_CHECK(output.find("1 2") != string::npos);
    }
}

// Тестирование работы со сложными типами 
BOOST_AUTO_TEST_CASE(StringStack) {
    Stack<string> s;
    s.SPUSH("Hello");
    s.SPUSH("World");

    BOOST_CHECK_EQUAL(s.GetSize(), 2);
    BOOST_CHECK_EQUAL(s.SPOP(), "World");
    BOOST_CHECK_EQUAL(s.SPOP(), "Hello");
}

BOOST_AUTO_TEST_SUITE_END()