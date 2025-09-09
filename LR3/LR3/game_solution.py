import sys

n = 0
m = 0
numbers = []
prefixSum = []
memo = []

# Функция для вычисления максимального преимущества текущего игрока
# над противником, начиная с позиции pos
def solve(pos):
    global n, m, numbers, prefixSum, memo
    
    # Базовый случай: все числа уже взяты
    if pos >= n:
        return 0
    
    # Проверяем, не вычислили ли мы уже этот случай
    if memo[pos] != float('-inf'):
        return memo[pos]
    
    maxAdvantage = float('-inf')
    currentSum = 0
    
    # Пробуем взять от 1 до m чисел
    take = 1
    while take <= m and pos + take <= n:
        # Вычисляем сумму взятых чисел
        currentSum = prefixSum[pos + take] - prefixSum[pos]
        
        # Преимущество = сумма взятых чисел - преимущество противника
        # на оставшихся числах
        advantage = currentSum - solve(pos + take)
        maxAdvantage = max(maxAdvantage, advantage)
        take += 1
    
    memo[pos] = maxAdvantage
    return maxAdvantage

def main():
    global n, m, numbers, prefixSum, memo
    
    n, m = map(int, input().split())
    numbers = [0] * n
    prefixSum = [0] * (n + 1)
    memo = [float('-inf')] * n
    
    # Читаем числа и вычисляем префиксные суммы
    nums = list(map(int, input().split()))
    for i in range(n):
        numbers[i] = nums[i]
        prefixSum[i + 1] = prefixSum[i] + numbers[i]
    
    # Вычисляем преимущество Павла
    pavelAdvantage = solve(0)
    
    # Если преимущество положительное, то Павел выигрывает
    if pavelAdvantage > 0:
        print(1)
    else:
        print(0)

if __name__ == "__main__":
    main()