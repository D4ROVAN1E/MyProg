#define BOOST_TEST_MODULE ArrayTestModule
#include <boost/test/included/unit_test.hpp>
#include <string>
#include <vector>
#include <cstdio> // для std::remove

#include "array.hpp" // Ваш заголовочный файл

using namespace std;

// Вспомогательная функция для очистки тестовых файлов
void cleanFile(const string& filename) {
    std::remove(filename.c_str());
}

BOOST_AUTO_TEST_SUITE(ArrayTests)

// 1. Тест конструкторов и базовых геттеров
BOOST_AUTO_TEST_CASE(ConstructorsAndGetters) {
    // Default constructor
    Array<int> arrDefault;
    BOOST_CHECK_EQUAL(arrDefault.GetSize(), 0);
    BOOST_CHECK_EQUAL(arrDefault.GetCapacity(), 1);

    // Constructor with capacity
    // Внимание: в вашем коде size инициализируется как cap - 1
    uint32_t initialCap = 10;
    Array<int> arrCap(initialCap);
    BOOST_CHECK_EQUAL(arrCap.GetCapacity(), initialCap);
    BOOST_CHECK_EQUAL(arrCap.GetSize(), initialCap - 1); 
    
    // Проверяем, что элементы занулены (или T())
    for(uint32_t i = 0; i < arrCap.GetSize(); ++i) {
        BOOST_CHECK_EQUAL(arrCap[i], 0);
    }
}

// 2. Тест добавления элементов и автоматического расширения (doubleArray)
BOOST_AUTO_TEST_CASE(PushBackAndResize) {
    Array<int> arr;
    // Изначальная емкость 1. Добавим 5 элементов, чтобы вызвать doubleArray несколько раз.
    for (int i = 0; i < 5; ++i) {
        arr.MPUSH_BACK(i * 10);
    }

    BOOST_CHECK_EQUAL(arr.GetSize(), 5);
    BOOST_CHECK(arr.GetCapacity() >= 5); // Емкость должна была вырасти

    // Проверка значений
    BOOST_CHECK_EQUAL(arr[0], 0);
    BOOST_CHECK_EQUAL(arr[4], 40);
}

// 3. Тест доступа по индексу и оператора []
BOOST_AUTO_TEST_CASE(AccessAndExceptions) {
    Array<string> arr;
    arr.MPUSH_BACK("test");

    // Valid access
    BOOST_CHECK_EQUAL(arr[0], "test");
    BOOST_CHECK_EQUAL(arr.MGET_BY_IND(0), "test");

    // Invalid access (Operator [])
    BOOST_CHECK_THROW(arr[1], std::out_of_range);

    // Invalid access (MGET_BY_IND)
    BOOST_CHECK_THROW(arr.MGET_BY_IND(5), std::out_of_range);
}

// 4. Тест вставки, удаления и замены (Insert, Delete, Swap)
BOOST_AUTO_TEST_CASE(ModificationMethods) {
    Array<int> arr;
    arr.MPUSH_BACK(1);
    arr.MPUSH_BACK(3);

    // MPUSH_BY_IND: Вставляем 2 между 1 и 3
    // Индексы сейчас: [0]=1, [1]=3. Вставляем на [1].
    arr.MPUSH_BY_IND(1, 2); 
    
    BOOST_CHECK_EQUAL(arr.GetSize(), 3);
    BOOST_CHECK_EQUAL(arr[0], 1);
    BOOST_CHECK_EQUAL(arr[1], 2);
    BOOST_CHECK_EQUAL(arr[2], 3);

    // Тест вставки с расширением (выход за capacity)
    // Заполним массив до предела
    while(arr.GetSize() < arr.GetCapacity()) {
        arr.MPUSH_BACK(99);
    }
    // Вставка должна вызвать resize
    uint32_t oldCap = arr.GetCapacity();
    arr.MPUSH_BY_IND(0, 100);
    BOOST_CHECK(arr.GetCapacity() > oldCap);
    BOOST_CHECK_EQUAL(arr[0], 100);

    // MDEL_BY_IND: Удаляем первый элемент
    arr.MDEL_BY_IND(0);
    BOOST_CHECK_EQUAL(arr[0], 1); // 1 сместилась на место 0

    // MSWAP_BY_IND
    arr.MSWAP_BY_IND(0, 555);
    BOOST_CHECK_EQUAL(arr[0], 555);

    // Ошибки границ
    BOOST_CHECK_THROW(arr.MPUSH_BY_IND(100, 1), std::out_of_range);
    BOOST_CHECK_THROW(arr.MDEL_BY_IND(100), std::out_of_range);
    BOOST_CHECK_THROW(arr.MSWAP_BY_IND(100, 1), std::out_of_range);
}

