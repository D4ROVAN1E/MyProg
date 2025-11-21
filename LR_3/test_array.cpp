#define BOOST_TEST_MODULE ArrayTestModule
#include <boost/test/included/unit_test.hpp>
#include <string>
#include <vector>
#include <cstdio> // для remove

#include "array.hpp" 

using namespace std;

// Вспомогательная функция для очистки тестовых файлов
void cleanFile(const string& filename) {
    remove(filename.c_str());
}

BOOST_AUTO_TEST_SUITE(ArrayTests)

// Тест конструкторов и базовых геттеров
BOOST_AUTO_TEST_CASE(ConstructorsAndGetters) {
    // Default constructor
    Array<int> arrDefault;
    BOOST_CHECK_EQUAL(arrDefault.GetSize(), 0);
    BOOST_CHECK_EQUAL(arrDefault.GetCapacity(), 1);

    // Constructor with capacity
    uint32_t initialCap = 10;
    Array<int> arrCap(initialCap);
    BOOST_CHECK_EQUAL(arrCap.GetCapacity(), initialCap);
    BOOST_CHECK_EQUAL(arrCap.GetSize(), initialCap - 1); 
    
    // Проверяем, что элементы занулены (или T())
    for(uint32_t i = 0; i < arrCap.GetSize(); ++i) {
        BOOST_CHECK_EQUAL(arrCap[i], 0);
    }
}

// Тест добавления элементов и автоматического расширения (doubleArray)
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

// Тест доступа по индексу и оператора []
BOOST_AUTO_TEST_CASE(AccessAndExceptions) {
    Array<string> arr;
    const Array<int> constArr(1);
    arr.MPUSH_BACK("test");

    // Valid access
    BOOST_CHECK_EQUAL(arr[0], "test");
    BOOST_CHECK_EQUAL(arr.MGET_BY_IND(0), "test");

    // Invalid access (Operator [])
    BOOST_CHECK_THROW(arr[1], out_of_range);

    //Invalid access (Operator const [])
    BOOST_CHECK_THROW(constArr[5], out_of_range);

    // Invalid access (MGET_BY_IND)
    BOOST_CHECK_THROW(arr.MGET_BY_IND(5), out_of_range);
}

// Тест вставки, удаления и замены (Insert, Delete, Swap)
BOOST_AUTO_TEST_CASE(ModificationMethods) {
    Array<int> arr;
    arr.MPUSH_BACK(1);
    arr.MPUSH_BACK(3);

    // MВставляем 2 между 1 и 3
    // [0]=1, [1]=3. Вставляем на [1].
    arr.MPUSH_BY_IND(1, 2); 
    
    BOOST_CHECK_EQUAL(arr.GetSize(), 3);
    BOOST_CHECK_EQUAL(arr[0], 1);
    BOOST_CHECK_EQUAL(arr[1], 2);
    BOOST_CHECK_EQUAL(arr[2], 3);

    // Тест вставки с расширением 
    // Заполним массив до предела
    while(arr.GetSize() < arr.GetCapacity()) {
        arr.MPUSH_BACK(99);
    }
    // Вставка должна вызвать resize
    uint32_t oldCap = arr.GetCapacity();
    arr.MPUSH_BY_IND(0, 100);
    BOOST_CHECK(arr.GetCapacity() > oldCap);
    BOOST_CHECK_EQUAL(arr[0], 100);

    // Удаляем первый элемент
    arr.MDEL_BY_IND(0);
    BOOST_CHECK_EQUAL(arr[0], 1); // 1 сместилась на место 0

    // MSWAP_BY_IND
    arr.MSWAP_BY_IND(0, 555);
    BOOST_CHECK_EQUAL(arr[0], 555);

    // Ошибки границ
    BOOST_CHECK_THROW(arr.MPUSH_BY_IND(100, 1), out_of_range);
    BOOST_CHECK_THROW(arr.MDEL_BY_IND(100), out_of_range);
    BOOST_CHECK_THROW(arr.MSWAP_BY_IND(100, 1), out_of_range);
}

