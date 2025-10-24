#include <iostream>
#include "array.hpp"
#include "set.hpp"

using namespace std;

struct Turtle {
    uint32_t ahead;   // черепах впереди
    uint32_t behind;  // черепах позади

    Turtle() : ahead(0), behind(0) {}
    Turtle(uint32_t a, uint32_t b) : ahead(a), behind(b) {}
};

int main() {
    uint32_t N;
    cin >> N;

    Array<Turtle> turtles;

    // Читаем данные о черепахах
    for (uint32_t i = 0; i < N; i++) {
        uint32_t a, b;
        cin >> a >> b;
        MPUSH_BACK(turtles, Turtle(a, b));
    }

    uint32_t maxTruthful = 0;

    // Перебираем все возможные значения K (реальное количество черепах на дороге)
    // K может быть от 1 до N
    for (uint32_t K = 1; K <= N; K++) {
        HashSet<uint32_t> usedPositions(K * 2);
        uint32_t truthfulCount = 0;

        // Проверяем каждую черепаху
        for (uint32_t i = 0; i < N; i++) {
            Turtle t = MGET_BY_IND(turtles, i);

            // Проверяем, может ли эта черепаха говорить правду при K черепахах на дороге
            // Условие: ahead + behind + 1 = K
            if (t.ahead + t.behind + 1 == K) {
                // Позиция черепахи = количество черепах впереди
                uint32_t position = t.ahead;

                // Проверяем, что позиция валидна (0 <= position < K)
                if (position < K) {
                    // Проверяем, что эта позиция еще не занята
                    if (!usedPositions.SET_AT(position)) {
                        usedPositions.SETADD(position);
                        truthfulCount++;
                    }
                }
            }
        }

        // Обновляем максимум
        if (truthfulCount > maxTruthful) {
            maxTruthful = truthfulCount;
        }
    }

    cout << maxTruthful << endl;

    return 0;
}