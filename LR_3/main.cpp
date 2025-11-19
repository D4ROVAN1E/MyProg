#include <iostream>
#include <string>
#include <stdexcept> // Для try-catch в тесте очереди
#include <cassert>   // Для assert()
#include <fstream>   // Для проверки содержимого файлов
#include <sstream>   // Для проверки содержимого файлов

// Подключаем все ваши заголовочные файлы
#include "stack.hpp"
#include "queue.hpp" 
#include "array.hpp"
#include "singly_list.hpp"
#include "doubly_list.hpp"
#include "binary_tree.hpp"

using namespace std;

// --- Вспомогательная функция для тестов SAVE/LOAD ---

// Читает все содержимое файла в одну строку для удобной проверки
string get_file_content(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        // Если assert сработает здесь, значит, TSAVE/SSAVE и т.д.
        // даже не смогли создать файл
        assert(false && "Не удалось открыть тестовый файл для чтения");
    }
    stringstream ss;
    ss << file.rdbuf();
    file.close();
    return ss.str();
}

// --- Объявления тестовых функций ---
void test_stack();
void test_queue();
void test_array();
void test_singly_list();
void test_doubly_list();
void test_binary_tree();

// --- Главная функция ---
int main() {
    cout << "========================================" << endl;
    cout << "     НАЧАЛО ТЕСТИРОВАНИЯ СТРУКТУР     " << endl;
    cout << "     (c 'assert' проверками)          " << endl;
    cout << "========================================" << endl << endl;

    // Последовательно вызываем тесты для каждой структуры
    test_stack();
    test_queue();
    test_array();
    test_singly_list();
    test_doubly_list();
    test_binary_tree();

    cout << endl;
    cout << "========================================" << endl;
    cout << "      ВСЕ ТЕСТЫ ПРОЙДЕНЫ УСПЕШНО!     " << endl;
    cout << "========================================" << endl;

    return 0;
}

// --- Реализации тестовых функций ---

void test_stack() {
    cout << "--- 1. ТЕСТИРОВАНИЕ СТЕКА (Stack) ---" << endl;
    
    // 1. Конструктор по умолчанию и SPUSH
    Stack<int> s1;
    assert(s1.GetSize() == 0);
    
    s1.SPUSH(10); // capacity = 1, size = 1
    assert(s1.GetSize() == 1);

    s1.SPUSH(20); // doubleStack, capacity = 2, size = 2
    assert(s1.GetSize() == 2);
    
    s1.SPUSH(30); // doubleStack, capacity = 4, size = 3
    assert(s1.GetSize() == 3);

    // 2. SPOP
    assert(s1.SPOP() == 30);
    assert(s1.GetSize() == 2);
    assert(s1.SPOP() == 20);
    assert(s1.GetSize() == 1);

    // 3. Копирующий конструктор (s1 сейчас содержит [10])
    Stack<int> s2 = s1;
    assert(s2.GetSize() == 1);
    // Убедимся, что это глубокая копия
    s2.SPUSH(50); // s2 = [10, 50], s1 = [10]
    assert(s1.GetSize() == 1);
    assert(s2.GetSize() == 2);
    assert(s2.SPOP() == 50);
    assert(s2.SPOP() == 10);
    
    // 4. Копирующий оператор присваивания
    Stack<int> s3;
    s3 = s1; // s1 = [10]
    assert(s3.GetSize() == 1);
    assert(s3.SPOP() == 10);
    assert(s1.GetSize() == 1); // s1 не должен измениться

    // 5. SSAVE и SLOAD
    s1.SPUSH(100); // s1 = [10, 100]
    s1.SSAVE("stack_test.txt");
    
    // Проверяем содержимое файла
    string content = get_file_content("stack_test.txt");
    assert(content == "2\n10 100 "); // SSAVE пишет "size \n data..."

    Stack<int> s4;
    s4.SLOAD("stack_test.txt");
    assert(s4.GetSize() == 2);
    assert(s4.SPOP() == 100);
    assert(s4.SPOP() == 10);
    
    cout << "Stack... OK" << endl;
}

