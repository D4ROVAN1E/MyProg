#define BOOST_TEST_MODULE QueueTests
#include <boost/test/included/unit_test.hpp>

#include "queue.hpp"
#include <string>
#include <vector>
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

BOOST_AUTO_TEST_SUITE(QueueTestSuite)

// Тест конструкторов и базового состояния
BOOST_AUTO_TEST_CASE(ConstructorTest) {
    // Конструктор по умлочанию
    Queue<int> q1;
    BOOST_CHECK(q1.empty());
    BOOST_CHECK_EQUAL(q1.GetSize(), 0);

    // Параметризованный конструктор
    Queue<int> q2(10);
    BOOST_CHECK(q2.empty());
    BOOST_CHECK_EQUAL(q2.GetSize(), 0);

    // Обрабатываемый случай нулевого размера
    Queue<int> q3(0);
    q3.QPUSH(5);
    BOOST_CHECK_EQUAL(q3.GetSize(), 1);
}

// Тест логики Push, Pop и Get 
BOOST_AUTO_TEST_CASE(PushPopGetTest) {
    Queue<int> q(5);

    q.QPUSH(10);
    q.QPUSH(20);
    q.QPUSH(30);

    BOOST_CHECK_EQUAL(q.GetSize(), 3);
    BOOST_CHECK_EQUAL(q.empty(), false);

    // Тест QGET (не удаляет)
    BOOST_CHECK_EQUAL(q.QGET(), 10);
    BOOST_CHECK_EQUAL(q.GetSize(), 3);

    // Тест QPOP (удаляет)
    BOOST_CHECK_EQUAL(q.QPOP(), 10);
    BOOST_CHECK_EQUAL(q.QPOP(), 20);
    BOOST_CHECK_EQUAL(q.GetSize(), 1);
    
    // Добавляем еще, чтобы проверить смешанный режим
    q.QPUSH(40);
    BOOST_CHECK_EQUAL(q.QPOP(), 30);
    BOOST_CHECK_EQUAL(q.QPOP(), 40);
    
    BOOST_CHECK(q.empty());
}

// Тест расширения массива (Resize)
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

// Тест кольцевого буфера 
BOOST_AUTO_TEST_CASE(CircularBufferTest) {
    // Емкость 3
    Queue<int> q(3);

    q.QPUSH(1);
    q.QPUSH(2);
    q.QPUSH(3);
    
    // Сейчас массив полон. Tail за последним элементом
    
    // Удаляем два элемента. Head сдвигается вперед
    q.QPOP(); // 1
    q.QPOP(); // 2
    
    // Теперь Head в середине. Добавляем элементы. Tail должен уйти в начало массива 
    q.QPUSH(4); 
    q.QPUSH(5); 
    
    BOOST_CHECK_EQUAL(q.GetSize(), 3);
    BOOST_CHECK_EQUAL(q.QPOP(), 3);
    BOOST_CHECK_EQUAL(q.QPOP(), 4);
    BOOST_CHECK_EQUAL(q.QPOP(), 5);
}

// Тест исключений (Exceptions)
BOOST_AUTO_TEST_CASE(ExceptionTest) {
    Queue<int> q;

    // Pop из пустой очереди
    BOOST_CHECK_THROW(q.QPOP(), out_of_range);
    
    // Get из пустой очереди
    BOOST_CHECK_THROW(q.QGET(), out_of_range);
}

// Тест копирования и присваивания 
BOOST_AUTO_TEST_CASE(CopyAndAssignTest) {
    Queue<int> original(5);
    original.QPUSH(100);
    original.QPUSH(200);

    // Копирующий конструктор
    Queue<int> copyConstructed(original);
    
    BOOST_CHECK_EQUAL(copyConstructed.GetSize(), 2);
    BOOST_CHECK_EQUAL(copyConstructed.QGET(), 100);

    // Модифицируем копию - оригинал не должен измениться
    copyConstructed.QPOP();
    BOOST_CHECK_EQUAL(copyConstructed.GetSize(), 1);
    BOOST_CHECK_EQUAL(original.GetSize(), 2);

    // Оператор присваивания
    Queue<int> assigned;
    assigned = original;

    BOOST_CHECK_EQUAL(assigned.GetSize(), 2);
    BOOST_CHECK_EQUAL(assigned.QPOP(), 100);
    
    // Самоприсваивание
    original = original;
    BOOST_CHECK_EQUAL(original.GetSize(), 2);
    BOOST_CHECK_EQUAL(original.QPOP(), 100);
}

// Тест сохранения и загрузки (Текстовый режим)
BOOST_AUTO_TEST_CASE(FileIOSaveLoadTest) {
    string filename = "queue_test_text.txt";
    
    {
        Queue<string> q;
        q.QPUSH("Hello");
        q.QPUSH("World");
        q.QPUSH("Boost");
        
        // Save
        q.QSAVE(filename);
    } 

    {
        Queue<string> qLoaded;
        qLoaded.QLOAD(filename);
        
        BOOST_CHECK_EQUAL(qLoaded.GetSize(), 3);
        BOOST_CHECK_EQUAL(qLoaded.QPOP(), "Hello");
        BOOST_CHECK_EQUAL(qLoaded.QPOP(), "World");
        BOOST_CHECK_EQUAL(qLoaded.QPOP(), "Boost");
    }

    // Очистка
    remove(filename.c_str());
}

// Тест ошибок при загрузке файла
BOOST_AUTO_TEST_CASE(FileIOErrorTest) {
    Queue<int> q;
    // Несуществующий файл
    BOOST_CHECK_THROW(q.QLOAD("non_existent_file.txt"), runtime_error);
    
    // Создаем битый файл (заголовок говорит 10 элементов, а данных нет)
    ofstream badFile("bad_queue.txt");
    badFile << "10" << endl; // size = 10
    badFile.close();

    BOOST_CHECK_THROW(q.QLOAD("bad_queue.txt"), runtime_error);
    remove("bad_queue.txt");
}

// Тест бинарного сохранения и загрузки
BOOST_AUTO_TEST_CASE(BinaryFileIOTest) {
    string filename = "queue_test_bin.dat";
    
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
    
    remove(filename.c_str());
}

// Тест функции PRINT 
BOOST_AUTO_TEST_CASE(PrintTest) {
    CoutRedirect capture;
    Queue<int> q;
    // Тест пустого вывода
    q.PRINT();
    string out1 = capture.getString();
    BOOST_CHECK(out1.find("Содержимое очереди (от головы к хвосту): пусто") != string::npos);
    q.QPUSH(1);
    q.QPUSH(2);
    // Тест непустого вывода
    q.PRINT();
    string out2 = capture.getString();
    BOOST_CHECK(out2.find("Содержимое очереди (от головы к хвосту): 1 2") != string::npos);
}

// Тест на работу с другими типами 
BOOST_AUTO_TEST_CASE(StringQueueTest) {
    Queue<string> q;
    q.QPUSH("Test");
    q.QPUSH("String");
    
    BOOST_CHECK_EQUAL(q.QGET(), "Test");
    BOOST_CHECK_EQUAL(q.QPOP(), "Test");
    BOOST_CHECK_EQUAL(q.QPOP(), "String");
}

BOOST_AUTO_TEST_SUITE_END()