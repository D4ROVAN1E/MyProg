#define BOOST_TEST_MODULE SinglyListTests
#include <boost/test/included/unit_test.hpp>
#include "singly_list.hpp"
#include <vector>
#include <string>
#include <cstdio> // Для remove() файла

// Вспомогательная функция для проверки содержимого списка
template <typename T>
void CheckListContent(const ForwardList<T>& list, const std::vector<T>& expected) {
    auto current = list.GetHead();
    for (const auto& val : expected) {
        BOOST_REQUIRE_MESSAGE(current != nullptr, "List is shorter than expected vector");
        BOOST_CHECK_EQUAL(current->key, val);
        current = current->next;
    }
    BOOST_CHECK_MESSAGE(current == nullptr, "List is longer than expected vector");
}

BOOST_AUTO_TEST_SUITE(ListTestSuite)

// --- Тесты создания и базовой вставки ---
BOOST_AUTO_TEST_CASE(TestCreateAndPushHeadBack) {
    ForwardList<int> list;
    BOOST_CHECK(list.GetHead() == nullptr); // Проверка конструктора по умолчанию

    list.FCREATE(10);
    CheckListContent(list, {10});

    // FCREATE должен очистить старый список
    list.FCREATE(20);
    CheckListContent(list, {20});

    list.FPUSH_HEAD(10);
    CheckListContent(list, {10, 20});

    list.FPUSH_BACK(30);
    CheckListContent(list, {10, 20, 30});
    
    // Тест FPUSH_BACK на пустой список
    ForwardList<int> list2;
    list2.FPUSH_BACK(5);
    CheckListContent(list2, {5});
}

// --- Тест вставки "После" и "До" ---
BOOST_AUTO_TEST_CASE(TestPushForwardAndBefore) {
    ForwardList<int> list;
    list.FPUSH_BACK(10);
    list.FPUSH_BACK(30); // 10 -> 30

    // Вставка ПОСЛЕ головы
    auto head = list.GetHead();
    list.FPUSH_FORWARD(head, 20); 
    CheckListContent(list, {10, 20, 30});

    // Вставка ДО значения (середина)
    list.FPUSH_BEFORE(30, 25);
    CheckListContent(list, {10, 20, 25, 30});

    // Вставка ДО значения (голова)
    list.FPUSH_BEFORE(10, 5);
    CheckListContent(list, {5, 10, 20, 25, 30});
}

// --- Тесты удаления ---
BOOST_AUTO_TEST_CASE(TestDeletions) {
    ForwardList<int> list;
    // 10 -> 20 -> 30 -> 40 -> 50
    list.FPUSH_BACK(10); list.FPUSH_BACK(20); list.FPUSH_BACK(30); 
    list.FPUSH_BACK(40); list.FPUSH_BACK(50);

    // Удаление головы
    list.FDEL_HEAD(); // 20 -> 30 -> 40 -> 50
    CheckListContent(list, {20, 30, 40, 50});

    // Удаление хвоста
    list.FDEL_BACK(); // 20 -> 30 -> 40
    CheckListContent(list, {20, 30, 40});

    // Удаление после узла (удаляем 40, который после 30)
    auto node30 = list.FGET_BY_VALUE(30);
    list.FDEL_AFTER(node30); // 20 -> 30
    CheckListContent(list, {20, 30});

    // Удаление по значению
    list.FPUSH_BACK(40); // 20 -> 30 -> 40
    list.FDEL_BY_VALUE(20); // Удаление головы по значению -> 30 -> 40
    CheckListContent(list, {30, 40});

    list.FDEL_BY_VALUE(40); // Удаление хвоста по значению -> 30
    CheckListContent(list, {30});
}

// --- Тесты удаления "ДО" (самая сложная логика) ---
BOOST_AUTO_TEST_CASE(TestDelBefore) {
    ForwardList<int> list;
    list.FPUSH_BACK(10); list.FPUSH_BACK(20); list.FPUSH_BACK(30); list.FPUSH_BACK(40);
    // 10 -> 20 -> 30 -> 40

    // Удалить элемент до 40 (это 30)
    list.FDEL_BEFORE(40);
    CheckListContent(list, {10, 20, 40});

    // Удалить элемент до 20 (это 10, т.е. голова)
    // Это специальная ветка в коде: if (head->next->key == targetKey)
    list.FDEL_BEFORE(20); 
    CheckListContent(list, {20, 40});
}