void test_queue() {
    cout << "--- 2. ТЕСТИРОВАНИЕ ОЧЕРЕДИ (Queue) ---" << endl;

    // 1. Конструкторы, QPUSH, empty
    Queue<int> q1;
    assert(q1.GetSize() == 0);
    assert(q1.empty() == true);
    
    q1.QPUSH(1); // cap=1, size=1
    assert(q1.GetSize() == 1);
    assert(q1.empty() == false);
    
    q1.QPUSH(2); // resize (cap=2), size=2
    assert(q1.GetSize() == 2);

    // 2. QGET
    assert(q1.QGET() == 1);
    assert(q1.GetSize() == 2); // QGET не должен изменять размер

    // 3. QPOP
    assert(q1.QPOP() == 1); // q1 = [2]
    assert(q1.GetSize() == 1);

    // 4. Тест "кольцевого буфера"
    // q1: cap=2, size=1, head=1, tail=0. q1 = [_, 2]
    q1.QPUSH(3); // q1 = [3, 2], cap=2, size=2, head=1, tail=1
    assert(q1.GetSize() == 2);
    
    q1.QPUSH(4); // resize (cap=4), q1 = [2, 3, 4, _], size=3, head=0, tail=3
    assert(q1.GetSize() == 3);
    assert(q1.QPOP() == 2); // q1 = [_, 3, 4, _], size=2, head=1
    assert(q1.QPOP() == 3); // q1 = [_, _, 4, _], size=1, head=2
    assert(q1.QPOP() == 4); // q1 = [], size=0, head=3
    assert(q1.GetSize() == 0);
    assert(q1.empty() == true);

    // 5. Копирующий конструктор
    q1.QPUSH(10);
    q1.QPUSH(20); // q1 = [10, 20]
    Queue<int> q2 = q1;
    assert(q2.GetSize() == 2);
    assert(q2.QPOP() == 10);
    assert(q1.GetSize() == 2); // q1 не должен измениться

    // 6. Копирующий оператор присваивания
    Queue<int> q3;
    q3 = q1; // q1 = [10, 20]
    assert(q3.GetSize() == 2);
    assert(q3.QPOP() == 10);
    assert(q3.QPOP() == 20);

    // 7. QSAVE / QLOAD
    q1.QSAVE("queue_test.txt");
    string content = get_file_content("queue_test.txt");
    assert(content == "2\n10 20 "); // QSAVE пишет "size \n data..."

    Queue<int> q4;
    q4.QLOAD("queue_test.txt");
    assert(q4.GetSize() == 2);
    assert(q4.QPOP() == 10);
    
    // 8. Тест исключений
    Queue<int> q_empty;
    bool exception_caught = false;
    try {
        q_empty.QPOP();
    } catch (const out_of_range& e) {
        exception_caught = true;
    }
    assert(exception_caught);

    cout << "Queue... OK" << endl;
}

