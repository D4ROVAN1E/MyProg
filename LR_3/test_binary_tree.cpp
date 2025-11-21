#define BOOST_TEST_MODULE BinaryTreeTests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <iostream>
#include <fstream>
#include <cstdio>

#include "binary_tree.hpp"

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

BOOST_AUTO_TEST_SUITE(TreeTestSuite)

// Тест конструктора и вставки (TINSERT)
BOOST_AUTO_TEST_CASE(ConstructorAndInsert) {
    FullBinaryTree<int> tree;
    BOOST_CHECK(tree.GetRoot() == nullptr);

    tree.TINSERT(10);
    BOOST_CHECK(tree.GetRoot() != nullptr);
    BOOST_CHECK_EQUAL(tree.GetRoot()->key, 10);

    tree.TINSERT(5);
    tree.TINSERT(15);

    // Проверяем структуру 
    BOOST_CHECK_EQUAL(tree.GetRoot()->left->key, 5);
    BOOST_CHECK_EQUAL(tree.GetRoot()->right->key, 15);
}

// Тест проверки на полноту дерева (TFULL)
BOOST_AUTO_TEST_CASE(IsFullTree) {
    FullBinaryTree<int> tree;
    
    // Пустое дерево считается полным
    BOOST_CHECK(tree.TFULL());

    tree.TINSERT(10);
    // Лист является полным
    BOOST_CHECK(tree.TFULL());

    tree.TINSERT(5);
    // Есть левый, нет правого -> не полное
    BOOST_CHECK(tree.TFULL() == false);

    tree.TINSERT(15);
    // Есть оба ребенка -> полное
    BOOST_CHECK(tree.TFULL());

    // Добавляем внука слева
    tree.TINSERT(2);
    // У узла 5 теперь один ребенок -> не полное
    BOOST_CHECK(tree.TFULL() == false);
}

// Тест Копирование и Присваивание
BOOST_AUTO_TEST_CASE(CopyAndAssign) {
    FullBinaryTree<int> tree1;
    tree1.TINSERT(10);
    tree1.TINSERT(5);
    tree1.TINSERT(15);

    // Тест копирующего конструктора
    FullBinaryTree<int> tree2(tree1);
    
    // Проверяем, что данные скопировались
    BOOST_CHECK(tree2.GetRoot() != nullptr);
    BOOST_CHECK_EQUAL(tree2.GetRoot()->key, 10);

    // Проверяем глубокое копирование
    BOOST_CHECK_NE(tree1.GetRoot(), tree2.GetRoot());

    // Модификация копии не должна менять оригинал
    tree2.TINSERT(20);
    
    // Проверка через TFULL
    BOOST_CHECK(tree1.TFULL() == true); 
    BOOST_CHECK(tree2.TFULL() == false);

    // Тест оператора присваивания
    FullBinaryTree<int> tree3;
    tree3 = tree1;
    BOOST_CHECK_EQUAL(tree3.GetRoot()->key, 10);
    BOOST_CHECK_NE(tree3.GetRoot(), tree1.GetRoot());

    // Тест самоприсваивания
    tree3 = tree3;
    BOOST_CHECK_EQUAL(tree3.GetRoot()->key, 10);
}

// Тест методов вывода (PRINT)
BOOST_AUTO_TEST_CASE(PrintMethods) {
    FullBinaryTree<int> tree;
    tree.TINSERT(2);
    tree.TINSERT(1);
    tree.TINSERT(3);

    {
        CoutRedirect capture;
        tree.PRINT(1); // Breadth-first: 2 1 3
        string output = capture.getString();
        BOOST_CHECK(output.find("2 1 3") != string::npos);
    }

    {
        CoutRedirect capture;
        tree.PRINT(2); // Pre-order: 2 1 3
        string output = capture.getString();
        BOOST_CHECK(output.find("2 1 3") != string::npos);
    }

    {
        CoutRedirect capture;
        tree.PRINT(3); // In-order: 1 2 3
        string output = capture.getString();
        BOOST_CHECK(output.find("1 2 3") != string::npos);
    }

    {
        CoutRedirect capture;
        tree.PRINT(4); // Post-order: 1 3 2
        string output = capture.getString();
        BOOST_CHECK(output.find("1 3 2") != string::npos);
    }

    {
        CoutRedirect capture;
        tree.PRINT(5); // Visual
        string output = capture.getString();
        // Проверяем наличие элементов визуализации
        BOOST_CHECK(output.find("|___") != string::npos || output.find("|---") != string::npos);
    }
    
    // Тест пустого дерева (визуальный вывод)
    FullBinaryTree<int> emptyTree;
    {
        CoutRedirect capture;
        emptyTree.PRINT(5);
        BOOST_CHECK(capture.getString().find("Дерево пустое") != string::npos);
    }

    // Тест исключения при неверном коде
    BOOST_CHECK_THROW(tree.PRINT(99), invalid_argument);
}

