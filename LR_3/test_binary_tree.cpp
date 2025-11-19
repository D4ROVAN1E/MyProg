#define BOOST_TEST_MODULE BinaryTreeTests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <iostream>
#include <fstream>
#include <cstdio>

// Подключаем тестируемый заголовок
#include "binary_tree.hpp"

using namespace std;

// Вспомогательная структура для перехвата std::cout
struct CoutRedirect {
    CoutRedirect() {
        old = std::cout.rdbuf(buffer.rdbuf());
    }
    ~CoutRedirect() {
        std::cout.rdbuf(old);
    }
    std::string getString() {
        return buffer.str();
    }
    std::stringstream buffer;
    std::streambuf* old;
};

BOOST_AUTO_TEST_SUITE(TreeTestSuite)

// 1. Тест конструктора и вставки (TINSERT)
BOOST_AUTO_TEST_CASE(ConstructorAndInsert) {
    FullBinaryTree<int> tree;
    BOOST_CHECK(tree.GetRoot() == nullptr);

    tree.TINSERT(10);
    BOOST_CHECK(tree.GetRoot() != nullptr);
    BOOST_CHECK_EQUAL(tree.GetRoot()->key, 10);

    tree.TINSERT(5);
    tree.TINSERT(15);

    // Проверяем структуру (BST свойство)
    BOOST_CHECK_EQUAL(tree.GetRoot()->left->key, 5);
    BOOST_CHECK_EQUAL(tree.GetRoot()->right->key, 15);
}

// 2. Тест проверки на полноту дерева (TFULL)
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

// 3. Тест Rule of 5: Копирование и Присваивание
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

    // Проверяем глубокое копирование (адреса должны отличаться)
    BOOST_CHECK_NE(tree1.GetRoot(), tree2.GetRoot());

    // Модификация копии не должна менять оригинал
    tree2.TINSERT(20);
    
    // Проверка через TFULL (tree1 полное, tree2 теперь нет, так как у 15 появился ребенок 20)
    // 10 -> L:5, R:15 -> R:20. Узел 15 имеет правого, но не имеет левого.
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

// 4. Тест методов вывода (PRINT)
BOOST_AUTO_TEST_CASE(PrintMethods) {
    FullBinaryTree<int> tree;
    tree.TINSERT(2);
    tree.TINSERT(1);
    tree.TINSERT(3);

    {
        CoutRedirect capture;
        tree.PRINT(1); // Breadth-first: 2 1 3
        std::string output = capture.getString();
        BOOST_CHECK(output.find("2 1 3") != std::string::npos);
    }

    {
        CoutRedirect capture;
        tree.PRINT(2); // Pre-order: 2 1 3
        std::string output = capture.getString();
        BOOST_CHECK(output.find("2 1 3") != std::string::npos);
    }

    {
        CoutRedirect capture;
        tree.PRINT(3); // In-order: 1 2 3
        std::string output = capture.getString();
        BOOST_CHECK(output.find("1 2 3") != std::string::npos);
    }

    {
        CoutRedirect capture;
        tree.PRINT(4); // Post-order: 1 3 2
        std::string output = capture.getString();
        BOOST_CHECK(output.find("1 3 2") != std::string::npos);
    }

    {
        CoutRedirect capture;
        tree.PRINT(5); // Visual
        std::string output = capture.getString();
        // Проверяем наличие элементов визуализации
        BOOST_CHECK(output.find("|___") != std::string::npos || output.find("|---") != std::string::npos);
    }
    
    // Тест пустого дерева (визуальный вывод)
    FullBinaryTree<int> emptyTree;
    {
        CoutRedirect capture;
        emptyTree.PRINT(5);
        BOOST_CHECK(capture.getString().find("Дерево пустое") != std::string::npos);
    }

    // Тест исключения при неверном коде
    BOOST_CHECK_THROW(tree.PRINT(99), std::invalid_argument);
}

// 5. Тест сохранения и загрузки (Текстовый режим)
BOOST_AUTO_TEST_CASE(FileIO_Text) {
    FullBinaryTree<int> tree;
    tree.TINSERT(10);
    tree.TINSERT(5);
    tree.TINSERT(15);

    std::string filename = "test_tree.txt";

    // Сохранение
    {
        CoutRedirect capture; // Глушим вывод в консоль ("Saved to...")
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
    BOOST_CHECK(capture.getString().find("5 10 15") != std::string::npos);

    // Очистка файла
    std::remove(filename.c_str());
}

// 6. Тест сохранения и загрузки (Бинарный режим)
BOOST_AUTO_TEST_CASE(FileIO_Binary) {
    FullBinaryTree<int> tree;
    tree.TINSERT(10);
    tree.TINSERT(5);
    tree.TINSERT(15);
    tree.TINSERT(3); // Усложним структуру

    std::string filename = "test_tree.bin";

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

    std::remove(filename.c_str());
}

// 7. Тест обработки ошибок ввода-вывода
BOOST_AUTO_TEST_CASE(FileIO_Errors) {
    FullBinaryTree<int> tree;
    
    // Попытка загрузить несуществующий файл
    BOOST_CHECK_THROW(tree.TLOAD("non_existent_file.txt"), std::runtime_error);
    BOOST_CHECK_THROW(tree.TLOAD_BINARY("non_existent_file.bin"), std::runtime_error);
    
    // Попытка сохранить в некорректный путь (например, директорию)
    // Примечание: этот тест зависит от ОС, оставим базовую проверку
    #ifdef __linux__
    BOOST_CHECK_THROW(tree.TSAVE("/root/forbidden.txt"), std::runtime_error);
    #endif
    
    // Тест загрузки бинарного файла с "битыми" данными
    {
        std::ofstream badFile("broken.bin");
        badFile << "Not binary data";
        badFile.close();
        
        // TLOAD_BINARY ожидает bool маркеры, текст вызовет сбой или рассинхрон
        // В текущей реализации это может быть runtime_error или некорректное чтение
        // Мы проверяем, что программа не падает с segfault, а кидает исключение
        BOOST_CHECK_THROW(tree.TLOAD_BINARY("broken.bin"), std::runtime_error);
        std::remove("broken.bin");
    }
}

// 8. Тест работы с другим типом данных (double)
BOOST_AUTO_TEST_CASE(TemplateDouble) {
    FullBinaryTree<double> tree;
    tree.TINSERT(10.5);
    tree.TINSERT(5.5);
    
    BOOST_CHECK_CLOSE(tree.GetRoot()->key, 10.5, 0.001);
    BOOST_CHECK_CLOSE(tree.GetRoot()->left->key, 5.5, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()