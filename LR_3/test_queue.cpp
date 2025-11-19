#define BOOST_TEST_MODULE QueueTests
#include <boost/test/included/unit_test.hpp>

#include "queue.hpp"
#include <string>
#include <vector>
#include <cstdio> // Для remove()

// Вспомогательная функция для проверки равенства очередей
template <typename T>
bool queuesAreEqual(Queue<T>& q1, const std::vector<T>& expectedData) {
    if (q1.GetSize() != expectedData.size()) return false;
    
    // Создаем копию, чтобы не разрушать исходную очередь при проверке
    Queue<T> temp = q1; 
    for (const auto& val : expectedData) {
        if (temp.QPOP() != val) return false;
    }
    return true;
}

BOOST_AUTO_TEST_SUITE(QueueTestSuite)

// 1. Тест конструкторов и базового состояния
BOOST_AUTO_TEST_CASE(ConstructorTest) {
    // Default constructor
    Queue<int> q1;
    BOOST_CHECK(q1.empty());
    BOOST_CHECK_EQUAL(q1.GetSize(), 0);

    // Parameterized constructor
    Queue<int> q2(10);
    BOOST_CHECK(q2.empty());
    BOOST_CHECK_EQUAL(q2.GetSize(), 0);

    // Edge case: Zero capacity (должна стать 1)
    Queue<int> q3(0);
    q3.QPUSH(5);
    BOOST_CHECK_EQUAL(q3.GetSize(), 1);
}

// 2. Тест логики Push, Pop и Get (FIFO)
BOOST_AUTO_TEST_CASE(PushPopGetTest) {
    Queue<int> q(5);

    q.QPUSH(10);
    q.QPUSH(20);
    q.QPUSH(30);

    BOOST_CHECK_EQUAL(q.GetSize(), 3);
    BOOST_CHECK_EQUAL(q.empty(), false);

    // Check QGET (не удаляет)
    BOOST_CHECK_EQUAL(q.QGET(), 10);
    BOOST_CHECK_EQUAL(q.GetSize(), 3);

    // Check QPOP (удаляет)
    BOOST_CHECK_EQUAL(q.QPOP(), 10);
    BOOST_CHECK_EQUAL(q.QPOP(), 20);
    BOOST_CHECK_EQUAL(q.GetSize(), 1);
    
    // Добавляем еще, чтобы проверить смешанный режим
    q.QPUSH(40);
    BOOST_CHECK_EQUAL(q.QPOP(), 30);
    BOOST_CHECK_EQUAL(q.QPOP(), 40);
    
    BOOST_CHECK(q.empty());
}

// 3. Тест расширения массива (Resize)
BOOST_AUTO_TEST_CASE(ResizeTest) {
    // Создаем маленькую очередь
    Queue<int> q(2);

    // Заполняем полностью
    q.QPUSH(1);
    q.QPUSH(2);
    
    // Добавляем элемент, вызывая resize()
    q.QPUSH(3); 
    q.QPUSH(4);
    q.QPUSH(5);

    BOOST_CHECK_EQUAL(q.GetSize(), 5);
    
    // Проверяем целостность данных после ресайза
    BOOST_CHECK_EQUAL(q.QPOP(), 1);
    BOOST_CHECK_EQUAL(q.QPOP(), 2);
    BOOST_CHECK_EQUAL(q.QPOP(), 3);
    BOOST_CHECK_EQUAL(q.QPOP(), 4);
    BOOST_CHECK_EQUAL(q.QPOP(), 5);
}

// 4. Тест кольцевого буфера (Circular Buffer Logic)
BOOST_AUTO_TEST_CASE(CircularBufferTest) {
    // Емкость 3
    Queue<int> q(3);

    q.QPUSH(1);
    q.QPUSH(2);
    q.QPUSH(3);
    
    // Сейчас массив полон. Tail за последним элементом.
    
    // Удаляем два элемента. Head сдвигается вперед.
    q.QPOP(); // 1
    q.QPOP(); // 2
    
    // Теперь Head в середине. Добавляем элементы. Tail должен уйти в начало массива (wrap around).
    q.QPUSH(4); 
    q.QPUSH(5); 
    
    // Структура в памяти (примерно): [4, 5, 3] (Head указывает на 3)
    
    BOOST_CHECK_EQUAL(q.GetSize(), 3);
    BOOST_CHECK_EQUAL(q.QPOP(), 3);
    BOOST_CHECK_EQUAL(q.QPOP(), 4);
    BOOST_CHECK_EQUAL(q.QPOP(), 5);
}

// 5. Тест исключений (Exceptions)
BOOST_AUTO_TEST_CASE(ExceptionTest) {
    Queue<int> q;

    // Pop из пустой очереди
    BOOST_CHECK_THROW(q.QPOP(), std::out_of_range);
    
    // Get из пустой очереди
    BOOST_CHECK_THROW(q.QGET(), std::out_of_range);
}