// --- Тесты исключений (Exceptions) для покрытия ---
BOOST_AUTO_TEST_CASE(TestExceptions) {
    ForwardList<int> emptyList;
    ForwardList<int> list;
    list.FPUSH_BACK(10);
    list.FPUSH_BACK(20);

    // FPUSH_FORWARD: null ptr
    BOOST_CHECK_THROW(list.FPUSH_FORWARD(nullptr, 5), std::invalid_argument);

    // FPUSH_BEFORE
    BOOST_CHECK_THROW(emptyList.FPUSH_BEFORE(1, 1), std::runtime_error); // Empty
    BOOST_CHECK_THROW(list.FPUSH_BEFORE(999, 5), std::runtime_error); // Not found

    // FDEL_HEAD
    BOOST_CHECK_THROW(emptyList.FDEL_HEAD(), std::runtime_error);

    // FDEL_BACK
    BOOST_CHECK_THROW(emptyList.FDEL_BACK(), std::runtime_error);
    // Удаление единственного элемента через FDEL_BACK (проверка ветки if (head->next == nullptr))
    ForwardList<int> single; 
    single.FPUSH_BACK(1);
    single.FDEL_BACK(); 
    BOOST_CHECK(single.GetHead() == nullptr);

    // FDEL_AFTER
    BOOST_CHECK_THROW(list.FDEL_AFTER(nullptr), std::invalid_argument);
    auto tail = list.FGET_BY_VALUE(20);
    BOOST_CHECK_THROW(list.FDEL_AFTER(tail), std::runtime_error); // No element after

    // FDEL_BEFORE
    BOOST_CHECK_THROW(emptyList.FDEL_BEFORE(10), std::runtime_error); // Empty
    BOOST_CHECK_THROW(list.FDEL_BEFORE(10), std::logic_error); // Cannot delete before head
    BOOST_CHECK_THROW(single.FDEL_BEFORE(99), std::runtime_error); // List too short/empty logic
    BOOST_CHECK_THROW(list.FDEL_BEFORE(999), std::runtime_error); // Target not found

    // FDEL_BY_VALUE
    BOOST_CHECK_THROW(emptyList.FDEL_BY_VALUE(10), std::runtime_error);
    BOOST_CHECK_THROW(list.FDEL_BY_VALUE(999), std::runtime_error);
}

// --- Тесты копирования и присваивания ---
BOOST_AUTO_TEST_CASE(TestCopyAndAssign) {
    ForwardList<int> list1;
    list1.FPUSH_BACK(1); list1.FPUSH_BACK(2);

    // Конструктор копирования
    ForwardList<int> list2(list1);
    CheckListContent(list2, {1, 2});

    // Deep copy check: меняем list1, list2 не должен измениться
    list1.FDEL_HEAD();
    CheckListContent(list1, {2});
    CheckListContent(list2, {1, 2});

    // Оператор присваивания
    ForwardList<int> list3;
    list3 = list2;
    CheckListContent(list3, {1, 2});

    // Самоприсваивание
    list3 = list3;
    CheckListContent(list3, {1, 2});
    
    // Копирование пустого списка
    ForwardList<int> empty1;
    ForwardList<int> empty2(empty1);
    BOOST_CHECK(empty2.GetHead() == nullptr);
}

// --- Тесты ввода/вывода (Файлы) ---
BOOST_AUTO_TEST_CASE(TestFileIO) {
    ForwardList<int> list;
    list.FPUSH_BACK(100);
    list.FPUSH_BACK(200);
    
    std::string txtFile = "test_list.txt";
    std::string binFile = "test_list.bin";

    // Текстовый формат
    list.FSAVE(txtFile);
    
    ForwardList<int> listLoaded;
    listLoaded.FLOAD(txtFile);
    CheckListContent(listLoaded, {100, 200});
    
    // Проверка загрузки в непустой список (должен очиститься)
    listLoaded.FPUSH_BACK(300);
    listLoaded.FLOAD(txtFile);
    CheckListContent(listLoaded, {100, 200});

    // Бинарный формат
    list.FSERIALIZE(binFile);
    ForwardList<int> listBinLoaded;
    listBinLoaded.FDESERIALIZE(binFile);
    CheckListContent(listBinLoaded, {100, 200});

    // Тест печати (просто чтобы убедиться, что не падает)
    std::cout << "Testing Print Output:" << std::endl;
    list.PRINT();
    ForwardList<int> empty;
    empty.PRINT();

    // Очистка временных файлов
    std::remove(txtFile.c_str());
    std::remove(binFile.c_str());
    
    // Тест ошибок открытия файлов
    BOOST_CHECK_THROW(list.FSAVE("/invalid/path/out.txt"), std::runtime_error);
    BOOST_CHECK_THROW(list.FLOAD("non_existent_file.txt"), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()