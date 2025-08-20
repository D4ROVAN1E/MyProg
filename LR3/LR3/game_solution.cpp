#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

int n, m;
vector<int> numbers;
vector<int64_t> prefixSum;
vector<int64_t> memo;

// Функция для вычисления максимального преимущества текущего игрока
// над противником, начиная с позиции pos
int64_t solve(int pos) {
    // Базовый случай: все числа уже взяты
    if (pos >= n) {
        return 0;
    }

    // Проверяем, не вычислили ли мы уже этот случай
    if (memo[pos] != LLONG_MIN) {
        return memo[pos];
    }

    int64_t maxAdvantage = LLONG_MIN;
    int64_t currentSum = 0;

    // Пробуем взять от 1 до m чисел
    for (int take = 1; take <= m && pos + take <= n; take++) {
        // Вычисляем сумму взятых чисел
        currentSum = prefixSum[pos + take] - prefixSum[pos];

        // Преимущество = сумма взятых чисел - преимущество противника
        // на оставшихся числах
        int64_t advantage = currentSum - solve(pos + take);

        maxAdvantage = max(maxAdvantage, advantage);
    }

    memo[pos] = maxAdvantage;
    return maxAdvantage;
}

int main() {
    cin >> n >> m;

    numbers.resize(n);
    prefixSum.resize(n + 1, 0);
    memo.resize(n, LLONG_MIN);

    // Читаем числа и вычисляем префиксные суммы
    for (int i = 0; i < n; i++) {
        cin >> numbers[i];
        prefixSum[i + 1] = prefixSum[i] + numbers[i];
    }

    // Вычисляем преимущество Павла
    int64_t pavelAdvantage = solve(0);

    // Если преимущество положительное, то Павел выигрывает
    if (pavelAdvantage > 0) {
        cout << 1 << endl;
    }
    else {
        cout << 0 << endl;
    }

    return 0;
}