// Тест конструктора копирования и оператора присваивания
BOOST_AUTO_TEST_CASE(CopyAndAssign) {
    Array<int> original;
    original.MPUSH_BACK(10);
    original.MPUSH_BACK(20);

    // Copy Constructor
    Array<int> copy(original);
    BOOST_CHECK_EQUAL(copy.GetSize(), original.GetSize());
    BOOST_CHECK_EQUAL(copy[0], 10);
    
    // Проверка Deep Copy
    copy.MSWAP_BY_IND(0, 999);
    BOOST_CHECK_EQUAL(copy[0], 999);
    BOOST_CHECK_EQUAL(original[0], 10);

    // Оператор присваивания
    Array<int> assigned;
    assigned = original;
    BOOST_CHECK_EQUAL(assigned.GetSize(), 2);
    BOOST_CHECK_EQUAL(assigned[1], 20);

    // Самоприсваивание
    assigned = assigned;
    BOOST_CHECK_EQUAL(assigned.GetSize(), 2);
    BOOST_CHECK_EQUAL(assigned[0], 10);
}

// Сеттеры
BOOST_AUTO_TEST_CASE(SettersLogic) {
    Array<int> arr;
    arr.MPUSH_BACK(1);
    arr.MPUSH_BACK(2); // size 2, cap 2 

    // SetCapacity
    arr.SetCapacity(10);
    BOOST_CHECK_EQUAL(arr.GetCapacity(), 10);
    
    // новая capacity меньше текущего size
    BOOST_CHECK_THROW(arr.SetCapacity(1), length_error);

    // SetSize
    arr.SetSize(5); // Допустимо, так как capacity 10
    BOOST_CHECK_EQUAL(arr.GetSize(), 5);

    // новый size больше capacity
    BOOST_CHECK_THROW(arr.SetSize(20), length_error);
}

// Тест текстового сохранения и загрузки (MSAVE / MLOAD)
BOOST_AUTO_TEST_CASE(TextFileIO) {
    string filename = "test_array.txt";
    string badFile = "/root/forbidden.txt";
    {
        Array<int> arrOut;
        arrOut.MPUSH_BACK(100);
        arrOut.MPUSH_BACK(200);
        arrOut.MSAVE(filename);
        BOOST_CHECK_THROW(arrOut.MSAVE(badFile), runtime_error);
    }

    Array<int> arrIn;
    arrIn.MLOAD(filename);

    BOOST_CHECK_EQUAL(arrIn.GetSize(), 2);
    BOOST_CHECK_EQUAL(arrIn[0], 100);
    BOOST_CHECK_EQUAL(arrIn[1], 200);

    cleanFile(filename);

    // Тест ошибки открытия файла
    BOOST_CHECK_THROW(arrIn.MLOAD("non_existent_file.txt"), runtime_error);
}

// Тест бинарного сохранения и загрузки 
BOOST_AUTO_TEST_CASE(BinaryFileIO) {
    string filename = "test_array.bin";
    string badFile = "/root/forbidden.txt";
    {
        Array<double> arrOut;
        arrOut.MPUSH_BACK(1.1);
        arrOut.MPUSH_BACK(2.2);
        arrOut.MSAVE_BINARY(filename);
        BOOST_CHECK_THROW(arrOut.MSAVE(badFile), runtime_error);
    }

    Array<double> arrIn;
    arrIn.MLOAD_BINARY(filename);

    BOOST_CHECK_EQUAL(arrIn.GetSize(), 2);
    BOOST_CHECK_CLOSE(arrIn[0], 1.1, 0.001);
    BOOST_CHECK_CLOSE(arrIn[1], 2.2, 0.001);

    cleanFile(filename);
    
    // Тест ошибки чтения
    BOOST_CHECK_THROW(arrIn.MLOAD_BINARY("non_existent_bin.bin"), runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()