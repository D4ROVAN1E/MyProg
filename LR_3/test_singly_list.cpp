#define BOOST_TEST_MODULE SinglyListTests
#include <boost/test/included/unit_test.hpp>
#include "singly_list.hpp"
#include <string>
#include <cstdio> // Для remove()

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

// Вспомогательная функция для проверки содержимого списка 
template <typename T>
void CheckListManual(const ForwardList<T>& list, const T* expectedValues, size_t size) {
    auto current = list.GetHead();
    for (size_t i = 0; i < size; ++i) {
        BOOST_REQUIRE_MESSAGE(current != nullptr, "List ends prematurely");
        BOOST_CHECK_EQUAL(current->key, expectedValues[i]);
        current = current->next;
    }
    BOOST_CHECK_MESSAGE(current == nullptr, "List contains more elements than expected");
}

BOOST_AUTO_TEST_SUITE(ListTestSuite)

// Тесты создания и базовой вставки (HEAD/BACK/CREATE)
BOOST_AUTO_TEST_CASE(TestCreateAndPushHeadBack) {
    ForwardList<int> list;
    BOOST_CHECK(list.GetHead() == nullptr); // Покрытие конструктора

    // Тест FCREATE
    list.FCREATE(10);
    int exp1[] = {10};
    CheckListManual(list, exp1, 1);

    // FCREATE должен очистить старый список 
    list.FCREATE(20);
    int exp2[] = {20};
    CheckListManual(list, exp2, 1);

    // FPUSH_HEAD
    list.FPUSH_HEAD(10);
    int exp3[] = {10, 20};
    CheckListManual(list, exp3, 2);

    // FPUSH_BACK
    list.FPUSH_BACK(30);
    int exp4[] = {10, 20, 30};
    CheckListManual(list, exp4, 3);
    
    // Тест FPUSH_BACK на пустой список 
    ForwardList<int> list2;
    list2.FPUSH_BACK(5);
    int exp5[] = {5};
    CheckListManual(list2, exp5, 1);
}

// Тест вставки после и до (FORWARD/BEFORE)
BOOST_AUTO_TEST_CASE(TestPushForwardAndBefore) {
    ForwardList<int> list;
    list.FPUSH_BACK(10);
    list.FPUSH_BACK(30); 

    // Вставка ПОСЛЕ головы (FPUSH_FORWARD)
    auto head = list.GetHead();
    list.FPUSH_FORWARD(head, 20); 
    int exp1[] = {10, 20, 30};
    CheckListManual(list, exp1, 3);

    // Вставка ДО значения - середина списка (FPUSH_BEFORE)
    list.FPUSH_BEFORE(30, 25);
    int exp2[] = {10, 20, 25, 30};
    CheckListManual(list, exp2, 4);

    // Вставка ДО значения - голова списка (
    list.FPUSH_BEFORE(10, 5);
    int exp3[] = {5, 10, 20, 25, 30};
    CheckListManual(list, exp3, 5);
}

// Тесты удаления (HEAD/BACK/AFTER/BY_VALUE)
BOOST_AUTO_TEST_CASE(TestDeletions) {
    ForwardList<int> list;
    // Создаем: 10 -> 20 -> 30 -> 40 -> 50
    list.FPUSH_BACK(10); list.FPUSH_BACK(20); list.FPUSH_BACK(30); 
    list.FPUSH_BACK(40); list.FPUSH_BACK(50);

    // Удаление головы (FDEL_HEAD)
    list.FDEL_HEAD(); 
    int exp1[] = {20, 30, 40, 50};
    CheckListManual(list, exp1, 4);

    // Удаление хвоста (FDEL_BACK)
    list.FDEL_BACK(); 
    int exp2[] = {20, 30, 40};
    CheckListManual(list, exp2, 3);

    // Удаление после узла (FDEL_AFTER)
    // Получаем узел 30 и удаляем то, что после него (40)
    auto node30 = list.FGET_BY_VALUE(30);
    list.FDEL_AFTER(node30); 
    int exp3[] = {20, 30};
    CheckListManual(list, exp3, 2);

    // Удаление по значению (FDEL_BY_VALUE) - случай головы
    list.FPUSH_BACK(40); // восстановим 40 -> {20, 30, 40}
    list.FDEL_BY_VALUE(20); 
    int exp4[] = {30, 40};
    CheckListManual(list, exp4, 2);

    // Удаление по значению - обычный случай
    list.FDEL_BY_VALUE(40);
    int exp5[] = {30};
    CheckListManual(list, exp5, 1);
}

// Тесты сложного удаления (FDEL_BEFORE)
BOOST_AUTO_TEST_CASE(TestDelBefore) {
    ForwardList<int> list;
    list.FPUSH_BACK(10); list.FPUSH_BACK(20); list.FPUSH_BACK(30); list.FPUSH_BACK(40);
    // 10 -> 20 -> 30 -> 40

    // Удалить элемент до 40 (это 30)
    list.FDEL_BEFORE(40);
    int exp1[] = {10, 20, 40};
    CheckListManual(list, exp1, 3);

    // Удалить элемент до 20 (это 10, т.е. голова)
    list.FDEL_BEFORE(20); 
    int exp2[] = {20, 40};
    CheckListManual(list, exp2, 2);
}