void test_array() {
    cout << "--- 3. ТЕСТИРОВАНИЕ МАССИВА (Array) ---" << endl;

    // 1. Конструктор и MPUSH_BACK
    Array<int> arr1;
    assert(arr1.GetSize() == 0);
    arr1.MPUSH_BACK(10); // arr1 = [10]
    assert(arr1.GetSize() == 1);
    arr1.MPUSH_BACK(20); // doubleArray, arr1 = [10, 20]
    assert(arr1.GetSize() == 2);
    
    // 2. MGET_BY_IND
    assert(arr1.MGET_BY_IND(0) == 10);
    assert(arr1.MGET_BY_IND(1) == 20);

    // 3. MPUSH_BY_IND
    arr1.MPUSH_BACK(30); // arr1 = [10, 20, 30]
    arr1.MPUSH_BY_IND(1, 15); // arr1 = [10, 15, 20, 30]
    assert(arr1.GetSize() == 4);
    assert(arr1.MGET_BY_IND(0) == 10);
    assert(arr1.MGET_BY_IND(1) == 15);
    assert(arr1.MGET_BY_IND(2) == 20);
    assert(arr1.MGET_BY_IND(3) == 30);

    // 4. MDEL_BY_IND
    arr1.MDEL_BY_IND(0); // arr1 = [15, 20, 30]
    assert(arr1.GetSize() == 3);
    assert(arr1.MGET_BY_IND(0) == 15);

    // 5. MSWAP_BY_IND
    arr1.MSWAP_BY_IND(2, 99); // arr1 = [15, 20, 99]
    assert(arr1.GetSize() == 3);
    assert(arr1.MGET_BY_IND(2) == 99);

    // 6. Копирование
    Array<int> arr2 = arr1;
    assert(arr2.GetSize() == 3);
    assert(arr2.MGET_BY_IND(1) == 20);
    arr2.MPUSH_BACK(100);
    assert(arr1.GetSize() == 3); // arr1 не изменился
    assert(arr2.GetSize() == 4);
    
    Array<int> arr3;
    arr3 = arr1;
    assert(arr3.GetSize() == 3);
    assert(arr3.MGET_BY_IND(2) == 99);
    
    // 7. MSAVE / MLOAD
    arr1.MSAVE("array_test.txt");
    string content = get_file_content("array_test.txt");
    assert(content == "3\n15 20 99 ");

    Array<int> arr4;
    arr4.MLOAD("array_test.txt");
    assert(arr4.GetSize() == 3);
    assert(arr4.MGET_BY_IND(1) == 20);

    cout << "Array... OK" << endl;
}

void test_singly_list() {
    cout << "--- 4. ТЕСТИРОВАНИЕ ОДНОСВЯЗНОГО СПИСКА (ForwardList) ---" << endl;

    // 1. FCREATE
    ForwardList<int> flist1;
    assert(flist1.GetHead() == nullptr);
    flist1.FCREATE(10);
    assert(flist1.GetHead() != nullptr && flist1.GetHead()->key == 10 && flist1.GetHead()->next == nullptr);

    // 2. FPUSH_HEAD
    flist1.FPUSH_HEAD(5); // list = [5, 10]
    assert(flist1.GetHead()->key == 5 && flist1.GetHead()->next->key == 10);

    // 3. FPUSH_BACK
    flist1.FPUSH_BACK(20); // list = [5, 10, 20]
    assert(flist1.GetHead()->next->next->key == 20 && flist1.GetHead()->next->next->next == nullptr);

    // 4. FGET_BY_VALUE и FPUSH_FORWARD
    SNode<int>* node10 = flist1.FGET_BY_VALUE(10);
    assert(node10 != nullptr && node10->key == 10);
    flist1.FPUSH_FORWARD(node10, 15); // list = [5, 10, 15, 20]
    assert(node10->next->key == 15 && node10->next->next->key == 20);
    
    // 5. FPUSH_BEFORE
    flist1.FPUSH_BEFORE(10, 8); // list = [5, 8, 10, 15, 20]
    assert(flist1.GetHead()->next->key == 8 && flist1.GetHead()->next->next == node10);

    // 6. FDEL_HEAD
    flist1.FDEL_HEAD(); // list = [8, 10, 15, 20]
    assert(flist1.GetHead()->key == 8);
    
    // 7. FDEL_BACK
    flist1.FDEL_BACK(); // list = [8, 10, 15]
    assert(flist1.GetHead()->next->next->key == 15 && flist1.GetHead()->next->next->next == nullptr);

    // 8. FDEL_AFTER
    SNode<int>* node8 = flist1.FGET_BY_VALUE(8);
    flist1.FDEL_AFTER(node8); // list = [8, 15] (удалили 10)
    assert(node8->next->key == 15);

    // 9. FDEL_BY_VALUE
    flist1.FDEL_BY_VALUE(8); // list = [15]
    assert(flist1.GetHead()->key == 15 && flist1.GetHead()->next == nullptr);

    // 10. FDEL_BEFORE
    flist1.FPUSH_BACK(20);
    flist1.FPUSH_BACK(30); // list = [15, 20, 30]
    flist1.FDEL_BEFORE(30); // list = [15, 30] (удалили 20)
    assert(flist1.GetHead()->key == 15 && flist1.GetHead()->next->key == 30);
    
    // 11. Копирование
    ForwardList<int> flist2 = flist1; // flist1 = [15, 30]
    assert(flist2.GetHead()->key == 15 && flist2.GetHead()->next->key == 30);
    assert(flist2.GetHead() != flist1.GetHead()); // Проверка на глубокую копию
    
    ForwardList<int> flist3;
    flist3 = flist1;
    assert(flist3.GetHead()->key == 15 && flist3.GetHead() != flist1.GetHead());

    // 12. FSAVE / FLOAD
    flist1.FSAVE("slist_test.txt");
    string content = get_file_content("slist_test.txt");
    assert(content == "15 30 ");

    ForwardList<int> flist4;
    flist4.FLOAD("slist_test.txt");
    assert(flist4.GetHead()->key == 15 && flist4.GetHead()->next->key == 30);

    cout << "SinglyList... OK" << endl;
}

