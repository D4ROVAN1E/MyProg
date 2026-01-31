#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore>
#include <barrier>
#include <atomic>
#include <chrono>
#include <random>
#include <functional>
#include <iomanip>
#include <string>
#include <condition_variable>

using namespace std;

// Эмуляция вспомогательных инструментов

// Аналог StopWatch
class StopWatch {
    using Clock = chrono::high_resolution_clock;
    chrono::time_point<Clock> start_time;
public:
    void Start() { start_time = Clock::now(); }
    double Stop() {
        auto end_time = Clock::now();
        return chrono::duration<double, milli>(end_time - start_time).count();
    }
};

// Генератор случайных символов
char getRandomChar() {
    static thread_local mt19937 generator(hash<thread::id>{}(this_thread::get_id()));
    uniform_int_distribution<int> distribution(33, 126); // ASCII printable
    return static_cast<char>(distribution(generator));
}

// Примитивы синхронизации

// Mutex
mutex mtx;

// Semaphore (C++20)
// Binary semaphore работает как мьютекс
binary_semaphore sem(1); 

// SpinLock (Реализация на атомиках)
class SpinLock {
    atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag.test_and_set(memory_order_acquire)) {
            // Busy wait
            flag.wait(true, memory_order_relaxed);
        }
    }
    void unlock() {
        flag.clear(memory_order_release);
        flag.notify_one();
    }
};
SpinLock spinLock;

// Monitor (Mutex + Condition Variable)
class MonitorCounter {
    int count = 0;   // Охраняемые данные
    mutex mtx;   // Мьютекс для защиты данных
    condition_variable cv;   // Условная переменная для сигналов
    bool busy = false;   // Предикат (условие занятости)

public:
    // Метод безопасного инкремента
    void increment() {
        // Захватываем мьютекс через unique_lock 
        // (unique_lock нужен для cv.wait)
        unique_lock<mutex> lock(mtx);

        // Wait
        // Если busy == true, поток засыпает на этой строке и отпускает мьютекс.
        // Когда другой поток сделает notify, этот проснется, 
        // снова захватит мьютекс и проверит условие (!busy).
        cv.wait(lock, [this] { return !busy; });

        // Меняем состояние
        busy = true;

        count++;

        // Освобождаем ресурс 
        busy = false;

        // Сначала разблокируем мьютекс, чтобы проснувшийся поток не уперся сразу в закрытый мьютекс.
        lock.unlock();

        // Будим один из ждущих потоков
        cv.notify_one();
    }

    int get_value() const {
        return count;
    }
    
    void reset() {
        count = 0;
        busy = false;
    }
};

// SpinWait
// В чистом виде это просто цикл, но для защиты данных нужен атомик.
class BusySpin {
    atomic<bool> locked{false};
public:
    void lock() {
        while (locked.exchange(true, memory_order_acquire)) {
        }
    }
    void unlock() {
        locked.store(false, memory_order_release);
    }
};
BusySpin busySpin;

// Логика гонки

const int TARGET_COUNT = 1000000; // Общее количество операций
int shared_counter = 0;

// Функция сброса для нового теста
void reset_benchmark() {
    shared_counter = 0;
}

// Бенчмарк

void run_benchmark(string name, function<void()> body) {
    reset_benchmark();
    StopWatch sw;
    sw.Start();
    
    body();
    
    double elapsed = sw.Stop();
    cout << left << setw(20) << name 
              << " | Time: " << fixed << setprecision(2) << elapsed << " ms" 
              << " | Counter: " << shared_counter << endl;
}

int main() {
    const int num_threads = 4;
    // Делим работу поровну, но с запасом, чтобы потоки конкурировали до конца
    int ops_per_thread = TARGET_COUNT / num_threads;

    cout << "Starting Benchmark (Target: " << TARGET_COUNT << " ops, Threads: " << num_threads << ")\n";
    cout << "------------------------------------------------------------\n";

    // Unsafe (No Sync) - теперь выглядит так же, как и остальные
    run_benchmark("No Sync (Unsafe)", [&]() {
        vector<thread> threads;
        for(int i=0; i<num_threads; ++i) 
            threads.emplace_back([&]() {
                for(int j=0; j<ops_per_thread; ++j) {
                    getRandomChar(); // Имитация работы
                    shared_counter++; 
                }
            });
        for(auto& t : threads) t.join();
    });

    // Mutex
    run_benchmark("mutex", [&]() {
        vector<thread> threads;
        for(int i=0; i<num_threads; ++i) 
            threads.emplace_back([&]() {
                for(int j=0; j<ops_per_thread; ++j) {
                    getRandomChar();
                    mtx.lock();
                    shared_counter++;
                    mtx.unlock();
                }
            });
        for(auto& t : threads) t.join();
    });

    // Semaphore (Binary)
    run_benchmark("binary_semaphore", [&]() {
        vector<thread> threads;
        for(int i=0; i<num_threads; ++i) 
            threads.emplace_back([&]() {
                for(int j=0; j<ops_per_thread; ++j) {
                    getRandomChar();
                    sem.acquire();
                    shared_counter++;
                    sem.release();
                }
            });
        for(auto& t : threads) t.join();
    });

    // SpinLock (atomic_flag)
    run_benchmark("SpinLock (Yield)", [&]() {
        vector<thread> threads;
        for(int i=0; i<num_threads; ++i) 
            threads.emplace_back([&]() {
                for(int j=0; j<ops_per_thread; ++j) {
                    getRandomChar();
                    spinLock.lock();
                    shared_counter++;
                    spinLock.unlock();
                }
            });
        for(auto& t : threads) t.join();
    });

    // SpinWait (No Yield)
    run_benchmark("SpinWait (Busy)", [&]() {
        vector<thread> threads;
        for(int i=0; i<num_threads; ++i) 
            threads.emplace_back([&]() {
                for(int j=0; j<ops_per_thread; ++j) {
                    getRandomChar();
                    busySpin.lock();
                    shared_counter++;
                    busySpin.unlock();
                }
            });
        for(auto& t : threads) t.join();
    });

    // Monitor (unique_lock)
    MonitorCounter monitor;
    run_benchmark("Monitor", [&]() {
        vector<thread> threads;
        for(int i=0; i<num_threads; ++i) 
            threads.emplace_back([&]() {
                for(int j=0; j<ops_per_thread; ++j) {
                    getRandomChar();
                    {
                        monitor.increment();
                    }
                }
            });
        for(auto& t : threads) t.join();
        // Передаем результат из монитора в глобальную переменную для вывода
        shared_counter = monitor.get_value();
    });

    // Barrier
    // Барьер не используется для защиты данных,
    // он используется для синхронизации этапов.
    // Здесь мы используем его, чтобы все потоки начали "гонку" одновременно.
    run_benchmark("Barrier + Atomic", [&]() {
        barrier sync_point(num_threads);
        vector<thread> threads;
        atomic<int> barrier_counter = 0;
        for(int i=0; i<num_threads; ++i) {
            threads.emplace_back([&]() {
                // Ждем, пока все потоки будут готовы
                sync_point.arrive_and_wait(); 
                // Здесь должна быть защита данных, барьер не защищает память, а координирует потоки.
                // Используем атомик для скорости в этом тесте.
                for(int j=0; j<ops_per_thread; ++j) {
                    getRandomChar();
                    // Атомарная операция
                    barrier_counter++;
                }
            });
        }
        for(auto& t : threads) t.join();
        shared_counter = barrier_counter;
    });
    return 0;
}