// 5. Тест конструктора копирования и оператора присваивания
BOOST_AUTO_TEST_CASE(CopyAndAssign) {
    Array<int> original;
    original.MPUSH_BACK(10);
    original.MPUSH_BACK(20);

    // Copy Constructor
    Array<int> copy(original);
    BOOST_CHECK_EQUAL(copy.GetSize(), original.GetSize());
    BOOST_CHECK_EQUAL(copy[0], 10);
    
    // Проверка Deep Copy (изменение копии не влияет на оригинал)
    copy.MSWAP_BY_IND(0, 999);
    BOOST_CHECK_EQUAL(copy[0], 999);
    BOOST_CHECK_EQUAL(original[0], 10);

    // Assignment Operator
    Array<int> assigned;
    assigned = original;
    BOOST_CHECK_EQUAL(assigned.GetSize(), 2);
    BOOST_CHECK_EQUAL(assigned[1], 20);

    // Self-assignment check (безопасность a = a)
    assigned = assigned;
    BOOST_CHECK_EQUAL(assigned.GetSize(), 2);
    BOOST_CHECK_EQUAL(assigned[0], 10);
}

// 6. Тест Setters (SetSize, SetCapacity)
BOOST_AUTO_TEST_CASE(SettersLogic) {
    Array<int> arr;
    arr.MPUSH_BACK(1);
    arr.MPUSH_BACK(2); // size 2, cap 2 (обычно удваивается с 1)

    // SetCapacity
    arr.SetCapacity(10);
    BOOST_CHECK_EQUAL(arr.GetCapacity(), 10);
    
    // Ошибка: новая capacity меньше текущего size
    BOOST_CHECK_THROW(arr.SetCapacity(1), std::length_error);

    // SetSize
    arr.SetSize(5); // Допустимо, так как capacity 10
    BOOST_CHECK_EQUAL(arr.GetSize(), 5);

    // Ошибка: новый size больше capacity
    BOOST_CHECK_THROW(arr.SetSize(20), std::length_error);
}

// 7. Тест текстового сохранения и загрузки (MSAVE / MLOAD)
BOOST_AUTO_TEST_CASE(TextFileIO) {
    string filename = "test_array.txt";
    {
        Array<int> arrOut;
        arrOut.MPUSH_BACK(100);
        arrOut.MPUSH_BACK(200);
        arrOut.MSAVE(filename);
    }

    Array<int> arrIn;
    arrIn.MLOAD(filename);

    BOOST_CHECK_EQUAL(arrIn.GetSize(), 2);
    BOOST_CHECK_EQUAL(arrIn[0], 100);
    BOOST_CHECK_EQUAL(arrIn[1], 200);

    cleanFile(filename);

    // Тест ошибки открытия файла
    BOOST_CHECK_THROW(arrIn.MLOAD("non_existent_file.txt"), std::runtime_error);
}

// 8. Тест бинарного сохранения и загрузки (MSAVE_BINARY / MLOAD_BINARY)
BOOST_AUTO_TEST_CASE(BinaryFileIO) {
    string filename = "test_array.bin";
    {
        Array<double> arrOut;
        arrOut.MPUSH_BACK(1.1);
        arrOut.MPUSH_BACK(2.2);
        arrOut.MSAVE_BINARY(filename);
    }

    Array<double> arrIn;
    arrIn.MLOAD_BINARY(filename);

    BOOST_CHECK_EQUAL(arrIn.GetSize(), 2);
    BOOST_CHECK_CLOSE(arrIn[0], 1.1, 0.001);
    BOOST_CHECK_CLOSE(arrIn[1], 2.2, 0.001);

    cleanFile(filename);
    
    // Тест ошибки чтения
    BOOST_CHECK_THROW(arrIn.MLOAD_BINARY("non_existent_bin.bin"), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()