void test_doubly_list() {
    cout << "--- 5. ТЕСТИРОВАНИЕ ДВУСВЯЗНОГО СПИСКА (DoublyList) ---" << endl;

    // 1. LCREATE
    DoublyList<int> dlist1;
    assert(dlist1.GetHead() == nullptr && dlist1.GetTail() == nullptr);
    dlist1.LCREATE(10);
    assert(dlist1.GetHead() != nullptr && dlist1.GetHead() == dlist1.GetTail());
    assert(dlist1.GetHead()->key == 10 && dlist1.GetHead()->prev == nullptr && dlist1.GetHead()->next == nullptr);

    // 2. LPUSH_HEAD
    dlist1.LPUSH_HEAD(5); // list = [5, 10]
    assert(dlist1.GetHead()->key == 5 && dlist1.GetTail()->key == 10);
    assert(dlist1.GetHead()->next == dlist1.GetTail() && dlist1.GetTail()->prev == dlist1.GetHead());
    assert(dlist1.GetHead()->prev == nullptr && dlist1.GetTail()->next == nullptr);

    // 3. LPUSH_BACK
    dlist1.LPUSH_BACK(20); // list = [5, 10, 20]
    assert(dlist1.GetTail()->key == 20 && dlist1.GetTail()->prev->key == 10);
    assert(dlist1.GetTail()->prev == dlist1.GetHead()->next);

    // 4. LGET_BY_VALUE и LPUSH_AFTER
    DNode<int>* node10 = dlist1.LGET_BY_VALUE(10);
    dlist1.LPUSH_AFTER(10, 15); // list = [5, 10, 15, 20]
    assert(node10->next->key == 15 && node10->next->next->key == 20);
    assert(node10->next->next->prev == node10->next);

    // 5. LPUSH_BEFORE
    dlist1.LPUSH_BEFORE(10, 8); // list = [5, 8, 10, 15, 20]
    assert(node10->prev->key == 8 && node10->prev->prev == dlist1.GetHead());
    assert(dlist1.GetHead()->next == node10->prev);
    
    // 6. LDEL_HEAD
    dlist1.LDEL_HEAD(); // list = [8, 10, 15, 20]
    assert(dlist1.GetHead()->key == 8 && dlist1.GetHead()->prev == nullptr);

    // 7. LDEL_BACK
    dlist1.LDEL_BACK(); // list = [8, 10, 15]
    assert(dlist1.GetTail()->key == 15 && dlist1.GetTail()->next == nullptr);

    // 8. LDEL_AFTER
    dlist1.LDEL_AFTER(8); // list = [8, 15] (удалили 10)
    assert(dlist1.GetHead()->key == 8 && dlist1.GetTail()->key == 15);
    assert(dlist1.GetHead()->next == dlist1.GetTail() && dlist1.GetTail()->prev == dlist1.GetHead());

    // 9. LDEL_BEFORE
    dlist1.LDEL_BEFORE(15); // list = [15] (удалили 8)
    assert(dlist1.GetHead() == dlist1.GetTail() && dlist1.GetHead()->key == 15);

    // 10. LDEL_BY_VALUE
    dlist1.LPUSH_HEAD(5);
    dlist1.LPUSH_BACK(20); // list = [5, 15, 20]
    dlist1.LDEL_BY_VALUE(15); // list = [5, 20]
    assert(dlist1.GetHead()->key == 5 && dlist1.GetTail()->key == 20);
    assert(dlist1.GetHead()->next == dlist1.GetTail() && dlist1.GetTail()->prev == dlist1.GetHead());
    
    // 11. Копирование
    DoublyList<int> dlist2 = dlist1; // dlist1 = [5, 20]
    assert(dlist2.GetHead()->key == 5 && dlist2.GetTail()->key == 20);
    assert(dlist2.GetHead() != dlist1.GetHead()); // Глубокая копия
    
    // 12. LSAVE / LLOAD
    dlist1.LSAVE("dlist_test.txt");
    string content = get_file_content("dlist_test.txt");
    assert(content == "5 20 ");

    DoublyList<int> dlist4;
    dlist4.LLOAD("dlist_test.txt");
    assert(dlist4.GetHead()->key == 5 && dlist4.GetTail()->key == 20);

    cout << "DoublyList... OK" << endl;
}

