#include <iostream>
#include <cstdint>
#include <vector>
#include <chrono>

class XorshiftGenerator {
private:
    uint32_t state;

public:
    // Конструктор с начальным значением (seed)
    explicit XorshiftGenerator(uint32_t seed = 1) : state(seed) {
        // Убеждаемся, что состояние не равно 0 (это приведет к зацикливанию на 0)
        if (state == 0) {
            state = 1;
        }
    }

    // Основной метод генерации следующего псевдослучайного числа
    uint32_t next() {
        // Xorshift алгоритм с параметрами (13, 17, 5)
        state ^= state << 13;   // Сдвиг влево на 13 и XOR
        state ^= state >> 17;   // Сдвиг вправо на 17 и XOR
        state ^= state << 5;    // Сдвиг влево на 5 и XOR
        return state;
    }

    // Генерация числа в диапазоне [0, max)
    uint32_t nextInRange(uint32_t max) {
        return next() % max;
    }

    // Установка нового seed
    void setSeed(uint32_t seed) {
        state = (seed == 0) ? 1 : seed;
    }

    // Получение текущего состояния
    uint32_t getState() const {
        return state;
    }
};

// Функция для тестирования генератора
void testGenerator() {
    std::cout << "=== Тестирование Xorshift генератора ===" << std::endl;

    // Создаем генератор с текущим временем как seed
    auto seed = static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    XorshiftGenerator rng(seed);

    std::cout << "Seed: " << seed << std::endl << std::endl;

    // Генерируем несколько случайных чисел
    std::cout << "10 случайных чисел (uint32):" << std::endl;
    for (int i = 0; i < 10; ++i) {
        std::cout << rng.next() << " ";
    }
    std::cout << std::endl << std::endl;

    // Генерируем числа в диапазоне
    std::cout << "10 случайных чисел в диапазоне [0, 100):" << std::endl;
    for (int i = 0; i < 10; ++i) {
        std::cout << rng.nextInRange(100) << " ";
    }
    std::cout << std::endl << std::endl;
}

int main() {
    setlocale(LC_ALL, "ru");
    std::cout << "Реализация алгоритма Xorshift" << std::endl;
    std::cout << "==============================" << std::endl << std::endl;

    // Запускаем тесты
    testGenerator();

    // Демонстрация воспроизводимости с одинаковым seed
    std::cout << "\n=== Демонстрация воспроизводимости ===" << std::endl;
    uint32_t testSeed = 42;

    XorshiftGenerator rng1(testSeed);
    XorshiftGenerator rng2(testSeed);

    std::cout << "Два генератора с одинаковым seed (" << testSeed << "):" << std::endl;
    std::cout << "Генератор 1: ";
    for (int i = 0; i < 5; ++i) {
        std::cout << rng1.next() << " ";
    }
    std::cout << std::endl;

    std::cout << "Генератор 2: ";
    for (int i = 0; i < 5; ++i) {
        std::cout << rng2.next() << " ";
    }
    std::cout << std::endl;

    return 0;
}