// Тесты исключений 
BOOST_AUTO_TEST_CASE(TestExceptions) {
    ForwardList<int> emptyList;
    ForwardList<int> list;
    list.FPUSH_BACK(10);
    list.FPUSH_BACK(20);

    // FPUSH_FORWARD exceptions
    BOOST_CHECK_THROW(list.FPUSH_FORWARD(nullptr, 5), invalid_argument);

    // FPUSH_BEFORE exceptions
    BOOST_CHECK_THROW(emptyList.FPUSH_BEFORE(1, 1), runtime_error); // Пустой
    BOOST_CHECK_THROW(list.FPUSH_BEFORE(999, 5), runtime_error);    // Не найден

    // FDEL_HEAD exceptions
    BOOST_CHECK_THROW(emptyList.FDEL_HEAD(), runtime_error);

    // FDEL_BACK exceptions
    BOOST_CHECK_THROW(emptyList.FDEL_BACK(), runtime_error);
    
    // Удаление единственного элемента через FDEL_BACK 
    ForwardList<int> single; 
    single.FPUSH_BACK(1);
    single.FDEL_BACK(); 
    BOOST_CHECK(single.GetHead() == nullptr);

    // FDEL_AFTER exceptions
    BOOST_CHECK_THROW(list.FDEL_AFTER(nullptr), invalid_argument);
    auto tail = list.FGET_BY_VALUE(20);
    BOOST_CHECK_THROW(list.FDEL_AFTER(tail), runtime_error); // Нет элемента после хвоста

    // FDEL_BEFORE exceptions
    BOOST_CHECK_THROW(emptyList.FDEL_BEFORE(10), runtime_error); // Пустой
    BOOST_CHECK_THROW(list.FDEL_BEFORE(10), logic_error); // Попытка удалить перед головой
    
    // Список слишком короткий для удаления "перед" чем-то (если элемент не голова)
    ForwardList<int> shortList;
    shortList.FPUSH_BACK(5);
    BOOST_CHECK_THROW(shortList.FDEL_BEFORE(99), runtime_error); // !head->next

    BOOST_CHECK_THROW(list.FDEL_BEFORE(999), runtime_error); // Цель не найдена

    // FDEL_BY_VALUE exceptions
    BOOST_CHECK_THROW(emptyList.FDEL_BY_VALUE(10), runtime_error);
    BOOST_CHECK_THROW(list.FDEL_BY_VALUE(999), runtime_error);
}

// Тесты копирования и присваивания
BOOST_AUTO_TEST_CASE(TestCopyAndAssign) {
    ForwardList<int> list1;
    list1.FPUSH_BACK(1); list1.FPUSH_BACK(2);
    int exp[] = {1, 2};

    // Конструктор копирования
    ForwardList<int> list2(list1);
    CheckListManual(list2, exp, 2);

    // Тест глубокого копирования
    list1.FDEL_HEAD();
    int expL1[] = {2};
    CheckListManual(list1, expL1, 1);
    CheckListManual(list2, exp, 2);

    // Оператор присваивания
    ForwardList<int> list3;
    list3 = list2;
    CheckListManual(list3, exp, 2);

    // Самоприсваивание
    list3 = list3;
    CheckListManual(list3, exp, 2);
    
    // Копирование пустого списка 
    ForwardList<int> empty1;
    ForwardList<int> empty2(empty1);
    BOOST_CHECK(empty2.GetHead() == nullptr);
}

// Тесты ввода/вывода 
BOOST_AUTO_TEST_CASE(TestFileIO) {
    ForwardList<int> list;
    list.FPUSH_BACK(100);
    list.FPUSH_BACK(200);
    
    string txtFile = "test_list_manual.txt";
    string binFile = "test_list_manual.bin";
    int exp[] = {100, 200};

    // Сохранение и чтение (Текст)
    list.FSAVE(txtFile);
    
    ForwardList<int> listLoaded;
    listLoaded.FLOAD(txtFile);
    CheckListManual(listLoaded, exp, 2);
    
    // Проверка загрузки в непустой список (должен очиститься внутри FLOAD)
    listLoaded.FPUSH_BACK(300);
    listLoaded.FLOAD(txtFile);
    CheckListManual(listLoaded, exp, 2);

    // Сохранение и чтение (Бинарный)
    list.FSERIALIZE(binFile);
    ForwardList<int> listBinLoaded;
    listBinLoaded.FDESERIALIZE(binFile);
    CheckListManual(listBinLoaded, exp, 2);

    // Тест PRINT 
    {
        CoutRedirect capture;
        list.PRINT();
        string out1 = capture.getString();
        BOOST_CHECK(out1.find("100 -> 200 -> nullptr") != string::npos);
        ForwardList<int> empty;
        empty.PRINT();
        string out2 = capture.getString();
        BOOST_CHECK(out2.find("Список пуст") != string::npos);
    }

    // Очистка файлов
    remove(txtFile.c_str());
    remove(binFile.c_str());
    
    // Тест ошибок открытия файлов
    BOOST_CHECK_THROW(list.FLOAD("non_existent_file_XYZ.txt"), runtime_error);
    BOOST_CHECK_THROW(list.FDESERIALIZE("non_existent_file_XYZ.bin"), runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()