void test_binary_tree() {
    cout << "--- 6. ТЕСТИРОВАНИЕ БИНАРНОГО ДЕРЕВА (FullBinaryTree) ---" << endl;

    // 1. TINSERT (строит BST)
    FullBinaryTree<int> bst1;
    assert(bst1.GetRoot() == nullptr);
    bst1.TINSERT(50);
    assert(bst1.GetRoot() != nullptr && bst1.GetRoot()->key == 50);
    bst1.TINSERT(30);
    bst1.TINSERT(70);
    bst1.TINSERT(20);
    bst1.TINSERT(40);
    bst1.TINSERT(60);
    bst1.TINSERT(80);
    
    // Проверяем структуру BST
    assert(bst1.GetRoot()->left->key == 30);
    assert(bst1.GetRoot()->right->key == 70);
    assert(bst1.GetRoot()->left->left->key == 20);
    assert(bst1.GetRoot()->left->right->key == 40);
    assert(bst1.GetRoot()->right->left->key == 60);
    assert(bst1.GetRoot()->right->right->key == 80);

    // 2. TFULL
    assert(bst1.TFULL() == true);
    
    FullBinaryTree<int> bst_not_full;
    bst_not_full.TINSERT(50);
    bst_not_full.TINSERT(30);
    assert(bst_not_full.TFULL() == false);

    // 3. Копирующий конструктор
    FullBinaryTree<int> bst2 = bst1;
    assert(bst2.GetRoot()->key == 50);
    assert(bst2.GetRoot()->left->key == 30);
    assert(bst2.GetRoot()->left->left->key == 20);
    assert(bst2.GetRoot() != bst1.GetRoot()); // Глубокая копия
    assert(bst2.GetRoot()->left != bst1.GetRoot()->left);

    // 4. Копирующий оператор присваивания
    FullBinaryTree<int> bst3;
    bst3 = bst1;
    assert(bst3.GetRoot()->key == 50);
    assert(bst3.GetRoot()->right->right->key == 80);
    assert(bst3.GetRoot() != bst1.GetRoot());

    // 5. TSAVE
    // TSAVE делает обход в ширину
    bst1.TSAVE("tree_test.txt");
    string content = get_file_content("tree_test.txt");
    // Ожидаемый порядок (в ширину): 50 30 70 20 40 60 80
    assert(content == "50 30 70 20 40 60 80 ");

    // 6. TLOAD
    // TLOAD использует TINSERT, поэтому он перестроит BST
    FullBinaryTree<int> bst4;
    bst4.TLOAD("tree_test.txt");
    // Структура должна быть идентична bst1
    assert(bst4.GetRoot()->key == 50);
    assert(bst4.GetRoot()->left->key == 30);
    assert(bst4.GetRoot()->right->key == 70);
    assert(bst4.GetRoot()->left->left->key == 20);

    // 7. PRINT (визуальные функции, не проверяются assert'ом)
    cout << "Визуальная печать (проверьте глазами):" << endl;
    bst1.PRINT(5);

    cout << "BinaryTree... OK" << endl;
}