// Тест сохранения и загрузки (Текстовый режим)
BOOST_AUTO_TEST_CASE(FileIO_Text) {
    FullBinaryTree<int> tree;
    tree.TINSERT(10);
    tree.TINSERT(5);
    tree.TINSERT(15);

    string filename = "test_tree.txt";

    // Сохранение
    {
        CoutRedirect capture; // Глушим вывод в консоль 
        tree.TSAVE(filename);
    }

    // Загрузка в новое дерево
    FullBinaryTree<int> loadedTree;
    {
        CoutRedirect capture;
        loadedTree.TLOAD(filename);
    }

    // Проверка корректности загрузки (in-order обход должен дать 5 10 15)
    CoutRedirect capture;
    loadedTree.PRINT(3);
    BOOST_CHECK(capture.getString().find("5 10 15") != string::npos);

    // Очистка файла
    remove(filename.c_str());
}

// Тест сохранения и загрузки (Бинарный режим)
BOOST_AUTO_TEST_CASE(FileIO_Binary) {
    FullBinaryTree<int> tree;
    tree.TINSERT(10);
    tree.TINSERT(5);
    tree.TINSERT(15);
    tree.TINSERT(3); // Усложним структуру

    string filename = "test_tree.bin";

    // Сохранение
    {
        CoutRedirect capture;
        tree.TSAVE_BINARY(filename);
    }

    // Загрузка
    FullBinaryTree<int> loadedTree;
    {
        CoutRedirect capture;
        loadedTree.TLOAD_BINARY(filename);
    }

    // Проверяем структуру
    BOOST_CHECK(loadedTree.GetRoot()->key == 10);
    BOOST_CHECK(loadedTree.GetRoot()->left->key == 5);
    BOOST_CHECK(loadedTree.GetRoot()->left->left->key == 3); // Глубокий узел

    remove(filename.c_str());
}

// 7. Тест обработки ошибок ввода-вывода
BOOST_AUTO_TEST_CASE(FileIO_Errors) {
    FullBinaryTree<int> tree;
    
    // Попытка загрузить несуществующий файл
    BOOST_CHECK_THROW(tree.TLOAD("non_existent_file.txt"), runtime_error);
    BOOST_CHECK_THROW(tree.TLOAD_BINARY("non_existent_file.bin"), runtime_error);
    
    // Попытка сохранить в некорректный путь
    BOOST_CHECK_THROW(tree.TSAVE("/root/forbidden.txt"), runtime_error);
    
    // Тест загрузки бинарного файла с "битыми" данными
    {
        ofstream badFile("broken.bin");
        badFile << "Not binary data";
        badFile.close();
        
        // TLOAD_BINARY ожидает bool маркеры, текст вызовет сбой или рассинхрон
        BOOST_CHECK_THROW(tree.TLOAD_BINARY("broken.bin"), runtime_error);
        remove("broken.bin");
    }
}

// Тест работы с другим типом данных (double)
BOOST_AUTO_TEST_CASE(TemplateDouble) {
    FullBinaryTree<double> tree;
    tree.TINSERT(10.5);
    tree.TINSERT(5.5);
    
    BOOST_CHECK_CLOSE(tree.GetRoot()->key, 10.5, 0.001);
    BOOST_CHECK_CLOSE(tree.GetRoot()->left->key, 5.5, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()