// 6. Тест копирования и присваивания (Rule of Three)
BOOST_AUTO_TEST_CASE(CopyAndAssignTest) {
    Queue<int> original(5);
    original.QPUSH(100);
    original.QPUSH(200);

    // 1. Копирующий конструктор
    Queue<int> copyConstructed(original);
    
    BOOST_CHECK_EQUAL(copyConstructed.GetSize(), 2);
    BOOST_CHECK_EQUAL(copyConstructed.QGET(), 100);

    // Модифицируем копию - оригинал не должен измениться
    copyConstructed.QPOP();
    BOOST_CHECK_EQUAL(copyConstructed.GetSize(), 1);
    BOOST_CHECK_EQUAL(original.GetSize(), 2);

    // 2. Оператор присваивания
    Queue<int> assigned;
    assigned = original;

    BOOST_CHECK_EQUAL(assigned.GetSize(), 2);
    BOOST_CHECK_EQUAL(assigned.QPOP(), 100);
    
    // 3. Самоприсваивание
    original = original;
    BOOST_CHECK_EQUAL(original.GetSize(), 2);
    BOOST_CHECK_EQUAL(original.QPOP(), 100);
}

// 7. Тест сохранения и загрузки (Текстовый режим)
BOOST_AUTO_TEST_CASE(FileIOSaveLoadTest) {
    std::string filename = "queue_test_text.txt";
    
    {
        Queue<std::string> q;
        q.QPUSH("Hello");
        q.QPUSH("World");
        q.QPUSH("Boost");
        
        // Save
        q.QSAVE(filename);
    } // q destroyed here

    {
        Queue<std::string> qLoaded;
        qLoaded.QLOAD(filename);
        
        BOOST_CHECK_EQUAL(qLoaded.GetSize(), 3);
        BOOST_CHECK_EQUAL(qLoaded.QPOP(), "Hello");
        BOOST_CHECK_EQUAL(qLoaded.QPOP(), "World");
        BOOST_CHECK_EQUAL(qLoaded.QPOP(), "Boost");
    }

    // Очистка
    std::remove(filename.c_str());
}

// 8. Тест ошибок при загрузке файла
BOOST_AUTO_TEST_CASE(FileIOErrorTest) {
    Queue<int> q;
    // Несуществующий файл
    BOOST_CHECK_THROW(q.QLOAD("non_existent_file.txt"), std::runtime_error);
    
    // Создаем битый файл (заголовок говорит 10 элементов, а данных нет)
    std::ofstream badFile("bad_queue.txt");
    badFile << "10" << std::endl; // size = 10
    badFile.close();

    BOOST_CHECK_THROW(q.QLOAD("bad_queue.txt"), std::runtime_error);
    std::remove("bad_queue.txt");
}

// 9. Тест бинарного сохранения и загрузки
// ВАЖНО: Бинарный режим в вашем коде использует reinterpret_cast и запись байтов.
// Это безопасно ТОЛЬКО для POD-типов (int, float, char), но НЕ для std::string или классов с указателями.
BOOST_AUTO_TEST_CASE(BinaryFileIOTest) {
    std::string filename = "queue_test_bin.dat";
    
    {
        Queue<double> q;
        q.QPUSH(3.14);
        q.QPUSH(2.71);
        q.QSAVE_BINARY(filename);
    }

    {
        Queue<double> qLoaded;
        qLoaded.QLOAD_BINARY(filename);
        
        BOOST_CHECK_EQUAL(qLoaded.GetSize(), 2);
        BOOST_CHECK_CLOSE(qLoaded.QPOP(), 3.14, 0.001);
        BOOST_CHECK_CLOSE(qLoaded.QPOP(), 2.71, 0.001);
    }
    
    std::remove(filename.c_str());
}

// 10. Тест функции PRINT (вызов, чтобы убедиться, что не падает)
BOOST_AUTO_TEST_CASE(PrintTest) {
    Queue<int> q;
    // Тест пустого вывода
    q.PRINT();
    
    q.QPUSH(1);
    q.QPUSH(2);
    // Тест непустого вывода
    q.PRINT();
    
    // Так как функция пишет в cout, мы просто проверяем, что она отрабатывает без исключений.
    BOOST_CHECK(true);
}

// 11. Тест на работу с другими типами (Template Test)
BOOST_AUTO_TEST_CASE(StringQueueTest) {
    Queue<std::string> q;
    q.QPUSH("Test");
    q.QPUSH("String");
    
    BOOST_CHECK_EQUAL(q.QGET(), "Test");
    BOOST_CHECK_EQUAL(q.QPOP(), "Test");
    BOOST_CHECK_EQUAL(q.QPOP(), "String");
}

BOOST_AUTO_TEST_SUITE_END()